#include "benchmark.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "astar.hpp"
#include "dijkstra.hpp"
#include "generators.hpp"
#include "search_result.hpp"
#include "stats.hpp"

namespace {

using Clock = std::chrono::steady_clock;

// One row of a results CSV. Not every experiment uses every field; unused
// fields are left at their default ("" or 0) and printed as such.
struct BenchmarkRow {
  std::string algorithm;
  int num_vertices = 0;
  long long num_edges = 0;
  double density = 0.0;
  int source = 0;
  int target = 0;
  unsigned seed = 0;
  int repetitions = 0;
  std::string label; // extra context: distance category or heuristic type

  Stats time_ms;

  std::size_t nodes_processed = 0;
  std::size_t nodes_popped = 0;
  std::size_t edges_examined = 0;
  std::size_t relaxations = 0;
  std::size_t queue_pushes = 0;
  std::size_t max_queue_size = 0;

  bool found = false;
  double cost = 0.0;
};

void write_csv(const std::string &path, const std::vector<BenchmarkRow> &rows) {
  // Make sure the destination directory exists instead of assuming it
  // does (a missing "results/" directory would otherwise make the
  // ofstream below silently fail to open on some platforms).
  std::filesystem::path fs_path(path);
  if (fs_path.has_parent_path()) {
    std::filesystem::create_directories(fs_path.parent_path());
  }

  std::ofstream out(path);
  if (!out.is_open()) {
    throw std::runtime_error("nao foi possivel abrir '" + path +
                             "' para escrita");
  }

  out << "algorithm,label,num_vertices,num_edges,density,source,target,seed,"
         "repetitions,"
         "found,cost,"
         "mean_time_ms,median_time_ms,min_time_ms,max_time_ms,stddev_time_ms,"
         "nodes_processed,nodes_popped,edges_examined,relaxations,queue_pushes,"
         "max_queue_size\n";

  out.setf(std::ios::fixed);
  for (const auto &r : rows) {
    out << r.algorithm << ',' << r.label << ',' << r.num_vertices << ','
        << r.num_edges << ',' << std::setprecision(6) << r.density << ','
        << r.source << ',' << r.target << ',' << r.seed << ',' << r.repetitions
        << ',' << (r.found ? 1 : 0) << ',';
    if (r.found) {
      out << std::setprecision(6) << r.cost;
    } // else: leave the cost field empty (unreachable target, no cost to
      // report)
    out << ',' << std::setprecision(6) << r.time_ms.mean << ','
        << r.time_ms.median << ',' << r.time_ms.min << ',' << r.time_ms.max
        << ',' << r.time_ms.stddev << ',' << r.nodes_processed << ','
        << r.nodes_popped << ',' << r.edges_examined << ',' << r.relaxations
        << ',' << r.queue_pushes << ',' << r.max_queue_size << '\n';
  }

  std::cout << "  wrote " << path << " (" << rows.size() << " rows)\n"
            << std::flush;
}

// Times `repetitions` calls to `run_once` (which must execute exactly one
// Dijkstra/A* call and return its SearchResult). Only the algorithm call
// itself is measured; counters are taken from the first run since they are
// deterministic given the same graph/source/target/algorithm.
BenchmarkRow measure(const std::string &algorithm, const std::string &label,
                     int num_vertices, long long num_edges, double density,
                     int source, int target, unsigned seed, int repetitions,
                     const std::function<SearchResult()> &run_once) {
  BenchmarkRow row;
  row.algorithm = algorithm;
  row.label = label;
  row.num_vertices = num_vertices;
  row.num_edges = num_edges;
  row.density = density;
  row.source = source;
  row.target = target;
  row.seed = seed;
  row.repetitions = repetitions;

  std::vector<double> times_ms;
  times_ms.reserve(static_cast<std::size_t>(repetitions));

  SearchResult first_result;
  for (int i = 0; i < repetitions; ++i) {
    auto start = Clock::now();
    SearchResult result = run_once();
    auto end = Clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    times_ms.push_back(ms);
    if (i == 0)
      first_result = result;
  }

  row.time_ms = compute_stats(times_ms);
  row.found = first_result.found;
  row.cost = first_result.cost;
  row.nodes_processed = first_result.nodes_processed;
  row.nodes_popped = first_result.nodes_popped;
  row.edges_examined = first_result.edges_examined;
  row.relaxations = first_result.relaxations;
  row.queue_pushes = first_result.queue_pushes;
  row.max_queue_size = first_result.max_queue_size;

  return row;
}

constexpr int kRepetitions = 30;

void run_scalability(const std::string &results_dir) {
  std::cout << "Running scalability experiment...\n" << std::flush;
  std::vector<int> sizes = {100, 500, 1000, 5000, 10000};
  const int k_nearest = 6;
  const unsigned seed = 42;

  std::vector<BenchmarkRow> rows;
  for (int n : sizes) {
    Graph g = generate_geometric_graph(n, k_nearest, seed);
    int source = 0;
    int target = n - 1;
    double density =
        static_cast<double>(g.num_edges()) / (static_cast<double>(n) * (n - 1));

    rows.push_back(measure("dijkstra", "", n,
                           static_cast<long long>(g.num_edges()), density,
                           source, target, seed, kRepetitions,
                           [&]() { return dijkstra(g, source, target); }));

    Heuristic h = make_euclidean_heuristic(g, target);
    rows.push_back(measure("astar_euclidean", "", n,
                           static_cast<long long>(g.num_edges()), density,
                           source, target, seed, kRepetitions,
                           [&]() { return astar(g, source, target, h); }));

    std::cout << "  n=" << n << " done\n" << std::flush;
  }

  write_csv(results_dir + "/scalability.csv", rows);
}

void run_density(const std::string &results_dir) {
  std::cout << "Running density experiment...\n" << std::flush;
  const int n = 3000;
  std::vector<double> densities = {0.001, 0.005, 0.02, 0.05, 0.1};
  const unsigned seed = 7;

  std::vector<BenchmarkRow> rows;
  for (double density : densities) {
    Graph g = generate_geometric_graph_by_density(n, density, seed);
    int source = 0;
    int target = n - 1;

    rows.push_back(measure("dijkstra", "", n,
                           static_cast<long long>(g.num_edges()), density,
                           source, target, seed, kRepetitions,
                           [&]() { return dijkstra(g, source, target); }));

    Heuristic h = make_euclidean_heuristic(g, target);
    rows.push_back(measure("astar_euclidean", "", n,
                           static_cast<long long>(g.num_edges()), density,
                           source, target, seed, kRepetitions,
                           [&]() { return astar(g, source, target, h); }));

    std::cout << "  density=" << density << " (" << g.num_edges()
              << " directed arcs) done\n"
              << std::flush;
  }

  write_csv(results_dir + "/density.csv", rows);
}

void run_distance(const std::string &results_dir) {
  std::cout << "Running distance experiment...\n" << std::flush;
  const int n = 3000;
  const int k_nearest = 6;
  const unsigned seed = 123;

  Graph g = generate_geometric_graph(n, k_nearest, seed);
  int source = 0;

  // Rank every other vertex by straight-line distance from the source,
  // then pick a near, a medium and a far target from that ranking.
  // Reachability is verified below when computing each row (found=0
  // would show up plainly in the CSV).
  std::vector<std::pair<double, int>> by_distance;
  auto [sx, sy] = g.coordinates[static_cast<std::size_t>(source)];
  for (int v = 0; v < n; ++v) {
    if (v == source)
      continue;
    auto [vx, vy] = g.coordinates[static_cast<std::size_t>(v)];
    double dx = vx - sx, dy = vy - sy;
    by_distance.push_back({std::sqrt(dx * dx + dy * dy), v});
  }
  std::sort(by_distance.begin(), by_distance.end());

  struct Category {
    std::string label;
    int target;
  };
  std::vector<Category> categories = {
      {"near",
       by_distance[static_cast<std::size_t>(by_distance.size() * 5 / 100)]
           .second},
      {"medium",
       by_distance[static_cast<std::size_t>(by_distance.size() * 50 / 100)]
           .second},
      {"far",
       by_distance[static_cast<std::size_t>(by_distance.size() * 95 / 100)]
           .second},
  };

  double density =
      static_cast<double>(g.num_edges()) / (static_cast<double>(n) * (n - 1));

  std::vector<BenchmarkRow> rows;
  for (const auto &cat : categories) {
    int target = cat.target;

    rows.push_back(measure("dijkstra", cat.label, n,
                           static_cast<long long>(g.num_edges()), density,
                           source, target, seed, kRepetitions,
                           [&]() { return dijkstra(g, source, target); }));

    Heuristic h = make_euclidean_heuristic(g, target);
    rows.push_back(measure("astar_euclidean", cat.label, n,
                           static_cast<long long>(g.num_edges()), density,
                           source, target, seed, kRepetitions,
                           [&]() { return astar(g, source, target, h); }));

    std::cout << "  " << cat.label << " (target=" << target << ") done\n"
              << std::flush;
  }

  write_csv(results_dir + "/distance.csv", rows);
}

void run_heuristic(const std::string &results_dir) {
  std::cout << "Running heuristic-impact experiment...\n" << std::flush;
  const int n = 3000;
  const int k_nearest = 6;
  const unsigned seed = 99;

  Graph g = generate_geometric_graph(n, k_nearest, seed);
  int source = 0;
  int target = n - 1;
  double density =
      static_cast<double>(g.num_edges()) / (static_cast<double>(n) * (n - 1));

  std::vector<BenchmarkRow> rows;

  rows.push_back(measure("dijkstra", "n/a", n,
                         static_cast<long long>(g.num_edges()), density, source,
                         target, seed, kRepetitions,
                         [&]() { return dijkstra(g, source, target); }));

  Heuristic h0 = zero_heuristic();
  rows.push_back(measure("astar_h0", "h(n)=0", n,
                         static_cast<long long>(g.num_edges()), density, source,
                         target, seed, kRepetitions,
                         [&]() { return astar(g, source, target, h0); }));

  Heuristic he = make_euclidean_heuristic(g, target);
  rows.push_back(measure("astar_euclidean", "euclidean", n,
                         static_cast<long long>(g.num_edges()), density, source,
                         target, seed, kRepetitions,
                         [&]() { return astar(g, source, target, he); }));

  write_csv(results_dir + "/heuristic.csv", rows);
}

} // namespace

bool run_benchmark_experiment(const std::string &name,
                              const std::string &results_dir) {
  if (name == "scalability") {
    run_scalability(results_dir);
  } else if (name == "density") {
    run_density(results_dir);
  } else if (name == "distance") {
    run_distance(results_dir);
  } else if (name == "heuristic") {
    run_heuristic(results_dir);
  } else {
    return false;
  }
  return true;
}

void run_all_benchmark_experiments(const std::string &results_dir) {
  run_scalability(results_dir);
  run_density(results_dir);
  run_distance(results_dir);
  run_heuristic(results_dir);
}

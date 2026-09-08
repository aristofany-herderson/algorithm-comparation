#include "generators.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_set>
#include <vector>

namespace {

std::uint64_t encode_pair(int u, int v) {
  if (u > v)
    std::swap(u, v);
  return (static_cast<std::uint64_t>(u) << 32) | static_cast<std::uint32_t>(v);
}

} // namespace

Graph generate_random_graph(int num_vertices, long long num_edges,
                            unsigned seed, double min_weight,
                            double max_weight) {
  Graph graph(num_vertices);
  if (num_vertices < 2)
    return graph;

  std::mt19937_64 rng(seed);
  std::uniform_int_distribution<int> vertex_dist(0, num_vertices - 1);
  std::uniform_real_distribution<double> weight_dist(min_weight, max_weight);

  long long max_possible =
      static_cast<long long>(num_vertices) * (num_vertices - 1) / 2;
  num_edges = std::min(num_edges, max_possible);

  std::unordered_set<std::uint64_t> existing;
  existing.reserve(static_cast<std::size_t>(num_edges) * 2);

  long long added = 0;
  long long attempts = 0;
  long long max_attempts = num_edges * 20 + 1000;

  while (added < num_edges && attempts < max_attempts) {
    attempts++;
    int u = vertex_dist(rng);
    int v = vertex_dist(rng);
    if (u == v)
      continue;
    std::uint64_t key = encode_pair(u, v);
    if (existing.count(key))
      continue;
    existing.insert(key);
    graph.add_undirected_edge(u, v, weight_dist(rng));
    added++;
  }

  return graph;
}

Graph generate_random_graph_by_density(int num_vertices, double density,
                                       unsigned seed, double min_weight,
                                       double max_weight) {
  long long max_possible =
      static_cast<long long>(num_vertices) * (num_vertices - 1) / 2;
  long long num_edges = static_cast<long long>(
      std::round(density * static_cast<double>(max_possible)));
  return generate_random_graph(num_vertices, num_edges, seed, min_weight,
                               max_weight);
}

Graph generate_geometric_graph_by_density(int num_vertices, double density,
                                          unsigned seed, double width,
                                          double height) {
  Graph graph(num_vertices);
  if (num_vertices < 2)
    return graph;

  std::mt19937_64 rng(seed);
  std::uniform_real_distribution<double> x_dist(0.0, width);
  std::uniform_real_distribution<double> y_dist(0.0, height);

  graph.coordinates.resize(static_cast<std::size_t>(num_vertices));
  for (int i = 0; i < num_vertices; ++i) {
    graph.coordinates[static_cast<std::size_t>(i)] = {x_dist(rng), y_dist(rng)};
  }

  long long max_possible =
      static_cast<long long>(num_vertices) * (num_vertices - 1) / 2;
  long long num_edges = static_cast<long long>(
      std::round(density * static_cast<double>(max_possible)));

  std::uniform_int_distribution<int> vertex_dist(0, num_vertices - 1);
  std::unordered_set<std::uint64_t> existing;
  existing.reserve(static_cast<std::size_t>(num_edges) * 2);

  long long added = 0;
  long long attempts = 0;
  long long max_attempts = num_edges * 20 + 1000;

  while (added < num_edges && attempts < max_attempts) {
    attempts++;
    int u = vertex_dist(rng);
    int v = vertex_dist(rng);
    if (u == v)
      continue;
    std::uint64_t key = encode_pair(u, v);
    if (existing.count(key))
      continue;
    existing.insert(key);

    auto [xu, yu] = graph.coordinates[static_cast<std::size_t>(u)];
    auto [xv, yv] = graph.coordinates[static_cast<std::size_t>(v)];
    double dx = xu - xv;
    double dy = yu - yv;
    double dist = std::sqrt(dx * dx + dy * dy);

    graph.add_undirected_edge(u, v, dist);
    added++;
  }

  return graph;
}

Graph generate_geometric_graph(int num_vertices, int k_nearest, unsigned seed,
                               double width, double height) {
  Graph graph(num_vertices);
  if (num_vertices < 2)
    return graph;

  std::mt19937_64 rng(seed);
  std::uniform_real_distribution<double> x_dist(0.0, width);
  std::uniform_real_distribution<double> y_dist(0.0, height);

  graph.coordinates.resize(static_cast<std::size_t>(num_vertices));
  for (int i = 0; i < num_vertices; ++i) {
    graph.coordinates[static_cast<std::size_t>(i)] = {x_dist(rng), y_dist(rng)};
  }

  std::unordered_set<std::uint64_t> existing;

  for (int i = 0; i < num_vertices; ++i) {
    auto [xi, yi] = graph.coordinates[static_cast<std::size_t>(i)];

    std::vector<std::pair<double, int>> distances;
    distances.reserve(static_cast<std::size_t>(num_vertices - 1));
    for (int j = 0; j < num_vertices; ++j) {
      if (j == i)
        continue;
      auto [xj, yj] = graph.coordinates[static_cast<std::size_t>(j)];
      double dx = xi - xj;
      double dy = yi - yj;
      distances.push_back({std::sqrt(dx * dx + dy * dy), j});
    }

    int k = std::min(k_nearest, static_cast<int>(distances.size()));
    std::partial_sort(distances.begin(), distances.begin() + k,
                      distances.end());

    for (int idx = 0; idx < k; ++idx) {
      int j = distances[static_cast<std::size_t>(idx)].second;
      double dist = distances[static_cast<std::size_t>(idx)].first;
      std::uint64_t key = encode_pair(i, j);
      if (existing.count(key))
        continue;
      existing.insert(key);
      graph.add_undirected_edge(i, j, dist);
    }
  }

  return graph;
}

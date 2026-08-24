#include "tests.hpp"

#include <cmath>
#include <cstdio>
#include <random>
#include <vector>

#include "astar.hpp"
#include "dijkstra.hpp"
#include "generators.hpp"
#include "manual_graphs.hpp"

namespace {

constexpr double kCostTolerance = 1e-6;

bool costs_match(double a, double b) {
  return std::fabs(a - b) <= kCostTolerance;
}

// Runs Dijkstra and A* (Euclidean heuristic if geometric, else h=0) on the
// same graph/source/target and checks that they agree on both
// "found" status and total cost. Returns true if the check passes.
bool check_agreement(const char *case_name, const Graph &graph, int source,
                     int target, bool expect_found, double expect_cost) {
  SearchResult dijkstra_result = dijkstra(graph, source, target);

  Heuristic h = graph.is_geometric() ? make_euclidean_heuristic(graph, target)
                                     : zero_heuristic();
  SearchResult astar_result = astar(graph, source, target, h);

  bool ok = true;

  if (dijkstra_result.found != astar_result.found) {
    std::printf("[FAIL] %s: Dijkstra.found=%d but A*.found=%d\n", case_name,
                dijkstra_result.found, astar_result.found);
    ok = false;
  } else if (dijkstra_result.found &&
             !costs_match(dijkstra_result.cost, astar_result.cost)) {
    std::printf("[FAIL] %s: cost mismatch Dijkstra=%.9f A*=%.9f\n", case_name,
                dijkstra_result.cost, astar_result.cost);
    ok = false;
  }

  if (dijkstra_result.found != expect_found) {
    std::printf("[FAIL] %s: expected found=%d, Dijkstra found=%d\n", case_name,
                expect_found, dijkstra_result.found);
    ok = false;
  } else if (expect_found && !costs_match(dijkstra_result.cost, expect_cost)) {
    std::printf("[FAIL] %s: expected cost=%.9f, Dijkstra cost=%.9f\n",
                case_name, expect_cost, dijkstra_result.cost);
    ok = false;
  }

  if (ok) {
    std::printf("[PASS] %s (found=%d, cost=%.4f)\n", case_name,
                dijkstra_result.found,
                dijkstra_result.found ? dijkstra_result.cost : 0.0);
  }

  return ok;
}

bool run_manual_tests() {
  std::printf("-- Manual graph tests --\n");
  bool all_ok = true;

  ManualCase cases[] = {
      make_simple_path_case(),      make_multiple_paths_case(),
      make_disconnected_case(),     make_source_equals_target_case(),
      make_equal_cost_paths_case(),
  };

  for (auto &c : cases) {
    all_ok &= check_agreement(c.name, c.graph, c.source, c.target,
                              c.expected_found, c.expected_cost);
  }

  return all_ok;
}

bool run_randomized_cross_checks() {
  std::printf("-- Randomized cross-check tests (Dijkstra vs A*) --\n");
  bool all_ok = true;

  // Non-geometric random graphs: A* falls back to h(n) = 0.
  {
    Graph g = generate_random_graph(200, 800, /*seed=*/1);
    std::mt19937 rng(1);
    std::uniform_int_distribution<int> pick(0, g.num_vertices() - 1);
    for (int i = 0; i < 10; ++i) {
      int s = pick(rng);
      int t = pick(rng);
      char name[64];
      std::snprintf(name, sizeof(name), "random_graph_pair_%d", i);
      all_ok &= check_agreement(name, g, s, t,
                                /*expect_found=*/dijkstra(g, s, t).found,
                                dijkstra(g, s, t).cost);
    }
  }

  // Geometric random graphs: A* uses the Euclidean heuristic.
  {
    Graph g = generate_geometric_graph(300, 6, /*seed=*/2);
    std::mt19937 rng(2);
    std::uniform_int_distribution<int> pick(0, g.num_vertices() - 1);
    for (int i = 0; i < 10; ++i) {
      int s = pick(rng);
      int t = pick(rng);
      char name[64];
      std::snprintf(name, sizeof(name), "geometric_graph_pair_%d", i);
      all_ok &= check_agreement(name, g, s, t,
                                /*expect_found=*/dijkstra(g, s, t).found,
                                dijkstra(g, s, t).cost);
    }
  }

  return all_ok;
}

} // namespace

bool run_all_tests() {
  bool ok = true;
  ok &= run_manual_tests();
  ok &= run_randomized_cross_checks();

  std::printf("\n%s\n", ok ? "ALL TESTS PASSED" : "SOME TESTS FAILED");
  return ok;
}

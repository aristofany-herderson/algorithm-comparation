#pragma once

#include "graph.hpp"

// Small, hand-built graphs used to validate that Dijkstra and A* agree on
// correctness before any benchmarking is done. Each function also returns
// the intended source/target and, where relevant, the expected cost via
// out parameters, so tests can assert against a known-correct answer.

struct ManualCase {
  const char *name;
  Graph graph;
  int source;
  int target;
  bool expected_found;
  double expected_cost; // meaningful only if expected_found is true
};

// 1. Simple path: 0 -> 1 -> 2 -> 3, single unambiguous shortest path.
ManualCase make_simple_path_case();

// 2. Multiple paths with different costs: the shortest one must win.
ManualCase make_multiple_paths_case();

// 3. Disconnected graph: source and target are in different components.
ManualCase make_disconnected_case();

// 4. Source equal to target: shortest path cost must be 0.
ManualCase make_source_equals_target_case();

// 5. Multiple paths with the same (optimal) cost: cost must match,
//    even though the chosen path may differ between algorithms.
ManualCase make_equal_cost_paths_case();

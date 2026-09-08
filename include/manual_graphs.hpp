#pragma once

#include "graph.hpp"

struct ManualCase {
  const char *name;
  Graph graph;
  int source;
  int target;
  bool expected_found;
  double expected_cost;
};

ManualCase make_simple_path_case();

ManualCase make_multiple_paths_case();

ManualCase make_disconnected_case();

ManualCase make_source_equals_target_case();

ManualCase make_equal_cost_paths_case();

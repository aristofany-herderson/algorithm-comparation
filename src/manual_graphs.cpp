#include "manual_graphs.hpp"

ManualCase make_simple_path_case() {
  Graph g(4);
  g.add_undirected_edge(0, 1, 1.0);
  g.add_undirected_edge(1, 2, 1.0);
  g.add_undirected_edge(2, 3, 1.0);
  return {"simple_path", std::move(g), 0, 3, true, 3.0};
}

ManualCase make_multiple_paths_case() {
  // 0 -> 1 -> 3 costs 1 + 5 = 6
  // 0 -> 2 -> 3 costs 2 + 2 = 4  (this is the shortest one)
  Graph g(4);
  g.add_undirected_edge(0, 1, 1.0);
  g.add_undirected_edge(1, 3, 5.0);
  g.add_undirected_edge(0, 2, 2.0);
  g.add_undirected_edge(2, 3, 2.0);
  return {"multiple_paths", std::move(g), 0, 3, true, 4.0};
}

ManualCase make_disconnected_case() {
  // Two separate components: {0,1,2} and {3,4}.
  Graph g(5);
  g.add_undirected_edge(0, 1, 1.0);
  g.add_undirected_edge(1, 2, 1.0);
  g.add_undirected_edge(3, 4, 1.0);
  return {"disconnected", std::move(g), 0, 4, false, 0.0};
}

ManualCase make_source_equals_target_case() {
  Graph g(3);
  g.add_undirected_edge(0, 1, 1.0);
  g.add_undirected_edge(1, 2, 1.0);
  return {"source_equals_target", std::move(g), 1, 1, true, 0.0};
}

ManualCase make_equal_cost_paths_case() {
  // Two disjoint paths from 0 to 3, both costing exactly 4.
  // 0 -> 1 -> 3 : 2 + 2 = 4
  // 0 -> 2 -> 3 : 2 + 2 = 4
  Graph g(4);
  g.add_undirected_edge(0, 1, 2.0);
  g.add_undirected_edge(1, 3, 2.0);
  g.add_undirected_edge(0, 2, 2.0);
  g.add_undirected_edge(2, 3, 2.0);
  return {"equal_cost_paths", std::move(g), 0, 3, true, 4.0};
}

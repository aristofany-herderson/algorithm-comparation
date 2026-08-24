#pragma once

#include <functional>

#include "graph.hpp"
#include "search_result.hpp"

// Heuristic function: given a vertex id, returns an estimate h(n) of the
// remaining cost to the target. Passing a heuristic that always returns 0
// makes A* behave like Dijkstra (see README, "Impact of the heuristic").
using Heuristic = std::function<double(int)>;

// A* shortest path using std::priority_queue ordered by f(n) = g(n) + h(n).
// Same lazy-deletion strategy as the Dijkstra implementation so that the
// two are structurally comparable.
SearchResult astar(const Graph &graph, int source, int target,
                   const Heuristic &heuristic);

// Builds an admissible Euclidean-distance heuristic for a geometric graph
// (requires graph.is_geometric()). h(n) = euclidean_distance(n, target).
Heuristic make_euclidean_heuristic(const Graph &graph, int target);

// Heuristic that always returns 0 (used to show A* degenerating to Dijkstra).
Heuristic zero_heuristic();

#pragma once

#include <functional>

#include "graph.hpp"
#include "search_result.hpp"

using Heuristic = std::function<double(int)>;

SearchResult astar(const Graph &graph, int source, int target,
                   const Heuristic &heuristic);

Heuristic make_euclidean_heuristic(const Graph &graph, int target);

Heuristic zero_heuristic();

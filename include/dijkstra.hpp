#pragma once

#include "graph.hpp"
#include "search_result.hpp"

SearchResult dijkstra(const Graph &graph, int source, int target);

#pragma once

#include "graph.hpp"
#include "search_result.hpp"

// Classic Dijkstra shortest path using std::priority_queue (binary heap)
// with the standard "lazy deletion" strategy: stale entries are left in
// the queue and skipped when popped, instead of being removed eagerly.
SearchResult dijkstra(const Graph &graph, int source, int target);

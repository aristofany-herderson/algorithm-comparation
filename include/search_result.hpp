#pragma once

#include <cstddef>
#include <limits>
#include <vector>

// Result of a shortest-path search, plus instrumentation counters used to
// compare Dijkstra and A* on equal footing.
//
// Counter definitions (identical semantics for both algorithms):
//   nodes_popped     - number of times a vertex was popped from the
//                       priority queue (includes stale/duplicate pops).
//   nodes_processed   - number of vertices actually expanded, i.e. popped
//                       AND not already finalized with a better distance
//                       (stale duplicates popped from the queue are not
//                       counted here).
//   edges_examined    - number of adjacency-list edges looked at while
//                       expanding processed vertices.
//   relaxations        - number of edges for which a strictly better
//                       distance was found and applied.
//   queue_pushes       - number of push operations into the priority queue.
//   max_queue_size     - largest size the priority queue reached.
struct SearchResult {
  bool found = false;
  double cost = std::numeric_limits<double>::infinity();
  std::vector<int> path;

  std::size_t nodes_popped = 0;
  std::size_t nodes_processed = 0;
  std::size_t edges_examined = 0;
  std::size_t relaxations = 0;
  std::size_t queue_pushes = 0;
  std::size_t max_queue_size = 0;
};

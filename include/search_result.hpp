#pragma once

#include <cstddef>
#include <limits>
#include <vector>

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

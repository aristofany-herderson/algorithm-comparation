#include "dijkstra.hpp"

#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

namespace {

using QueueItem = std::pair<double, int>;

struct CompareByDistance {
  bool operator()(const QueueItem &a, const QueueItem &b) const {
    return a.first > b.first;
  }
};

} // namespace

SearchResult dijkstra(const Graph &graph, int source, int target) {
  SearchResult result;
  const int n = graph.num_vertices();
  if (source < 0 || source >= n || target < 0 || target >= n) {
    return result;
  }

  const double INF = std::numeric_limits<double>::infinity();
  std::vector<double> dist(static_cast<std::size_t>(n), INF);
  std::vector<int> parent(static_cast<std::size_t>(n), -1);
  std::vector<char> finalized(static_cast<std::size_t>(n), 0);

  std::priority_queue<QueueItem, std::vector<QueueItem>, CompareByDistance> pq;

  dist[static_cast<std::size_t>(source)] = 0.0;
  pq.push({0.0, source});
  result.queue_pushes++;
  result.max_queue_size = std::max(result.max_queue_size, pq.size());

  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();
    result.nodes_popped++;

    if (finalized[static_cast<std::size_t>(u)]) {
      continue;
    }
    finalized[static_cast<std::size_t>(u)] = 1;
    result.nodes_processed++;

    if (u == target) {
      break;
    }

    for (const auto &edge : graph.adjacency[static_cast<std::size_t>(u)]) {
      result.edges_examined++;
      if (finalized[static_cast<std::size_t>(edge.to)])
        continue;

      double candidate = d + edge.weight;
      if (candidate < dist[static_cast<std::size_t>(edge.to)]) {
        dist[static_cast<std::size_t>(edge.to)] = candidate;
        parent[static_cast<std::size_t>(edge.to)] = u;
        result.relaxations++;
        pq.push({candidate, edge.to});
        result.queue_pushes++;
        result.max_queue_size = std::max(result.max_queue_size, pq.size());
      }
    }
  }

  if (finalized[static_cast<std::size_t>(target)]) {
    result.found = true;
    result.cost = dist[static_cast<std::size_t>(target)];
    std::vector<int> path;
    for (int v = target; v != -1; v = parent[static_cast<std::size_t>(v)]) {
      path.push_back(v);
    }
    std::reverse(path.begin(), path.end());
    result.path = std::move(path);
  }

  return result;
}

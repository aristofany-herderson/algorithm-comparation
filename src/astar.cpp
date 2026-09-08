#include "astar.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <vector>

namespace {

struct QueueItem {
  double f;
  int vertex;
  double g;
};

struct CompareByF {
  bool operator()(const QueueItem &a, const QueueItem &b) const {
    return a.f > b.f;
  }
};

} // namespace

Heuristic zero_heuristic() {
  return [](int) { return 0.0; };
}

Heuristic make_euclidean_heuristic(const Graph &graph, int target) {
  const double target_x =
      graph.coordinates[static_cast<std::size_t>(target)].first;
  const double target_y =
      graph.coordinates[static_cast<std::size_t>(target)].second;

  const std::vector<std::pair<double, double>> *coords = &graph.coordinates;

  return [coords, target_x, target_y](int v) {
    double dx = (*coords)[static_cast<std::size_t>(v)].first - target_x;
    double dy = (*coords)[static_cast<std::size_t>(v)].second - target_y;
    return std::sqrt(dx * dx + dy * dy);
  };
}

SearchResult astar(const Graph &graph, int source, int target,
                   const Heuristic &heuristic) {
  SearchResult result;
  const int n = graph.num_vertices();
  if (source < 0 || source >= n || target < 0 || target >= n) {
    return result;
  }

  const double INF = std::numeric_limits<double>::infinity();
  std::vector<double> best_g(static_cast<std::size_t>(n), INF);
  std::vector<int> parent(static_cast<std::size_t>(n), -1);
  std::vector<char> finalized(static_cast<std::size_t>(n), 0);

  std::priority_queue<QueueItem, std::vector<QueueItem>, CompareByF> pq;

  best_g[static_cast<std::size_t>(source)] = 0.0;
  pq.push({heuristic(source), source, 0.0});
  result.queue_pushes++;
  result.max_queue_size = std::max(result.max_queue_size, pq.size());

  while (!pq.empty()) {
    QueueItem top = pq.top();
    pq.pop();
    result.nodes_popped++;

    int u = top.vertex;
    double g = top.g;

    if (finalized[static_cast<std::size_t>(u)]) {
      continue;
    }
    if (g > best_g[static_cast<std::size_t>(u)]) {
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

      double candidate_g = g + edge.weight;
      if (candidate_g < best_g[static_cast<std::size_t>(edge.to)]) {
        best_g[static_cast<std::size_t>(edge.to)] = candidate_g;
        parent[static_cast<std::size_t>(edge.to)] = u;
        result.relaxations++;
        double f = candidate_g + heuristic(edge.to);
        pq.push({f, edge.to, candidate_g});
        result.queue_pushes++;
        result.max_queue_size = std::max(result.max_queue_size, pq.size());
      }
    }
  }

  if (finalized[static_cast<std::size_t>(target)]) {
    result.found = true;
    result.cost = best_g[static_cast<std::size_t>(target)];
    std::vector<int> path;
    for (int v = target; v != -1; v = parent[static_cast<std::size_t>(v)]) {
      path.push_back(v);
    }
    std::reverse(path.begin(), path.end());
    result.path = std::move(path);
  }

  return result;
}

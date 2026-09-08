#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

struct Edge {
  int to;
  double weight;
};

struct Graph {
  std::vector<std::vector<Edge>> adjacency;
  std::vector<std::pair<double, double>> coordinates;

  explicit Graph(int num_vertices = 0)
      : adjacency(static_cast<std::size_t>(num_vertices)) {}

  int num_vertices() const { return static_cast<int>(adjacency.size()); }

  std::size_t num_edges() const {
    std::size_t total = 0;
    for (const auto &edges : adjacency)
      total += edges.size();
    return total;
  }

  bool is_geometric() const {
    return coordinates.size() == adjacency.size() && !coordinates.empty();
  }

  void add_undirected_edge(int u, int v, double weight) {
    adjacency[static_cast<std::size_t>(u)].push_back({v, weight});
    adjacency[static_cast<std::size_t>(v)].push_back({u, weight});
  }

  void add_directed_edge(int u, int v, double weight) {
    adjacency[static_cast<std::size_t>(u)].push_back({v, weight});
  }
};

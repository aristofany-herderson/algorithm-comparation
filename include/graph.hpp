#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

// Edge in an adjacency list: destination vertex and non-negative weight.
struct Edge {
  int to;
  double weight;
};

// Simple weighted graph represented as an adjacency list.
// Vertices are identified by integer indices in [0, num_vertices).
// Optionally carries 2D coordinates for each vertex so that a graph can be
// treated as "geometric" (used to build an admissible Euclidean heuristic
// for A*).
struct Graph {
  std::vector<std::vector<Edge>> adjacency;
  std::vector<std::pair<double, double>> coordinates; // empty if not geometric

  explicit Graph(int num_vertices = 0)
      : adjacency(static_cast<std::size_t>(num_vertices)) {}

  int num_vertices() const { return static_cast<int>(adjacency.size()); }

  std::size_t num_edges() const {
    std::size_t total = 0;
    for (const auto &edges : adjacency)
      total += edges.size();
    return total; // directed edge count; undirected graphs store both
                  // directions
  }

  bool is_geometric() const {
    return coordinates.size() == adjacency.size() && !coordinates.empty();
  }

  // Adds an undirected edge (two directed arcs) with the given weight.
  void add_undirected_edge(int u, int v, double weight) {
    adjacency[static_cast<std::size_t>(u)].push_back({v, weight});
    adjacency[static_cast<std::size_t>(v)].push_back({u, weight});
  }

  // Adds a single directed arc u -> v.
  void add_directed_edge(int u, int v, double weight) {
    adjacency[static_cast<std::size_t>(u)].push_back({v, weight});
  }
};

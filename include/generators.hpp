#pragma once

#include <cstdint>

#include "graph.hpp"

// Generates a random undirected weighted graph (no coordinates).
// Approximately `num_edges` distinct undirected edges are added between
// random vertex pairs, with weights drawn uniformly from
// [min_weight, max_weight]. Weights are always positive.
// `num_edges` is a target; a small number of duplicate/self edges are
// skipped during sampling so the actual count may be slightly lower.
Graph generate_random_graph(int num_vertices, long long num_edges,
                            unsigned seed, double min_weight = 1.0,
                            double max_weight = 10.0);

// Generates a random undirected weighted graph from a target density in
// (0, 1], where density = num_edges / max_possible_undirected_edges.
Graph generate_random_graph_by_density(int num_vertices, double density,
                                       unsigned seed, double min_weight = 1.0,
                                       double max_weight = 10.0);

// Generates a geometric graph: `num_vertices` points scattered uniformly
// at random in a [0, width] x [0, height] square. Each vertex is connected
// to its `k_nearest` closest neighbors (by Euclidean distance), and the
// edge weight equals the exact Euclidean distance between the two
// endpoints. Using the true Euclidean distance as the edge weight is what
// makes the straight-line heuristic admissible (see README).
Graph generate_geometric_graph(int num_vertices, int k_nearest, unsigned seed,
                               double width = 1000.0, double height = 1000.0);

// Generates a geometric graph (with coordinates) where edges are sampled
// randomly between pairs of vertices to reach a target density, instead of
// connecting k-nearest-neighbors. Edge weight is still the exact Euclidean
// distance between endpoints, so the straight-line heuristic remains
// admissible regardless of which edges happen to be present. Used in the
// density experiment so that A* can still use a valid heuristic.
Graph generate_geometric_graph_by_density(int num_vertices, double density,
                                          unsigned seed, double width = 1000.0,
                                          double height = 1000.0);

#pragma once

#include <cstdint>

#include "graph.hpp"

Graph generate_random_graph(int num_vertices, long long num_edges,
                            unsigned seed, double min_weight = 1.0,
                            double max_weight = 10.0);

Graph generate_random_graph_by_density(int num_vertices, double density,
                                       unsigned seed, double min_weight = 1.0,
                                       double max_weight = 10.0);

Graph generate_geometric_graph(int num_vertices, int k_nearest, unsigned seed,
                               double width = 1000.0, double height = 1000.0);

Graph generate_geometric_graph_by_density(int num_vertices, double density,
                                          unsigned seed, double width = 1000.0,
                                          double height = 1000.0);

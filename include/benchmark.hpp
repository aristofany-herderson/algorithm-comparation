#pragma once

#include <string>

// Runs one named experiment ("scalability", "density", "distance",
// "heuristic") and writes its CSV to results/<name>.csv.
// Returns false if the name is not recognized.
bool run_benchmark_experiment(const std::string &name,
                              const std::string &results_dir);

// Runs every experiment, in the order: scalability, density, distance,
// heuristic.
void run_all_benchmark_experiments(const std::string &results_dir);

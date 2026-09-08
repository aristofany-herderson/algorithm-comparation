#pragma once

#include <string>

bool run_benchmark_experiment(const std::string &name,
                              const std::string &results_dir);

void run_all_benchmark_experiments(const std::string &results_dir);

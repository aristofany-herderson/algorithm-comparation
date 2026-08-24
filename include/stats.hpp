#pragma once

#include <vector>

// Basic descriptive statistics computed over a set of repeated
// measurements (e.g. execution times in one benchmark run).
struct Stats {
  double mean = 0.0;
  double median = 0.0;
  double min = 0.0;
  double max = 0.0;
  double stddev = 0.0; // sample standard deviation (n-1 denominator)
};

// Computes mean/median/min/max/stddev over `values`. Returns a zeroed
// Stats if `values` is empty.
Stats compute_stats(std::vector<double> values);

#pragma once

#include <vector>

struct Stats {
  double mean = 0.0;
  double median = 0.0;
  double min = 0.0;
  double max = 0.0;
  double stddev = 0.0;
};

Stats compute_stats(std::vector<double> values);

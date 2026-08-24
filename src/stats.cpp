#include "stats.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

Stats compute_stats(std::vector<double> values) {
  Stats s;
  if (values.empty())
    return s;

  std::sort(values.begin(), values.end());

  double sum = std::accumulate(values.begin(), values.end(), 0.0);
  s.mean = sum / static_cast<double>(values.size());
  s.min = values.front();
  s.max = values.back();

  std::size_t n = values.size();
  if (n % 2 == 0) {
    s.median = (values[n / 2 - 1] + values[n / 2]) / 2.0;
  } else {
    s.median = values[n / 2];
  }

  if (n > 1) {
    double sq_sum = 0.0;
    for (double v : values) {
      double diff = v - s.mean;
      sq_sum += diff * diff;
    }
    s.stddev = std::sqrt(sq_sum / static_cast<double>(n - 1));
  }

  return s;
}

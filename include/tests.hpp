#pragma once

// Runs all correctness tests (manual graphs + randomized cross-checks
// between Dijkstra and A*). Prints a report to stdout and returns true iff
// every test passed.
bool run_all_tests();

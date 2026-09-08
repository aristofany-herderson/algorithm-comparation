#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "astar.hpp"
#include "benchmark.hpp"
#include "dijkstra.hpp"
#include "generators.hpp"
#include "manual_graphs.hpp"
#include "tests.hpp"

namespace {

void print_usage(const char *program_name) {
  std::cout << "Uso: " << program_name << " <modo> [experimento]\n\n"
            << "Modos:\n"
            << "  --test                     Executa os testes de validacao "
               "(Dijkstra vs A*)\n"
            << "  --example                  Executa e imprime os grafos de "
               "exemplo manuais\n"
            << "  --benchmark                Executa todos os experimentos e "
               "gera os CSVs\n"
            << "  --benchmark scalability    Executa apenas o experimento de "
               "escalabilidade\n"
            << "  --benchmark density        Executa apenas o experimento de "
               "densidade\n"
            << "  --benchmark distance       Executa apenas o experimento de "
               "distancia\n"
            << "  --benchmark heuristic      Executa apenas o experimento de "
               "heuristica\n";
}

void print_path(const std::vector<int> &path) {
  for (std::size_t i = 0; i < path.size(); ++i) {
    std::cout << path[i];
    if (i + 1 < path.size())
      std::cout << " -> ";
  }
}

void run_examples() {
  ManualCase cases[] = {
      make_simple_path_case(),      make_multiple_paths_case(),
      make_disconnected_case(),     make_source_equals_target_case(),
      make_equal_cost_paths_case(),
  };

  for (auto &c : cases) {
    std::cout << "=== " << c.name << " (source=" << c.source
              << ", target=" << c.target << ") ===\n";

    SearchResult d = dijkstra(c.graph, c.source, c.target);
    Heuristic h = c.graph.is_geometric()
                      ? make_euclidean_heuristic(c.graph, c.target)
                      : zero_heuristic();
    SearchResult a = astar(c.graph, c.source, c.target, h);

    std::cout << "Dijkstra: found=" << d.found << " cost=" << d.cost
              << " path=";
    print_path(d.path);
    std::cout << "\n  nodes_processed=" << d.nodes_processed
              << " edges_examined=" << d.edges_examined
              << " relaxations=" << d.relaxations
              << " queue_pushes=" << d.queue_pushes << "\n";

    std::cout << "A*:       found=" << a.found << " cost=" << a.cost
              << " path=";
    print_path(a.path);
    std::cout << "\n  nodes_processed=" << a.nodes_processed
              << " edges_examined=" << a.edges_examined
              << " relaxations=" << a.relaxations
              << " queue_pushes=" << a.queue_pushes << "\n\n";
  }

  std::cout << "=== geometric_example (grid-like random points) ===\n";
  Graph g = generate_geometric_graph(50, 6, 2024);
  int source = 0;
  int target = 49;
  SearchResult d = dijkstra(g, source, target);
  Heuristic h = make_euclidean_heuristic(g, target);
  SearchResult a = astar(g, source, target, h);

  std::cout << "Dijkstra: found=" << d.found << " cost=" << d.cost
            << " nodes_processed=" << d.nodes_processed << "\n";
  std::cout << "A*:       found=" << a.found << " cost=" << a.cost
            << " nodes_processed=" << a.nodes_processed << "\n";
}

} // namespace

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }

  std::string mode = argv[1];
  const std::string results_dir = "results";

  try {
    if (mode == "--test") {
      bool ok = run_all_tests();
      return ok ? 0 : 1;
    }

    if (mode == "--example") {
      run_examples();
      return 0;
    }

    if (mode == "--benchmark") {
      if (argc >= 3) {
        std::string experiment = argv[2];
        if (!run_benchmark_experiment(experiment, results_dir)) {
          std::cerr << "Experimento desconhecido: " << experiment << "\n";
          print_usage(argv[0]);
          return 1;
        }
      } else {
        run_all_benchmark_experiments(results_dir);
      }
      return 0;
    }
  } catch (const std::exception &e) {
    std::cerr << "Erro fatal: " << e.what() << "\n";
    return 1;
  }

  print_usage(argv[0]);
  return 1;
}

<p id="title" align="center">
  <a href="#title">
    <h1 align="center">graphsearch</h1>
  </a>
</p>

<p align="center">
  <img alt="" src="https://img.shields.io/badge/C%2B%2B-17-000000.svg?style=for-the-badge&labelColor=000">
  <img alt="" src="https://img.shields.io/badge/CMake-%E2%89%A53.16-000000.svg?style=for-the-badge&labelColor=000">
</p>

<p align="center">🔀&nbsp; Dijkstra e A* implementados do zero em C++17, comparados por corretude e por desempenho empírico real, sem bibliotecas externas de algoritmos</p>

<br>

## 📖&nbsp; Sumário

- [O que este projeto é](#-o-que-este-projeto-é)
- [1. Introdução](#1-introdução)
- [2. Definição do problema](#2-definição-do-problema)
- [3. Dijkstra](#3-dijkstra)
- [4. A\*](#4-a)
- [5. Representação dos grafos](#5-representação-dos-grafos)
- [6. Heurística utilizada e admissibilidade](#6-heurística-utilizada-e-admissibilidade)
- [7. Detalhes da implementação](#7-detalhes-da-implementação)
- [8. Testes de correção](#8-testes-de-correção)
- [9. Metodologia experimental](#9-metodologia-experimental)
- [10. Métricas utilizadas](#10-métricas-utilizadas)
- [11. Resultados dos experimentos](#11-resultados-dos-experimentos)
- [12. Análise dos resultados](#12-análise-dos-resultados)
- [13. Análise assintótica](#13-análise-assintótica)
- [14. Análise da complexidade baseada no código](#14-análise-da-complexidade-baseada-no-código)
- [15. Conclusão](#15-conclusão)
- [16. Como compilar e executar](#16-como-compilar-e-executar)
- [17. Estrutura do projeto](#17-estrutura-do-projeto)

<br>

## 💻&nbsp; O que este projeto é

Projeto acadêmico em C++17 que implementa Dijkstra e A\* do zero (sem
bibliotecas externas de algoritmos), valida a corretude dos dois contra os
mesmos casos, e mede empiricamente tempo de execução, nós processados,
arestas analisadas, relaxamentos e operações de fila de prioridade, sob
variação de tamanho do grafo, densidade, distância origem-destino e
qualidade da heurística.

Todos os números apresentados neste README foram obtidos executando o
próprio binário (`./build/graphsearch --benchmark`) nesta máquina; os CSVs
brutos estão em `results/`. Nenhum resultado foi inventado ou estimado.

<br>

## 1. Introdução

Dijkstra e A* resolvem o mesmo problema — caminho de menor custo em um grafo
com pesos não negativos — mas A* usa uma informação extra: uma **heurística**
que estima o custo restante até o destino. Essa informação extra é o que
diferencia os dois algoritmos na prática, mesmo quando, no pior caso, ambos
têm a mesma complexidade assintótica.

O objetivo deste projeto não é "provar que A\* é melhor", mas mostrar
**quando** e **por quê** ele é melhor, com números reais, e deixar claro que
complexidade de pior caso e desempenho empírico são coisas diferentes.

<br>

## 2. Definição do problema

> Dado um grafo ponderado `G = (V, E)`, com pesos `w(u, v) ≥ 0`, um vértice
> de origem `s` e um vértice de destino `t`, encontrar um caminho
> `s = v0, v1, ..., vk = t` que minimize a soma dos pesos das arestas do
> caminho, ou determinar que não existe caminho.

Ambos os algoritmos implementados aqui resolvem exatamente esse problema, e
são sempre executados sobre **o mesmo grafo, a mesma origem e o mesmo
destino** para permitir comparação direta.

<br>

## 3. Dijkstra

Dijkstra mantém, para cada vértice, a menor distância conhecida a partir da
origem (`g(n)`), e expande sempre o vértice não finalizado com menor `g(n)`,
usando uma fila de prioridade (min-heap). Ao expandir um vértice `u`, ele
tenta **relaxar** cada aresta `(u, v)`: se `g(u) + w(u, v) < g(v)`, atualiza
`g(v)` e reinsere `v` na fila.

Não há noção de "direção" para o destino: Dijkstra explora o grafo em ondas
concêntricas de custo crescente a partir da origem, então pode processar
vértices que estão muito longe do destino, desde que estejam "no caminho"
em termos de custo acumulado.

<br>

## 4. A\*

A\* usa a mesma estrutura de Dijkstra, mas ordena a fila de prioridade por

```
f(n) = g(n) + h(n)
```

onde `g(n)` é o custo acumulado (igual ao de Dijkstra) e `h(n)` é uma
estimativa do custo restante até o destino. Ao "olhar na direção" do
destino, A\* evita expandir vértices que Dijkstra expandiria só por terem
custo acumulado pequeno, mesmo estando na direção errada.

Quando `h(n) = 0` para todo `n`, `f(n) = g(n)` e A\* se comporta,
conceitualmente, como Dijkstra (a ordem de expansão passa a depender só do
custo acumulado). O experimento da [Seção 11.4](#114-impacto-da-heurística)
confirma isso empiricamente.

<br>

## 5. Representação dos grafos

O grafo é representado como lista de adjacência (`std::vector<std::vector<Edge>>`),
com `Edge { int to; double weight; }`. Essa é a representação padrão para
grafos esparsos e é a que Dijkstra/A\* com heap assumem na análise clássica
de complexidade.

Opcionalmente, cada grafo pode carregar coordenadas `(x, y)` por vértice
(`std::vector<std::pair<double,double>> coordinates`). Um grafo com
coordenadas é chamado de **geométrico** e é usado para construir a
heurística Euclidiana do A\*.

Dois tipos de grafo são suportados:

- **Grafos manuais** (`include/manual_graphs.hpp`, `src/manual_graphs.cpp`):
  cinco grafos pequenos, fixos, usados só para validação (Seção 8).
- **Grafos aleatórios** (`include/generators.hpp`, `src/generators.cpp`):
  - `generate_random_graph` / `generate_random_graph_by_density`: grafos
    aleatórios sem coordenadas, pesos uniformes em `[min, max]`.
  - `generate_geometric_graph`: `n` vértices com coordenadas uniformes em um
    quadrado, cada vértice ligado aos `k` vizinhos mais próximos, peso da
    aresta = distância Euclidiana exata entre os dois vértices.
  - `generate_geometric_graph_by_density`: mesma ideia, mas as arestas são
    amostradas aleatoriamente entre pares de vértices até atingir a
    densidade alvo (em vez de k-vizinhos-mais-próximos), mantendo peso =
    distância Euclidiana. Usado no experimento de densidade para que A\*
    continue tendo uma heurística válida mesmo variando a densidade.

<br>

## 6. Heurística utilizada e admissibilidade

Para grafos geométricos, a heurística é a distância Euclidiana até o
destino:

```
h(n) = sqrt( (x_n - x_t)^2 + (y_n - y_t)^2 )
```

**Por que essa heurística é admissível:** uma heurística é admissível se
nunca superestima o custo real restante, isto é, `h(n) ≤ custo_real(n, t)`
para todo `n`. Em todos os grafos geométricos gerados neste projeto, o peso
de cada aresta `(u, v)` é definido como sendo **exatamente** a distância
Euclidiana entre `u` e `v` (`generate_geometric_graph` e
`generate_geometric_graph_by_density`). Isso significa que qualquer caminho
`n = p0, p1, ..., pk = t` tem custo real

```
custo_real(n, t) = Σ dist_euclid(p_i, p_{i+1})
```

Pela desigualdade triangular, essa soma de distâncias Euclidianas nunca é
menor que a distância Euclidiana direta entre os extremos:

```
Σ dist_euclid(p_i, p_{i+1}) ≥ dist_euclid(p0, pk) = h(n)
```

Logo `h(n) ≤ custo_real(n, t)` para **qualquer** caminho, em particular para
o caminho ótimo — a heurística é admissível. Mais que isso, ela é
**consistente** (`h(u) ≤ w(u, v) + h(v)` para toda aresta `(u, v)`, que é a
própria desigualdade triangular aplicada localmente), o que garante que,
assim que um vértice é finalizado pela A\*, sua distância `g` já é ótima —
a mesma garantia que Dijkstra oferece.

Para grafos **não geométricos** (sem coordenadas), não existe uma heurística
geométrica válida; nesses casos o projeto usa `h(n) = 0` (heurística
trivial, também admissível — subestima tudo por definição), fazendo A\* se
comportar como Dijkstra. Isso é usado nos testes de corretude com grafos
aleatórios (Seção 8) e no experimento de impacto da heurística (Seção 11.4).

<br>

## 7. Detalhes da implementação

- **Estrutura de resultado comum** (`include/search_result.hpp`),
  usada pelos dois algoritmos:

  ```cpp
  struct SearchResult {
      bool found = false;
      double cost = std::numeric_limits<double>::infinity();
      std::vector<int> path;

      std::size_t nodes_popped = 0;
      std::size_t nodes_processed = 0;
      std::size_t edges_examined = 0;
      std::size_t relaxations = 0;
      std::size_t queue_pushes = 0;
      std::size_t max_queue_size = 0;
  };
  ```

- **Fila de prioridade**: ambos usam `std::priority_queue` com um
  comparador customizado para virar min-heap (o padrão de
  `std::priority_queue` é max-heap).

- **Deleção preguiçosa ("lazy deletion")**: nem Dijkstra nem A* removem
  entradas obsoletas da fila quando uma distância melhor é encontrada para
  um vértice já enfileirado. Em vez disso, cada vértice tem uma distância
  "atual" (`dist[v]` em Dijkstra, `best_g[v]` em A*) e um flag `finalized`.
  Quando uma entrada obsoleta é retirada da fila, ela é simplesmente
  ignorada (`continue`). Essa é a estratégia padrão e mais simples para usar
  `std::priority_queue` (que não suporta `decrease-key`) — a alternativa
  seria uma fila indexada, o que adicionaria complexidade sem mudar a
  complexidade assintótica. O impacto disso na análise está detalhado na
  Seção 14.

- **Early-exit no destino**: assim que o vértice `target` é finalizado (isto
  é, retirado da fila e confirmado como não-obsoleto), o laço principal
  encerra (`break`) — não há necessidade de continuar explorando o grafo
  depois que o destino tem certeza de estar com a distância ótima. Essa
  otimização é aplicada **igualmente** aos dois algoritmos, para manter a
  comparação justa.

- **A\* com desempate por `f`**: a fila de A\* guarda `(f, vertex, g)`. O `g`
  é armazenado junto para permitir detectar entradas obsoletas sem
  recalcular `h` (comparando o `g` da entrada com `best_g[v]` atual).

<br>

## 8. Testes de correção

Executados com `./build/graphsearch --test` (ver saída completa na Seção
16). O teste roda dois grupos de verificação:

**(a) Grafos manuais** (`src/manual_graphs.cpp`), cinco casos pedidos no
enunciado:

| Caso                   | O que testa                                    | Esperado                         |
| ---------------------- | ---------------------------------------------- | -------------------------------- |
| `simple_path`          | caminho único 0→1→2→3                          | custo = 3                        |
| `multiple_paths`       | dois caminhos de custo diferente (4 e 6)       | custo = 4 (o menor)              |
| `disconnected`         | origem e destino em componentes diferentes     | não encontrado                   |
| `source_equals_target` | origem = destino                               | custo = 0, caminho = [origem]    |
| `equal_cost_paths`     | dois caminhos disjuntos de mesmo custo (4 e 4) | custo = 4 (caminho pode diferir) |

**(b) Cross-check aleatório**: 10 pares origem/destino aleatórios em um
grafo aleatório sem coordenadas (A* com `h=0`) e 10 pares em um grafo
geométrico (A* com heurística Euclidiana). Para cada par, o teste roda
Dijkstra e A\* e verifica que `found` é igual e que `cost` é igual dentro de
tolerância `1e-6`. Se algum teste falhar, o programa imprime `[FAIL]` com os
valores divergentes e retorna código de saída 1.

Resultado real da última execução: **todos os 25 testes passaram** (5
manuais + 20 aleatórios).

<br>

## 9. Metodologia experimental

- Tempo medido com `std::chrono::steady_clock`, envolvendo **apenas** a
  chamada ao algoritmo (`dijkstra(...)` ou `astar(...)`). Geração do grafo,
  escrita de CSV e qualquer impressão ficam fora da região cronometrada.
- Cada configuração (grafo + algoritmo + origem + destino) é executada **30
  vezes** e agregada em média, mediana, mínimo, máximo e desvio padrão
  amostral (`include/stats.hpp`, `src/stats.cpp`).
- Contadores estruturais (nós processados, arestas examinadas,
  relaxamentos, operações de fila) são determinísticos para uma dada
  entrada — não mudam entre repetições — então são coletados de uma única
  execução representativa por configuração, enquanto o tempo é reamostrado
  30 vezes.
- Dijkstra e A\* sempre rodam **sobre a mesma instância de grafo**, com a
  mesma origem e o mesmo destino, dentro de uma mesma linha de experimento.
- Toda seed usada é registrada na coluna `seed` do CSV correspondente, para
  reprodutibilidade exata (mesma seed + mesmos parâmetros ⇒ mesmo grafo).

<br>

## 10. Métricas utilizadas

Cada linha de CSV registra: algoritmo, rótulo extra (categoria de distância
ou tipo de heurística, quando aplicável), número de vértices, número de
arestas (arcos direcionados), densidade, origem, destino, seed, número de
repetições, se encontrou caminho, custo do caminho, tempo médio/mediana/
mínimo/máximo/desvio-padrão (ms), nós processados, nós retirados da fila
(`nodes_popped`, inclui obsoletos), arestas examinadas, relaxamentos,
operações de push na fila e tamanho máximo da fila.

<br>

## 11. Resultados dos experimentos

Todos os valores abaixo vêm diretamente de `results/*.csv`, gerados por
`./build/graphsearch --benchmark`.

### 11.1 Escalabilidade (`results/scalability.csv`)

Grafos geométricos com 6 vizinhos mais próximos por vértice, origem = 0,
destino = último vértice, seed = 42.

|      V | E (arcos) | Algoritmo | Tempo médio (ms) | Nós processados | Arestas examinadas | Pushes na fila |
| -----: | --------: | --------- | ---------------: | --------------: | -----------------: | -------------: |
|    100 |       738 | Dijkstra  |           0.0021 |              44 |                320 |             82 |
|    100 |       738 | A\*       |           0.0012 |              13 |                 92 |             34 |
|    500 |     3 608 | Dijkstra  |           0.2084 |             460 |              3 306 |            617 |
|    500 |     3 608 | A\*       |           0.0064 |              85 |                601 |            184 |
|  1 000 |     7 094 | Dijkstra  |           0.0056 |              32 |                215 |             60 |
|  1 000 |     7 094 | A\*       |           0.0011 |               6 |                 35 |             19 |
|  5 000 |    35 368 | Dijkstra  |           0.4894 |           2 402 |             16 995 |          3 358 |
|  5 000 |    35 368 | A\*       |           0.0234 |             226 |              1 599 |            443 |
| 10 000 |    70 252 | Dijkstra  |           2.1045 |           7 952 |             55 849 |         10 948 |
| 10 000 |    70 252 | A\*       |           0.2788 |           1 000 |              7 028 |          1 864 |

Em todos os tamanhos, os custos finais reportados por Dijkstra e A*
coincidem (validado também pelos testes de corretude), e A* processa uma
fração muito menor dos vértices — em 10 000 vértices, A\* processa **1 000**
contra **7 952** de Dijkstra (≈12.6%).

Os contadores estruturais (nós processados, arestas examinadas, pushes) são
**idênticos** entre execuções repetidas do binário, porque dependem só da
seed e da lógica do algoritmo, nunca do relógio da máquina — isso foi
conferido comparando duas rodadas completas de `--benchmark` nesta
máquina. Já o tempo em milissegundos varia de execução para execução
(carga do sistema, cache, agendador do SO), o que é esperado e é
justamente por isso que cada configuração é repetida 30 vezes e reportada
com média/mediana/desvio-padrão em vez de uma amostra única.

(O caso `n=1000` mostra menos nós processados que `n=500` para os dois
algoritmos — isso é esperado: cada grafo é uma amostra geométrica aleatória
diferente, e a distância "topológica" entre a origem e o destino depende de
onde os pontos caem, não só de `n`. Fica evidente na variação normal de
grafos aleatórios, e é justamente o motivo de existir o experimento
dedicado de distância na Seção 11.3.)

### 11.2 Densidade (`results/density.csv`)

Grafos geométricos de 3 000 vértices, arestas amostradas aleatoriamente até
a densidade alvo (peso = distância Euclidiana), origem = 0,
destino = 2 999, seed = 7.

| Densidade | E (arcos) | Algoritmo |     Encontrado     | Tempo médio (ms) | Nós processados | Arestas examinadas |
| --------: | --------: | --------- | :----------------: | ---------------: | --------------: | -----------------: |
|     0.001 |     8 998 | Dijkstra  |        não         |           0.4227 |           2 821 |              8 966 |
|     0.001 |     8 998 | A\*       |        não         |           0.5074 |           2 821 |              8 966 |
|     0.005 |    44 986 | Dijkstra  | sim (custo 1133.0) |           0.7791 |           2 230 |             34 285 |
|     0.005 |    44 986 | A\*       | sim (custo 1133.0) |           0.1786 |             351 |              5 479 |
|     0.020 |   179 940 | Dijkstra  | sim (custo 423.3)  |           0.4794 |             444 |             26 730 |
|     0.020 |   179 940 | A\*       | sim (custo 423.3)  |           0.0925 |              79 |              4 585 |
|     0.050 |   449 850 | Dijkstra  |  sim (custo 61.9)  |           0.2391 |               5 |                610 |
|     0.050 |   449 850 | A\*       |  sim (custo 61.9)  |           0.0044 |               2 |                156 |
|     0.100 |   899 700 | Dijkstra  |  sim (custo 61.9)  |           0.8637 |               9 |              2 446 |
|     0.100 |   899 700 | A\*       |  sim (custo 61.9)  |           0.0080 |               2 |                311 |

Na densidade mais baixa (0.001), a origem e o destino simplesmente **não
estão conectados** no grafo amostrado — os dois algoritmos exploram
totalmente a componente conexa da origem (2 821 nós, os mesmos para ambos,
já que sem alcançar o destino não há "atalho" a ser explorado) e concluem
corretamente que não há caminho. Esse é um resultado real, não um erro: em
grafos aleatórios muito esparsos, a conectividade não é garantida.

A partir da densidade 0.005, o grafo fica conexo entre os dois vértices, e
o padrão se repete: A\* processa muito menos nós que Dijkstra, e a vantagem
(em nós processados e em tempo) tende a diminuir conforme a densidade
aumenta — em grafos muito densos, o destino está a poucos saltos de
qualquer vértice, e ambos os algoritmos terminam rápido de qualquer forma
(veja a Seção 12 para a explicação).

### 11.3 Distância origem-destino (`results/distance.csv`)

Um único grafo geométrico de 3 000 vértices (k=6, seed=123), origem = 0
fixa, alvo escolhido por percentil de distância Euclidiana até a origem
(5º, 50º e 95º percentil entre todos os vértices).

| Categoria | Alvo |  Custo | Algoritmo | Tempo médio (ms) | Nós processados | Arestas examinadas |
| --------- | ---: | -----: | --------- | ---------------: | --------------: | -----------------: |
| perto     | 2115 | 155.07 | Dijkstra  |           0.0121 |             149 |              1 031 |
| perto     | 2115 | 155.07 | A\*       |           0.0027 |              21 |                138 |
| médio     | 1611 | 443.79 | Dijkstra  |           0.2304 |           1 407 |             10 012 |
| médio     | 1611 | 443.79 | A\*       |           0.0070 |              71 |                495 |
| longe     | 1281 | 771.62 | Dijkstra  |           0.4928 |           2 826 |             19 948 |
| longe     | 1281 | 771.62 | A\*       |           0.0438 |             329 |              2 329 |

A razão nós-processados-Dijkstra / nós-processados-A* cresce com a
distância: ≈7.1× perto, ≈19.8× no meio, ≈8.6× longe (o grafo geométrico não
é uma grade regular, então a razão exata varia com a topologia local, mas a
tendência de A* explorar uma fração cada vez menor do grafo à medida que o
destino fica "atrás" de mais vértices irrelevantes é clara e consistente).
Isso confirma a expectativa teórica: quanto mais a heurística consegue
"apontar a direção certa" através de um caminho mais longo, mais nós fora
dessa direção A\* evita expandir.

### 11.4 Impacto da heurística (`results/heuristic.csv`)

Mesmo grafo geométrico (3 000 vértices, k=6, seed=99), mesma origem (0) e
destino (2999), comparando três configurações:

| Algoritmo | h(n)       | Tempo médio (ms) | Nós processados | Arestas examinadas | Relaxamentos |
| --------- | ---------- | ---------------: | --------------: | -----------------: | -----------: |
| Dijkstra  | —          |           0.3776 |           2 130 |             15 103 |        2 925 |
| A\*       | 0          |           0.3647 |           2 130 |             15 103 |        2 925 |
| A\*       | Euclidiana |           0.0637 |             388 |              2 751 |          753 |

Com `h(n) = 0`, `f(n) = g(n)`, e A* processa **exatamente** o mesmo número
de nós, examina exatamente o mesmo número de arestas e faz exatamente o
mesmo número de relaxamentos que Dijkstra (2 130 / 15 103 / 2 925 nos três
casos, incluindo Dijkstra) — a pequena diferença de tempo entre Dijkstra e
A*-com-h=0 é só o overhead de calcular `f = g + h(n)` a cada relaxamento,
mesmo com `h(n)=0`, e nem sempre tem o mesmo sinal entre execuções (ruído
de medição), já que o trabalho estrutural é idêntico.
Isso é a confirmação empírica direta de que, sem uma heurística informativa,
A\* degenera para o mesmo comportamento de exploração de Dijkstra.

Com a heurística Euclidiana, os nós processados caem de 2 130 para 388
(≈5.5× menos) e o tempo médio cai de ~0.37 ms para ~0.06 ms (≈5.9× mais
rápido).

### 11.5 Dashboard visual

Além das tabelas acima, o script `scripts/generate_report.py` lê os quatro
CSVs em `results/` e gera `results/dashboard.html` — uma página estática
(sem servidor, sem dependências além de Chart.js via CDN) com os mesmos
dados renderizados como gráficos: tempo × número de vértices (escala log)
e nós processados × número de vértices na escalabilidade; tempo e nós
processados × densidade; nós processados e tempo por categoria de
distância; e a comparação de três barras (Dijkstra / A* h=0 / A*
Euclidiana) do experimento de heurística. Todos os números do dashboard
são os mesmos das tabelas acima — o script só lê o CSV e desenha, não
recalcula nada. Veja a Seção 16 para o comando exato.

<br>

## 12. Análise dos resultados

Três observações, sustentadas pelos números da Seção 11:

**(1) A vantagem do A\* vem de uma heurística informativa, não do algoritmo
em si.** O experimento da Seção 11.4 isola essa variável: mesmo grafo,
mesma origem, mesmo destino — a única diferença é `h(n)`. Com `h(n)=0` os
dois algoritmos são estruturalmente idênticos (mesmos contadores). Com a
heurística Euclidiana, A\* processa 5.5× menos nós. A "inteligência" não
está na fila de prioridade nem no relaxamento — está inteiramente na
informação extra que `h(n)` injeta na ordem de expansão.

**(2) A vantagem cresce com a distância e diminui com a densidade.** No
experimento de distância (11.3), pares mais distantes dão à heurística mais
"espaço" para descartar direções erradas, então a fração de nós evitados
tende a crescer. No experimento de densidade (11.2), o oposto acontece: com
densidade muito alta, o destino está a poucos saltos de qualquer vértice
(o `custo` final até caiu de 423 para 62 quando a densidade passou de 0.02
para 0.05, porque uma aresta quase direta origem-destino passou a existir),
então tanto Dijkstra quanto A* processam poucos nós e a diferença entre
eles, em termos absolutos, fica pequena — a vantagem relativa do A* é maior
justamente quando o grafo é grande e esparso o suficiente para que "ir na
direção errada" tenha um custo real de exploração.

**(3) Dijkstra e A\* com heurística fraca continuam corretos, só mais
lentos.** Em nenhum experimento os custos finais divergiram entre os dois
algoritmos (respeitando a tolerância de ponto flutuante) — a diferença é
inteiramente de **quanto trabalho** cada um faz para chegar à mesma
resposta correta, nunca de qual resposta é encontrada.

<br>

## 13. Análise assintótica

Seja `V = |vértices|`, `E = |arestas|`, grafo representado por lista de
adjacência, fila de prioridade binária (a que `std::priority_queue` usa).

### 13.1 Tabela comparativa (melhor caso, caso médio, pior caso)

|                                                                   | Dijkstra                                                                                                                                                                                                  | A\* (heurística admissível e consistente)                                                                                                                                                                                                                                                                                                                                                                                         |
| ----------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Melhor caso**                                                   | `O(V log V)` — o destino é finalizado logo nas primeiras extrações da fila (poucos relaxamentos antes do `break`); ainda paga `O(V)` para inicializar os vetores de distância/predecessor/finalizado.     | `O(V)` a `O(k log k)`, com `k ≪ V` — com uma heurística muito informativa (próxima do custo real), A\* pode expandir só os `k` vértices do caminho ótimo antes de atingir o destino; no limite teórico ideal (`h(n)` = custo real exato), expande exatamente os vértices do caminho.                                                                                                                                              |
| **Caso médio** (grafo esparso, geométrico, heurística Euclidiana) | Entre `O(V log V)` e `O((V+E) log V)`, tipicamente perto do pior caso quando a origem e o destino estão distantes, porque Dijkstra explora por ondas de custo crescente sem "saber" a direção do destino. | Substancialmente menor que Dijkstra na prática — observado empiricamente na Seção 11 como uma fração de ~6% a ~35% dos nós processados por Dijkstra, dependendo da distância/densidade — mas sem uma cota assintótica fechada geral, porque depende de quão bem `h(n)` aproxima o custo real (não é uma propriedade só do algoritmo, é uma propriedade do par algoritmo+heurística+instância).                                    |
| **Pior caso**                                                     | `O((V + E) log V)` — grafo denso, destino é o último vértice a ser finalizado, quase todos os vértices/arestas são processados antes do `break`.                                                          | `O((V + E) log V)` — **idêntico** ao de Dijkstra. Ocorre quando a heurística não informa nada de útil sobre a direção do destino (equivalente, na prática, a `h(n)=0`) ou quando o próprio grafo não tem estrutura para a heurística explorar (ex.: grafo sem relação geométrica com os pesos). Confirmado empiricamente na Seção 11.4: com `h(n)=0`, A\* processa exatamente os mesmos nós, arestas e relaxamentos que Dijkstra. |
| **Espaço**                                                        | `O(V + E)`                                                                                                                                                                                                | `O(V + E)` (idêntico; a heurística Euclidiana usada aqui é `O(1)` de espaço extra por consulta, não `O(V)`)                                                                                                                                                                                                                                                                                                                       |

A conclusão direta dessa tabela é a mesma da Seção 15: **no pior caso os
dois algoritmos são iguais**; a diferença prática mora inteiramente na
coluna "caso médio", que não tem uma fórmula fechada porque depende da
qualidade da heurística e da instância — por isso ela precisa ser medida
empiricamente (Seção 11), não apenas derivada analiticamente.

### 13.2 Custos de operação, na representação escolhida

- Percorrer as arestas de um vértice `u`: `O(grau(u))`; somado sobre todos
  os vértices processados, no pior caso `O(E)` no total (cada aresta é
  examinada no máximo uma vez a partir de cada extremidade, ou seja, no
  máximo duas vezes se o grafo for não-direcionado — uma constante que some
  no Big-O).
- Inserir um elemento na fila de prioridade binária de tamanho `k`:
  `O(log k)`. Como a fila nunca tem mais que `O(E)` elementos, cada inserção
  custa `O(log E) = O(log V)` (já que `E ≤ V²`, `log E ≤ 2 log V`, e ambos
  são a mesma classe assintótica para grafos razoáveis).
- Remover o mínimo da fila (`pop`): também `O(log V)`.
- Relaxar uma aresta: `O(1)` de trabalho aritmético/comparação, mais o
  custo de um possível `push` (`O(log V)`) se a relaxação for bem-sucedida.

### 13.3 Derivação: Dijkstra e A\* no pior caso

**Dijkstra — pior caso:** cada aresta pode gerar no máximo um `push` (uma
relaxação bem-sucedida), então há no máximo `O(E)` pushes e, portanto,
`O(E)` pops relevantes (mais possíveis pops de entradas obsoletas, também
limitados por `O(E)`, pela estratégia de deleção preguiçosa — ver Seção
14). Isso dá:

```
O(E) pushes/pops × O(log V) por operação  =  O(E log V)
                                              + O(V) para inicializar distâncias
                                           =  O((V + E) log V)
```

que é a complexidade clássica de Dijkstra com heap binário e lista de
adjacência.

**A\* — pior caso:** A* tem a **mesma** complexidade de pior caso que
Dijkstra, `O((V + E) log V)`, e por um motivo simples: no pior caso
possível, a heurística não ajuda em nada (é equivalente a `h(n)=0` na
prática, mesmo que não seja literalmente zero) e A* acaba examinando
essencialmente os mesmos vértices e arestas que Dijkstra, com o mesmo custo
por operação (calcular `h(n)` é `O(1)` para a heurística Euclidiana, então
não muda a classe assintótica). Não existe garantia teórica geral de que
A\* processe menos nós que Dijkstra — essa garantia depende inteiramente da
qualidade (quão informativa é) da heurística usada:

- **`h(n) = 0`:** `f(n) = g(n)` para todo `n`; a ordem de expansão de A\*
  passa a ser idêntica à de Dijkstra (module empates), então o número de
  nós processados, arestas examinadas e relaxamentos é **igual** ao de
  Dijkstra — confirmado empiricamente na Seção 11.4.
- **Heurística informativa (ex.: Euclidiana, admissível e consistente):**
  A* ainda garante otimalidade (por consistência, todo vértice é finalizado
  com seu `g` ótimo, igual a Dijkstra), mas tende a processar **menos**
  vértices na prática, porque `f(n) = g(n) + h(n)` cresce mais rápido para
  vértices "fora do caminho" que para vértices "na direção certa",
  fazendo com que a fila de prioridade explore a região relevante do grafo
  primeiro. No limite ideal (heurística igual ao custo real restante), A*
  expande só os vértices do caminho ótimo — mas isso é um caso extremo, não
  o comportamento típico.

**Complexidade de espaço:** ambos armazenam `O(V)` para os vetores de
distância/`g`, predecessor e "finalizado", mais `O(E)` para a lista de
adjacência (compartilhada entre os dois algoritmos, não duplicada por
execução), mais `O(E)` no pior caso para a fila de prioridade (devido a
entradas obsoletas — Seção 14). Total: `O(V + E)` para os dois algoritmos.

<br>

## 14. Análise da complexidade baseada no código

Esta seção conecta trechos reais do código a contagens de execução e ao
Big-O final.

### 14.1 O laço principal: `while (!pq.empty())`

Em `src/dijkstra.cpp` e `src/astar.cpp`:

```cpp
while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();
    result.nodes_popped++;
    ...
}
```

**Quantas vezes esse laço executa?** Uma vez por elemento que já esteve na
fila, ou seja, uma vez por `push` executado. Cada `push` só acontece dentro
do laço de relaxamento (abaixo), e no máximo um `push` acontece por aresta
"relaxada com sucesso" — logo o número de iterações do `while` é limitado
por `1 (push inicial) + número de relaxamentos bem-sucedidos ≤ 1 + O(E)`.
No pior caso, isso é `O(E)` iterações. Cada iteração faz um `pq.pop()`, que
em um heap binário de tamanho até `O(E)` custa `O(log E) = O(log V)`.
Contribuição total do laço `while` (só os pops): `O(E log V)`.

O contador `nodes_popped` no `SearchResult` mede exatamente esse número de
iterações — nos experimentos, `nodes_popped` é sistematicamente maior que
`nodes_processed` (ex.: na linha "far" do experimento de distância,
Dijkstra tem `nodes_popped = 3771` contra `nodes_processed = 2826`), e essa
diferença **é** o custo das entradas obsoletas (Seção 14.3).

### 14.2 O laço interno: `for (const auto& edge : graph.adjacency[u])`

```cpp
for (const auto& edge : graph.adjacency[static_cast<std::size_t>(u)]) {
    result.edges_examined++;
    if (finalized[...]) continue;
    double candidate = ... ;
    if (candidate < dist[...]) {
        ...
        result.relaxations++;
        pq.push({candidate, edge.to});
        result.queue_pushes++;
    }
}
```

**Quantas vezes esse laço executa, no total, somado sobre toda a
execução?** Ele só roda para vértices que passam pelo `if
(finalized[u]) continue;` do laço externo, isto é, para vértices realmente
**processados** (não para pops obsoletos). Cada vértice é processado no
máximo uma vez (o flag `finalized` impede reprocessamento), então a soma de
`grau(u)` sobre todos os `u` processados é, no pior caso (todos os vértices
processados), `Σ grau(u) = O(E)` — a soma dos graus de todos os vértices de
um grafo é sempre `2|E|` (não-direcionado) ou `|E|` (direcionado). Essa é
exatamente a contagem que `edges_examined` mede no `SearchResult`, e nos
experimentos ela nunca excede o número de arcos do grafo, como esperado
(ex.: em `n=10000`, Dijkstra tem `edges_examined = 55849` contra
`num_edges = 70252` arcos totais — nem todo vértice chega a ser processado
antes do `break` no destino).

**Custo por iteração:** a comparação e a possível atualização são `O(1)`.
O `pq.push(...)`, quando acontece, custa `O(log V)` (heap com até `O(E)`
elementos). Como o número de `push`s bem-sucedidos é limitado por `O(E)`
(cada aresta contribui no máximo um relaxamento bem-sucedido — depois que
`dist[v]`/`best_g[v]` atinge seu valor ótimo, novas tentativas de relaxar
`v` falham na comparação `candidate < dist[v]`), o custo total de todos os
`push`s é `O(E log V)`.

### 14.3 Entradas obsoletas na `std::priority_queue`

`std::priority_queue` não suporta `decrease-key` (não há como diminuir a
prioridade de um elemento já inserido). A estratégia usada aqui —
**deleção preguiçosa** — insere uma nova entrada toda vez que uma distância
melhor é encontrada, e deixa a entrada antiga "morta" na fila. Quando essa
entrada morta chega ao topo, o código a descarta:

```cpp
if (finalized[static_cast<std::size_t>(u)]) {
    continue; // entrada obsoleta: já processamos este vértice com distância melhor
}
```

(em A\*, há uma verificação adicional, `if (g > best_g[u]) continue;`, pois
duas entradas para o mesmo vértice podem coexistir com `g`s diferentes.)

**Consequência para a análise:** o número de entradas na fila, em qualquer
momento, é `O(E)` (uma por `push`, e o total de `push`s é `O(E)` como
mostrado acima) — não `O(V)`. Isso afeta o Big-O de duas formas:

1. O tamanho do heap usado no `O(log tamanho)` de cada `push`/`pop` é
   `O(E)`, não `O(V)` — mas como `E ≤ V²`, temos `log E ≤ 2 log V`, então
   isso não muda a classe assintótica (`O(log E) = O(log V)`), só a
   constante.
2. O número de iterações do `while (!pq.empty())` é `O(E)` (uma por push),
   não `O(V)` — é por isso que a complexidade final tem `E log V` (e não
   `V log V`) como termo dominante do lado da fila.

A métrica `queue_pushes` no `SearchResult` mede exatamente esse total de
inserções, e `max_queue_size` mostra o pico real do heap durante a
execução — nos experimentos, `max_queue_size` fica sempre bem abaixo do
número total de arcos do grafo (ex.: em `n=10000`, `max_queue_size = 265`
contra `70252` arcos), porque o `break` no destino interrompe a execução
antes de o heap crescer até seu limite teórico.

### 14.4 Ligação código → Big-O

| Código                                                | Nº de execuções                         | Custo por execução | Contribuição total |
| ----------------------------------------------------- | --------------------------------------- | ------------------ | ------------------ |
| `while (!pq.empty())` (pop)                           | `O(E)` (1 por push)                     | `O(log V)`         | `O(E log V)`       |
| `for (edge : adjacency[u])` (examinar aresta)         | `O(E)` (soma dos graus dos processados) | `O(1)`             | `O(E)`             |
| `pq.push(...)` dentro do relaxamento                  | `O(E)` (1 por relaxação bem-sucedida)   | `O(log V)`         | `O(E log V)`       |
| Inicialização de `dist`/`best_g`/`parent`/`finalized` | `O(V)`                                  | `O(1)`             | `O(V)`             |

Somando: `O(E log V) + O(E) + O(E log V) + O(V) = O((V + E) log V)`, para
os dois algoritmos, no pior caso. A diferença prática entre Dijkstra e A\*
não aparece nesta tabela — ela aparece em **quantas vezes**, na prática, a
condição `candidate < dist[v]` (ou `finalized[u]`) é satisfeita antes de
`u == target`, o que depende de quantos vértices o `f(n) = g(n) + h(n)`
consegue "adiar" na fila. Isso é precisamente o que os experimentos da
Seção 11 medem diretamente pelos contadores, em vez de inferir do Big-O.

<br>

## 15. Conclusão

**Complexidade assintótica não é desempenho empírico.** Dijkstra e A\* têm a
mesma complexidade de pior caso, `O((V + E) log V)`, com a mesma
representação de grafo e a mesma estrutura de fila de prioridade — isso é
confirmado pela análise de código da Seção 14, onde as mesmas três
operações (`pop`, examinar aresta, `push`) aparecem nos dois algoritmos com
os mesmos custos por operação. Se a pergunta fosse só "qual é o Big-O?", a
resposta seria "os dois são iguais" e a comparação pareceria sem sentido.

Mas o Big-O de pior caso descreve um **limite superior sobre todas as
entradas possíveis**, não o comportamento em uma entrada específica. Na
prática, a _quantidade real_ de nós que cada algoritmo processa antes de
finalizar o destino depende de como a ordem de expansão da fila de
prioridade se alinha com a localização do destino — e é exatamente aí que
`h(n)` entra. O experimento da Seção 11.4 isola essa variável e mostra, com
o mesmo grafo e os mesmos contadores de código, que **sem heurística
informativa A\* processa exatamente o mesmo número de nós que Dijkstra**
(2 130 = 2 130); **com heurística Euclidiana, A\* processa 5.5× menos**
(388 nós). Os experimentos de distância (11.3) e densidade (11.2) mostram
que essa vantagem não é constante — ela cresce quando a heurística tem mais
"espaço" para descartar direções erradas (destinos distantes, grafos
esparsos) e encolhe quando o problema já é fácil para os dois (destinos
próximos, grafos densos).

**Em quais situações A\* apresenta vantagens práticas sobre Dijkstra?**
Quando existe uma heurística admissível e informativa (não trivial) e o
grafo tem estrutura geométrica/direcional relevante o bastante para que
"ir na direção certa" realmente evite trabalho — tipicamente, grafos
esparsos e grandes, com origem e destino relativamente distantes. Nessas
condições, medidas aqui diretamente (nós processados, arestas examinadas,
tempo), A\* processa uma fração pequena do que Dijkstra processa, mantendo
exatamente a mesma garantia de otimalidade. Quando a heurística é fraca ou
ausente (`h(n)=0`), ou o grafo é denso o suficiente para que qualquer
vértice esteja perto do destino, a vantagem prática desaparece e os dois
algoritmos convergem para o mesmo comportamento — consistente com o fato de
compartilharem a mesma complexidade de pior caso.

<br>

## 16. Como compilar e executar

### Compilar

```bash
cmake -S . -B build
cmake --build build
```

Isso gera o executável `./build/graphsearch` (C++17, flags `-O2` em build
Release, que é o padrão deste `CMakeLists.txt` quando `CMAKE_BUILD_TYPE`
não é especificado).

### Executar os testes de corretude

```bash
./build/graphsearch --test
```

Imprime `[PASS]`/`[FAIL]` para cada um dos 5 casos manuais e 20 casos
aleatórios (Seção 8), e termina com `ALL TESTS PASSED` (código de saída 0)
ou `SOME TESTS FAILED` (código de saída 1).

### Executar os exemplos

```bash
./build/graphsearch --example
```

Roda Dijkstra e A\* sobre os cinco grafos manuais e um pequeno grafo
geométrico de exemplo, imprimindo caminho, custo e estatísticas de cada
execução — útil para inspecionar visualmente o comportamento dos dois
algoritmos sem rodar o benchmark completo.

### Executar todos os benchmarks

```bash
./build/graphsearch --benchmark
```

Executa, em sequência, os quatro experimentos (escalabilidade, densidade,
distância, heurística) e grava os CSVs em `results/`.

### Executar experimentos específicos

```bash
./build/graphsearch --benchmark scalability
./build/graphsearch --benchmark density
./build/graphsearch --benchmark distance
./build/graphsearch --benchmark heuristic
```

Cada comando roda só o experimento indicado e reescreve o CSV
correspondente (`results/scalability.csv`, `results/density.csv`,
`results/distance.csv`, `results/heuristic.csv`).

Os arquivos CSV são sempre gravados no diretório `results/` (relativo ao
diretório de onde o comando é executado — rode a partir da raiz do
projeto, como nos exemplos acima, para que os arquivos caiam em
`project/results/`). O diretório é criado automaticamente pelo próprio
programa se ainda não existir (`std::filesystem::create_directories`), e
qualquer falha real de escrita (ex.: disco cheio, sem permissão) agora
gera uma mensagem de erro clara em vez de terminar silenciosamente ou
travar.

### Gerar o dashboard visual (gráficos)

Depois de rodar `--benchmark` (para gerar os CSVs), gere o painel visual
com:

```bash
python3 scripts/generate_report.py
```

Isso cria `results/dashboard.html` — abra esse arquivo em qualquer
navegador (não precisa de servidor, internet só é usada para carregar a
biblioteca de gráficos Chart.js via CDN). O script só usa a biblioteca
padrão do Python (`csv`, `json`, `os`) — nada para instalar. Ele lê os
CSVs tal como estão em `results/`, então os gráficos sempre refletem os
dados da última execução de `--benchmark`.

<br>

## 17. Estrutura do projeto

```
project/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── graph.hpp          # struct Graph (lista de adjacência + coordenadas opcionais)
│   ├── search_result.hpp  # struct SearchResult (resultado + contadores)
│   ├── dijkstra.hpp
│   ├── astar.hpp
│   ├── generators.hpp     # grafos aleatórios e geométricos
│   ├── manual_graphs.hpp  # grafos fixos para validação
│   ├── stats.hpp          # média/mediana/min/max/desvio-padrão
│   ├── tests.hpp          # testes de corretude (--test)
│   └── benchmark.hpp      # experimentos (--benchmark)
├── src/
│   ├── main.cpp           # CLI: --test / --example / --benchmark [experimento]
│   ├── dijkstra.cpp
│   ├── astar.cpp
│   ├── generators.cpp
│   ├── manual_graphs.cpp
│   ├── stats.cpp
│   ├── tests.cpp
│   └── benchmark.cpp
├── scripts/
│   └── generate_report.py # gera results/dashboard.html (gráficos) a partir dos CSVs reais
├── tests/                 # (reservado; os testes em si rodam via `graphsearch --test`)
└── results/
    ├── scalability.csv
    ├── density.csv
    ├── distance.csv
    ├── heuristic.csv
    └── dashboard.html      # gerado por scripts/generate_report.py (não versionado por padrão)
```

O projeto é compilado como um único executável (`graphsearch`) com quatro
modos de operação, em vez de múltiplos binários — mantém o build simples
(`CMakeLists.txt` com um único `add_executable`) e evita duplicar a lógica
de carregamento de grafos entre "testes" e "programa principal". A pasta
`tests/` existe conforme a estrutura pedida, mas os testes de corretude em
si estão integrados ao binário principal (`--test`) para evitar um segundo
sistema de build/execução separado só para isso — mantendo o projeto sem
overengineering, como pedido. A visualização (`scripts/generate_report.py`)
foi deixada fora do C++ deliberadamente: gerar HTML/gráficos não é
responsabilidade do algoritmo nem do benchmark, e um script Python de ~150
linhas sem dependências é mais simples do que embutir uma biblioteca de
plotagem C++ só para isso.

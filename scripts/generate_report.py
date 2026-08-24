#!/usr/bin/env python3
"""
Gera results/dashboard.html com graficos interativos a partir dos CSVs
reais produzidos por `./build/graphsearch --benchmark`.

Nao inventa nenhum dado: le results/scalability.csv, density.csv,
distance.csv e heuristic.csv e embute os valores exatamente como estao
la dentro do HTML (via JSON), renderizando com Chart.js (carregado de um
CDN, sem instalar nada). So usa a biblioteca padrao do Python (csv, json,
os) -- nenhuma dependencia externa.

Uso:
    python3 scripts/generate_report.py
    # abre results/dashboard.html no navegador
"""

import csv
import json
import os
import sys

RESULTS_DIR = os.path.join(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "results"
)


def read_csv(name):
    path = os.path.join(RESULTS_DIR, name)
    if not os.path.exists(path):
        print(
            f"aviso: {path} nao encontrado (rode ./build/graphsearch --benchmark primeiro)",
            file=sys.stderr,
        )
        return []
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        rows = []
        for row in reader:
            for key in (
                "num_vertices",
                "num_edges",
                "source",
                "target",
                "seed",
                "repetitions",
                "found",
                "nodes_processed",
                "nodes_popped",
                "edges_examined",
                "relaxations",
                "queue_pushes",
                "max_queue_size",
            ):
                row[key] = int(row[key]) if row[key] != "" else 0
            for key in (
                "density",
                "mean_time_ms",
                "median_time_ms",
                "min_time_ms",
                "max_time_ms",
                "stddev_time_ms",
            ):
                row[key] = float(row[key]) if row[key] != "" else 0.0
            row["cost"] = float(row["cost"]) if row["cost"] != "" else None
            rows.append(row)
        return rows


def main():
    data = {
        "scalability": read_csv("scalability.csv"),
        "density": read_csv("density.csv"),
        "distance": read_csv("distance.csv"),
        "heuristic": read_csv("heuristic.csv"),
    }

    if not any(data.values()):
        print(
            "Nenhum CSV encontrado em results/. Rode ./build/graphsearch --benchmark antes.",
            file=sys.stderr,
        )
        sys.exit(1)

    data_json = json.dumps(data)

    html = HTML_TEMPLATE.replace("__DATA_JSON__", data_json)

    out_path = os.path.join(RESULTS_DIR, "dashboard.html")
    with open(out_path, "w") as f:
        f.write(html)

    print(f"gerado: {out_path}")


HTML_TEMPLATE = r"""<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<title>Dijkstra vs A* - Dashboard de Resultados</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.4/dist/chart.umd.min.js"></script>
<style>
  :root {
    --dijkstra: #e07a3f;
    --astar: #3f7ce0;
    --astar-h0: #9b59b6;
    --bg: #0f1115;
    --panel: #171a21;
    --text: #e8e8ea;
    --muted: #9a9ea8;
    --border: #2a2e38;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0; padding: 32px 24px 64px;
    background: var(--bg); color: var(--text);
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
  }
  h1 { font-size: 1.6rem; margin-bottom: 4px; }
  .subtitle { color: var(--muted); margin-bottom: 32px; font-size: 0.95rem; }
  h2 { font-size: 1.15rem; border-left: 4px solid var(--astar); padding-left: 10px; margin-top: 48px; }
  .note { color: var(--muted); font-size: 0.85rem; margin: 4px 0 16px; max-width: 780px; line-height: 1.5; }
  .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(420px, 1fr)); gap: 20px; margin-top: 16px; }
  .panel {
    background: var(--panel); border: 1px solid var(--border); border-radius: 10px;
    padding: 18px; min-height: 320px;
  }
  .panel h3 { margin: 0 0 12px; font-size: 0.95rem; color: var(--muted); font-weight: 600; }
  .legend { display: flex; gap: 18px; margin-bottom: 20px; font-size: 0.85rem; color: var(--muted); flex-wrap: wrap; }
  .legend span { display: inline-flex; align-items: center; gap: 6px; }
  .swatch { width: 12px; height: 12px; border-radius: 3px; display: inline-block; }
  table { border-collapse: collapse; width: 100%; font-size: 0.82rem; margin-top: 8px; }
  th, td { text-align: right; padding: 6px 10px; border-bottom: 1px solid var(--border); }
  th:first-child, td:first-child { text-align: left; }
  th { color: var(--muted); font-weight: 600; }
  tr:hover td { background: #1e222c; }
  footer { margin-top: 56px; color: var(--muted); font-size: 0.8rem; border-top: 1px solid var(--border); padding-top: 16px; }
</style>
</head>
<body>

<h1>Dijkstra vs A* - Dashboard de Resultados</h1>
<p class="subtitle">
  Gerado a partir dos CSVs reais em <code>results/</code> (produzidos por
  <code>./build/graphsearch --benchmark</code>). Nenhum valor nesta página foi
  inventado ou estimado - todos vêm diretamente das execuções instrumentadas.
</p>

<div class="legend">
  <span><span class="swatch" style="background:var(--dijkstra)"></span> Dijkstra</span>
  <span><span class="swatch" style="background:var(--astar)"></span> A* (heurística Euclidiana)</span>
  <span><span class="swatch" style="background:var(--astar-h0)"></span> A* com h(n)=0</span>
</div>

<h2>1. Escalabilidade</h2>
<p class="note">Grafos geométricos (k=6 vizinhos mais próximos), origem=0, destino=último vértice. Eixo X em escala logarítmica.</p>
<div class="grid">
  <div class="panel"><h3>Tempo médio de execução (ms, escala log)</h3><canvas id="chart-scal-time"></canvas></div>
  <div class="panel"><h3>Nós processados</h3><canvas id="chart-scal-nodes"></canvas></div>
</div>

<h2>2. Densidade do grafo</h2>
<p class="note">3000 vértices geométricos, arestas amostradas até a densidade alvo (peso = distância Euclidiana), origem=0, destino=2999.</p>
<div class="grid">
  <div class="panel"><h3>Tempo médio de execução (ms, escala log)</h3><canvas id="chart-dens-time"></canvas></div>
  <div class="panel"><h3>Nós processados</h3><canvas id="chart-dens-nodes"></canvas></div>
</div>

<h2>3. Distância origem-destino</h2>
<p class="note">Mesmo grafo geométrico (3000 vértices, k=6), origem fixa, alvo variando por percentil de distância Euclidiana (perto / médio / longe).</p>
<div class="grid">
  <div class="panel"><h3>Nós processados por categoria</h3><canvas id="chart-dist-nodes"></canvas></div>
  <div class="panel"><h3>Tempo médio de execução (ms)</h3><canvas id="chart-dist-time"></canvas></div>
</div>

<h2>4. Impacto da heurística</h2>
<p class="note">Mesmo grafo, mesma origem/destino - Dijkstra vs A* com h(n)=0 vs A* com heurística Euclidiana.</p>
<div class="grid">
  <div class="panel"><h3>Nós processados</h3><canvas id="chart-heur-nodes"></canvas></div>
  <div class="panel"><h3>Tempo médio de execução (ms)</h3><canvas id="chart-heur-time"></canvas></div>
</div>

<h2>Tabela completa</h2>
<div class="panel" style="overflow-x:auto">
  <table id="raw-table"></table>
</div>

<footer>
  Dados brutos: <code>results/scalability.csv</code>, <code>results/density.csv</code>,
  <code>results/distance.csv</code>, <code>results/heuristic.csv</code>.
  Regenerar após novos benchmarks com: <code>python3 scripts/generate_report.py</code>.
</footer>

<div id="cdn-error" style="display:none; position:fixed; top:0; left:0; right:0; background:#5a1f1f; color:#ffd9d9; padding:12px 20px; font-size:0.9rem; text-align:center; z-index:999;">
  Não foi possível carregar a biblioteca de gráficos (Chart.js) via CDN -
  verifique sua conexão com a internet. A tabela de dados brutos no fim da
  página continua funcionando normalmente.
</div>

<script>
const DATA = __DATA_JSON__;

if (typeof Chart === "undefined") {
  document.getElementById("cdn-error").style.display = "block";
} else {
const COLORS = { dijkstra: "#e07a3f", astar_euclidean: "#3f7ce0", astar_h0: "#9b59b6" };
const LABELS = { dijkstra: "Dijkstra", astar_euclidean: "A* (Euclidiana)", astar_h0: "A* (h=0)" };

Chart.defaults.color = "#9a9ea8";
Chart.defaults.borderColor = "#2a2e38";
Chart.defaults.font.size = 12;

function byAlgo(rows) {
  const out = {};
  for (const r of rows) {
    if (!out[r.algorithm]) out[r.algorithm] = [];
    out[r.algorithm].push(r);
  }
  return out;
}

function lineChart(canvasId, rows, xKey, yKey, xLabel, yLabel, logY) {
  const grouped = byAlgo(rows);
  const datasets = Object.entries(grouped).map(([algo, pts]) => ({
    label: LABELS[algo] || algo,
    data: pts.map(p => ({ x: p[xKey], y: p[yKey] })),
    borderColor: COLORS[algo] || "#888",
    backgroundColor: COLORS[algo] || "#888",
    tension: 0.15,
    pointRadius: 4,
  }));
  new Chart(document.getElementById(canvasId), {
    type: "line",
    data: { datasets },
    options: {
      responsive: true,
      scales: {
        x: { type: "logarithmic", title: { display: true, text: xLabel } },
        y: { type: logY ? "logarithmic" : "linear", title: { display: true, text: yLabel } },
      },
      plugins: { legend: { position: "bottom" } },
    },
  });
}

function barChart(canvasId, rows, xKey, yKey, xLabel, yLabel, logY) {
  const labels = [...new Set(rows.map(r => r[xKey]))];
  const grouped = byAlgo(rows);
  const datasets = Object.entries(grouped).map(([algo, pts]) => {
    const byLabel = Object.fromEntries(pts.map(p => [p[xKey], p[yKey]]));
    return {
      label: LABELS[algo] || algo,
      data: labels.map(l => byLabel[l] ?? 0),
      backgroundColor: COLORS[algo] || "#888",
    };
  });
  new Chart(document.getElementById(canvasId), {
    type: "bar",
    data: { labels, datasets },
    options: {
      responsive: true,
      scales: {
        x: { title: { display: true, text: xLabel } },
        y: { type: logY ? "logarithmic" : "linear", title: { display: true, text: yLabel } },
      },
      plugins: { legend: { position: "bottom" } },
    },
  });
}

if (DATA.scalability.length) {
  lineChart("chart-scal-time", DATA.scalability, "num_vertices", "mean_time_ms", "Vértices", "ms", true);
  lineChart("chart-scal-nodes", DATA.scalability, "num_vertices", "nodes_processed", "Vértices", "nós processados", true);
}
if (DATA.density.length) {
  lineChart("chart-dens-time", DATA.density, "density", "mean_time_ms", "Densidade", "ms", true);
  lineChart("chart-dens-nodes", DATA.density, "density", "nodes_processed", "Densidade", "nós processados", true);
}
if (DATA.distance.length) {
  const order = { near: 0, medium: 1, far: 2 };
  const sorted = [...DATA.distance].sort((a, b) => order[a.label] - order[b.label]);
  barChart("chart-dist-nodes", sorted, "label", "nodes_processed", "Categoria", "nós processados", false);
  barChart("chart-dist-time", sorted, "label", "mean_time_ms", "Categoria", "ms", false);
}
if (DATA.heuristic.length) {
  barChart("chart-heur-nodes", DATA.heuristic, "algorithm", "nodes_processed", "", "nós processados", false);
  barChart("chart-heur-time", DATA.heuristic, "algorithm", "mean_time_ms", "", "ms", false);
}
} // fim do bloco "if (typeof Chart !== 'undefined')"

// Raw data table (all experiments concatenated) - independe de Chart.js.
const allRows = [
  ...DATA.scalability.map(r => ({ experiment: "scalability", ...r })),
  ...DATA.density.map(r => ({ experiment: "density", ...r })),
  ...DATA.distance.map(r => ({ experiment: "distance", ...r })),
  ...DATA.heuristic.map(r => ({ experiment: "heuristic", ...r })),
];
if (allRows.length) {
  const cols = ["experiment", "algorithm", "label", "num_vertices", "num_edges", "density",
                "target", "found", "cost", "mean_time_ms", "nodes_processed", "edges_examined",
                "relaxations", "queue_pushes"];
  const table = document.getElementById("raw-table");
  const thead = document.createElement("thead");
  thead.innerHTML = "<tr>" + cols.map(c => `<th>${c}</th>`).join("") + "</tr>";
  table.appendChild(thead);
  const tbody = document.createElement("tbody");
  for (const r of allRows) {
    const tr = document.createElement("tr");
    tr.innerHTML = cols.map(c => {
      let v = r[c];
      if (typeof v === "number") v = Number.isInteger(v) ? v : v.toFixed(4);
      return `<td>${v ?? ""}</td>`;
    }).join("");
    tbody.appendChild(tr);
  }
  table.appendChild(tbody);
}
</script>
</body>
</html>
"""

if __name__ == "__main__":
    main()

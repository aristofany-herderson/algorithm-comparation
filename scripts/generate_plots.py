#!/usr/bin/env python3
import math
import os
import sys

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd

RESULTS_DIR = os.path.join(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "results"
)
PLOTS_DIR = os.path.join(RESULTS_DIR, "plots")

ALGO_COLORS = {
    "dijkstra": "#1f77b4",
    "astar_euclidean": "#d62728",
    "astar_h0": "#ff7f0e",
}

ALGO_LABELS = {
    "dijkstra": "Dijkstra",
    "astar_euclidean": "A* (heurística euclidiana)",
    "astar_h0": "A* (h(n) = 0)",
}


def load_csv(name):
    path = os.path.join(RESULTS_DIR, name)
    if not os.path.exists(path):
        print(f"aviso: {path} nao encontrado", file=sys.stderr)
        return None
    df = pd.read_csv(path)
    if df.empty:
        print(f"aviso: {path} esta vazio", file=sys.stderr)
        return None
    return df


def save(fig, filename):
    os.makedirs(PLOTS_DIR, exist_ok=True)
    out_path = os.path.join(PLOTS_DIR, filename)
    fig.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"gerado: {out_path}")


def plot_scalability():
    df = load_csv("scalability.csv")
    if df is None:
        return

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    ax = axes[0]
    for algo, group in df.groupby("algorithm"):
        group = group.sort_values("num_vertices")
        ax.plot(
            group["num_vertices"],
            group["mean_time_ms"],
            marker="o",
            color=ALGO_COLORS.get(algo),
            label=ALGO_LABELS.get(algo, algo),
        )
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("Número de vértices")
    ax.set_ylabel("Tempo médio (ms)")
    ax.set_title("Escalabilidade: tempo de execução")
    ax.legend()
    ax.grid(True, which="both", linestyle="--", alpha=0.4)

    ax = axes[1]
    for algo, group in df.groupby("algorithm"):
        group = group.sort_values("num_vertices")
        ax.plot(
            group["num_vertices"],
            group["nodes_processed"],
            marker="o",
            color=ALGO_COLORS.get(algo),
            label=ALGO_LABELS.get(algo, algo),
        )
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("Número de vértices")
    ax.set_ylabel("Nós processados")
    ax.set_title("Escalabilidade: nós processados")
    ax.legend()
    ax.grid(True, which="both", linestyle="--", alpha=0.4)

    fig.suptitle("Dijkstra vs A* - Experimento de Escalabilidade")
    save(fig, "scalability.png")


def plot_density():
    df = load_csv("density.csv")
    if df is None:
        return

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    ax = axes[0]
    for algo, group in df.groupby("algorithm"):
        group = group.sort_values("density")
        ax.plot(
            group["density"],
            group["mean_time_ms"],
            marker="o",
            color=ALGO_COLORS.get(algo),
            label=ALGO_LABELS.get(algo, algo),
        )
    ax.set_xscale("log")
    ax.set_xlabel("Densidade do grafo")
    ax.set_ylabel("Tempo médio (ms)")
    ax.set_title("Densidade: tempo de execução")
    ax.legend()
    ax.grid(True, which="both", linestyle="--", alpha=0.4)

    ax = axes[1]
    for algo, group in df.groupby("algorithm"):
        group = group.sort_values("density")
        ax.plot(
            group["density"],
            group["nodes_processed"],
            marker="o",
            color=ALGO_COLORS.get(algo),
            label=ALGO_LABELS.get(algo, algo),
        )
    ax.set_xscale("log")
    ax.set_xlabel("Densidade do grafo")
    ax.set_ylabel("Nós processados")
    ax.set_title("Densidade: nós processados")
    ax.legend()
    ax.grid(True, which="both", linestyle="--", alpha=0.4)

    fig.suptitle("Dijkstra vs A* - Experimento de Densidade")
    save(fig, "density.png")


def plot_distance():
    df = load_csv("distance.csv")
    if df is None:
        return

    order = ["near", "medium", "far"]
    df["label"] = pd.Categorical(df["label"], categories=order, ordered=True)
    df = df.sort_values("label")

    algos = list(df["algorithm"].unique())
    x = range(len(order))
    width = 0.8 / max(len(algos), 1)

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    ax = axes[0]
    for i, algo in enumerate(algos):
        sub = df[df["algorithm"] == algo].set_index("label").reindex(order)
        offsets = [xi + i * width for xi in x]
        ax.bar(
            offsets,
            sub["mean_time_ms"],
            width=width,
            color=ALGO_COLORS.get(algo),
            label=ALGO_LABELS.get(algo, algo),
        )
    ax.set_xticks([xi + width * (len(algos) - 1) / 2 for xi in x])
    ax.set_xticklabels(["Perto", "Médio", "Longe"])
    ax.set_ylabel("Tempo médio (ms)")
    ax.set_title("Distância: tempo de execução")
    ax.legend()
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)

    ax = axes[1]
    for i, algo in enumerate(algos):
        sub = df[df["algorithm"] == algo].set_index("label").reindex(order)
        offsets = [xi + i * width for xi in x]
        ax.bar(
            offsets,
            sub["nodes_processed"],
            width=width,
            color=ALGO_COLORS.get(algo),
            label=ALGO_LABELS.get(algo, algo),
        )
    ax.set_xticks([xi + width * (len(algos) - 1) / 2 for xi in x])
    ax.set_xticklabels(["Perto", "Médio", "Longe"])
    ax.set_ylabel("Nós processados")
    ax.set_title("Distância: nós processados")
    ax.legend()
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)

    fig.suptitle("Dijkstra vs A* - Experimento de Distância")
    save(fig, "distance.png")


def plot_heuristic():
    df = load_csv("heuristic.csv")
    if df is None:
        return

    df = df.set_index("algorithm")
    order = [a for a in ["dijkstra", "astar_h0", "astar_euclidean"] if a in df.index]
    colors = [ALGO_COLORS.get(a) for a in order]
    labels = [ALGO_LABELS.get(a, a) for a in order]

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    ax = axes[0]
    ax.bar(labels, df.loc[order, "mean_time_ms"], color=colors)
    ax.set_ylabel("Tempo médio (ms)")
    ax.set_title("Impacto da heurística: tempo de execução")
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)
    ax.tick_params(axis="x", rotation=15)

    ax = axes[1]
    ax.bar(labels, df.loc[order, "nodes_processed"], color=colors)
    ax.set_ylabel("Nós processados")
    ax.set_title("Impacto da heurística: nós processados")
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)
    ax.tick_params(axis="x", rotation=15)

    fig.suptitle("Dijkstra vs A* - Impacto da Heurística")
    save(fig, "heuristic.png")


def plot_theory_vs_empirical():
    df = load_csv("scalability.csv")
    if df is None:
        return

    fig, axes = plt.subplots(1, 2, figsize=(13, 5.5))

    theory_specs = [
        ("O(V log V)", lambda v, e: v * math.log2(v), "--", "#7f7f7f"),
        ("O((V+E) log V)", lambda v, e: (v + e) * math.log2(v), "--", "#2ca02c"),
    ]

    for ax, algo in zip(axes, ["dijkstra", "astar_euclidean"]):
        group = df[df["algorithm"] == algo].sort_values("num_vertices")
        if group.empty:
            continue

        v = group["num_vertices"].to_numpy()
        e = group["num_edges"].to_numpy()
        t = group["mean_time_ms"].to_numpy()
        t_norm = t / t.max()

        ax.plot(
            v,
            t_norm,
            marker="o",
            color=ALGO_COLORS.get(algo),
            label=f"{ALGO_LABELS.get(algo, algo)} (empírico)",
            linewidth=2,
        )

        for name, fn, style, color in theory_specs:
            theory = [fn(vi, ei) for vi, ei in zip(v, e)]
            theory_max = max(theory)
            theory_norm = [x / theory_max for x in theory]
            ax.plot(v, theory_norm, style, color=color, label=f"{name} teórico")

        ax.set_xscale("log")
        ax.set_xlabel("Número de vértices (V)")
        ax.set_ylabel("Tempo normalizado")
        ax.set_title(f"{ALGO_LABELS.get(algo, algo)}: teoria x experimento")
        ax.legend(fontsize=8)
        ax.grid(True, which="both", linestyle="--", alpha=0.4)

    fig.suptitle("Comparação entre análise teórica e empírica (Dijkstra vs A*)")
    save(fig, "theory_vs_empirical.png")


def main():
    plot_scalability()
    plot_density()
    plot_distance()
    plot_heuristic()
    plot_theory_vs_empirical()


if __name__ == "__main__":
    main()

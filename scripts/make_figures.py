"""Figures for the industrial-software migration benchmark.

Every figure is projected from the CSVs in `data/analysis_out/`, so a figure can
never disagree with the table it illustrates. Output goes to a fresh directory
because a viewer left open on a previous PNG holds an exclusive lock.
"""

import os
import csv
import json
import sys

import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
DATA = os.path.join(ROOT, "data")
TABLES = os.path.join(DATA, "analysis_out")
FIG = os.path.join(ROOT, "figures")
os.makedirs(FIG, exist_ok=True)
FIGDIR = os.path.join(ROOT, "figures_run")
os.makedirs(FIGDIR, exist_ok=True)

C = {"a": "#1f4e79", "b": "#c0504d", "grey": "#808080", "light": "#d9d9d9"}


def load(n, default=None):
    for d in (TABLES, DATA):
        p = os.path.join(d, n)
        if os.path.exists(p):
            return list(csv.DictReader(open(p, encoding="utf-8")))
    return default if default is not None else []


def num(x):
    try:
        return float(x)
    except (TypeError, ValueError):
        return None


def short(s, n=26):
    s = str(s)
    return s if len(s) <= n else s[:n - 1] + "\u2026"


# ------------------------------------------------------- Fig 1: the corpus
def fig_corpus():
    man = json.load(open(os.path.join(DATA, "benchmark_tasks.json"),
                         encoding="utf-8"))
    tasks = man["tasks"]
    fig, axes = plt.subplots(1, 2, figsize=(10.4, 3.4))
    fig.subplots_adjust(left=0.115, right=0.985, top=0.83, bottom=0.17,
                        wspace=0.40)

    ax = axes[0]
    by = {}
    for t in tasks:
        by.setdefault(t["project"], []).append(t)
    names = sorted(by, key=lambda k: -len(by[k]))
    y = np.arange(len(names))
    ax.barh(y, [len(by[k]) for k in names], color=C["a"], height=0.6)
    ax.set_yticks(y)
    ax.set_yticklabels([short(k, 20) for k in names], fontsize=7.6)
    for i, k in enumerate(names):
        ax.text(len(by[k]) + 0.06, i, str(len(by[k])), va="center", fontsize=7.4)
    ax.set_xlim(0, max(len(by[k]) for k in names) + 0.8)
    ax.set_xlabel("migration pairs contributed")
    ax.set_title("(a) corpus by project", loc="left", fontsize=9)

    ax = axes[1]
    fr = sorted(t["change_fraction"] for t in tasks)
    ax.hist(fr, bins=10, color=C["a"], edgecolor="white")
    for v in fr:
        ax.axvline(v, color=C["light"], lw=0.6, zorder=0)
    ax.axvline(np.median(fr), color=C["b"], ls="--", lw=1.1,
               label=f"median = {np.median(fr):.2f}")
    ax.set_xlabel("share of changed non-blank lines")
    ax.set_ylabel("tasks")
    ax.legend(fontsize=7.4, frameon=False)
    ax.set_title("(b) how substantive the migration is", loc="left", fontsize=9)

    fig.suptitle("Fig. 1  The migration corpus, built from real repository "
                 "history", x=0.012, y=0.985, ha="left", fontsize=10)
    fig.savefig(os.path.join(FIGDIR, "fig1_corpus.png"), dpi=200)
    plt.close(fig)
    print("[fig] fig1_corpus")


# ------------------------------- Fig 2: coverage and proximity, per model
def fig_outcomes():
    rows = load("attempt_metrics.csv")
    if not rows:
        return
    res = json.load(open(os.path.join(TABLES, "analysis_results.json"),
                         encoding="utf-8"))
    deg = set(res["population"]["degenerate_task_ids"])
    rows = [r for r in rows if r["task_id"] not in deg]
    models = sorted({r["model"] for r in rows})

    fig, axes = plt.subplots(1, 2, figsize=(9.2, 3.2))

    ax = axes[0]
    data = [[num(r["intent_coverage"]) for r in rows if r["model"] == m]
            for m in models]
    bp = ax.boxplot(data, tick_labels=[short(m, 22) for m in models],
                    widths=0.45, patch_artist=True, showfliers=False)
    for patch in bp["boxes"]:
        patch.set_facecolor(C["a"])
        patch.set_alpha(0.45)
    for med in bp["medians"]:
        med.set_color(C["b"])
        med.set_linewidth(1.4)
    for i, m in enumerate(models, start=1):
        v = [num(r["intent_coverage"]) for r in rows if r["model"] == m]
        ax.scatter([i + 0.13] * len(v), v, s=11, color=C["a"], alpha=0.75,
                   zorder=3, linewidths=0)
    ax.axhline(1.0, color=C["grey"], ls=":", lw=0.9)
    ax.text(0.55, 1.02, "all intents covered", fontsize=6.8, color=C["grey"])
    ax.set_ylim(-0.05, 1.12)
    ax.set_ylabel("requested intents present")
    ax.set_title("(a) intent coverage", loc="left", fontsize=9)

    ax = axes[1]
    data = [[num(r["structural_similarity_to_modern"]) for r in rows
             if r["model"] == m] for m in models]
    bp = ax.boxplot(data, tick_labels=[short(m, 22) for m in models],
                    widths=0.45, patch_artist=True, showfliers=False)
    for patch in bp["boxes"]:
        patch.set_facecolor(C["a"])
        patch.set_alpha(0.45)
    for med in bp["medians"]:
        med.set_color(C["b"])
        med.set_linewidth(1.4)
    for i, m in enumerate(models, start=1):
        v = [num(r["structural_similarity_to_modern"]) for r in rows
             if r["model"] == m]
        ax.scatter([i + 0.13] * len(v), v, s=11, color=C["a"], alpha=0.75,
                   zorder=3, linewidths=0)
    ax.set_ylabel("similarity to human revision")
    ax.set_title("(b) proximity to the revision a human committed",
                 loc="left", fontsize=9)

    fig.suptitle("Fig. 2  What the models actually produce on real industrial "
                 "migration tasks", x=0.012, y=0.985, ha="left", fontsize=10)
    fig.savefig(os.path.join(FIGDIR, "fig2_outcomes.png"), dpi=200)
    plt.close(fig)
    print("[fig] fig2_outcomes")


# --------------------------------- Fig 3: per-intent miss rate + failures
def fig_intents():
    r3 = load("analysis_rq3_intents.csv")
    fails = load("analysis_compile_failures.csv")
    if not r3 and not fails:
        return
    fig, axes = plt.subplots(1, 2, figsize=(10.4, 3.2))
    fig.subplots_adjust(left=0.135, right=0.985, top=0.82, bottom=0.17,
                        wspace=0.44)

    ax = axes[0]
    if r3:
        r3 = sorted(r3, key=lambda r: -num(r["miss_rate"]))
        y = np.arange(len(r3))
        vals = [num(r["miss_rate"]) * 100 for r in r3]
        cols = [C["b"] if v >= 50 else C["a"] for v in vals]
        ax.barh(y, vals, color=cols, height=0.62)
        for i, r in enumerate(r3):
            ax.text(vals[i] + 1.6, i,
                    f"{vals[i]:.0f}%  of {r['n_opportunities']}",
                    va="center", fontsize=7.0)
        ax.set_yticks(y)
        ax.set_yticklabels([r["intent"] for r in r3], fontsize=7.4)
        ax.set_xlim(0, 132)
        ax.axvline(50, color=C["grey"], ls="--", lw=0.9)
        ax.set_xlabel("opportunities in which the intent was missed (%)")
        ax.set_title("(a) which migration intents get missed", loc="left",
                     fontsize=9)

    ax = axes[1]
    if fails:
        fails = sorted(fails, key=lambda r: -num(r["n"]))
        y = np.arange(len(fails))
        vals = [num(r["n"]) for r in fails]
        ax.barh(y, vals, color=C["a"], height=0.58)
        for i, r in enumerate(fails):
            ax.text(vals[i] + 0.12, i,
                    f"{vals[i]:.0f}  ({float(r['share'])*100:.0f}%)",
                    va="center", fontsize=7.2)
        ax.set_yticks(y)
        ax.set_yticklabels([short(r["cause"], 24) for r in fails], fontsize=7.4)
        ax.set_xlim(0, max(vals) * 1.35 + 1)
        ax.set_xlabel("attempts")
        ax.set_title("(b) why an attempt does not build", loc="left",
                     fontsize=9)

    fig.suptitle("Fig. 3  Failure structure: which intents are missed, and "
                 "what breaks the build", x=0.012, y=0.985, ha="left", fontsize=10)
    fig.savefig(os.path.join(FIGDIR, "fig3_failures.png"), dpi=200)
    plt.close(fig)
    print("[fig] fig3_failures")


# ------------------------- Fig 4: screening + the degenerate-pair evidence
def fig_screening():
    man = json.load(open(os.path.join(DATA, "benchmark_tasks.json"),
                         encoding="utf-8"))
    st = man["screening"]["stats"]
    order = [("accepted", "accepted as a task", C["a"]),
             ("change_too_small", "change below threshold", C["b"]),
             ("no_intent", "no migration intent", C["grey"]),
             ("too_large", "file too large", C["light"]),
             ("no_local_legacy", "no readable legacy blob", C["light"]),
             ("normalised_equivalent", "same program after normalisation",
              C["b"])]
    rows = [(lbl, st.get(k, 0), col) for k, lbl, col in order if st.get(k, 0)]
    if not rows:
        return
    fig, ax = plt.subplots(figsize=(8.2, 3.4))
    fig.subplots_adjust(left=0.30, right=0.985, top=0.72, bottom=0.19)
    y = np.arange(len(rows))
    vals = [r[1] for r in rows]
    ax.barh(y, vals, color=[r[2] for r in rows], height=0.6)
    for i, v in enumerate(vals):
        ax.text(v + max(vals) * 0.015 + 0.2, i, str(v), va="center",
                fontsize=7.6)
    ax.set_yticks(y)
    ax.set_yticklabels([r[0] for r in rows], fontsize=7.8)
    ax.invert_yaxis()
    ax.set_xlim(0, max(vals) * 1.18 + 1)
    ax.set_xlabel("candidate files examined")
    ax.set_title("Fig. 4  What the construction phase rejected, and why\n"
                 "a line-multiset change threshold is not sufficient:\n"
                 "one accepted pair differed only by an added #define",
                 loc="left", fontsize=9.4)
    fig.savefig(os.path.join(FIGDIR, "fig4_screening.png"), dpi=200)
    plt.close(fig)
    print("[fig] fig4_screening")


if __name__ == "__main__":
    fig_corpus()
    fig_outcomes()
    fig_intents()
    fig_screening()
    print("FIGURES COMPLETE ->", FIGDIR)

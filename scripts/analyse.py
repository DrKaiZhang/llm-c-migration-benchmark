"""Statistics for the industrial-software migration benchmark.

Every number quoted in the manuscript is produced here from
`data/analysis_out/attempt_metrics.csv`, which the harness writes by evaluating
each attempt mechanically. The questions are:

RQ1  How far does a small code model get on real industrial migration tasks,
     measured by intent coverage and by structural proximity to the revision a
     human actually committed?
RQ2  Do the two model sizes differ, and where?
RQ3  Which migration intents does a model reliably miss, and is the miss rate
     stable across repetitions of the same task?
RQ4  Does an attempt that covers the requested intents also look more like the
     human revision? (an internal-validity check on the intent axis)
"""

import os
import re
import csv
import sys
import json
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(ROOT, "..", "00_shared"))
try:
    import stats_core as ST
except ImportError:  # pragma: no cover
    import stats_tools as ST

DATA = os.path.join(ROOT, "data")
TABLES = os.path.join(DATA, "analysis_out")
os.makedirs(TABLES, exist_ok=True)

MAN = os.path.join(DATA, "benchmark_tasks.json")


def f(v, cast=float):
    if v is None or v == "":
        return None
    try:
        return cast(v)
    except (TypeError, ValueError):
        return None


def ordered_code(text):
    """Comment-stripped, whitespace-normalised, order-preserving line list."""
    t = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    t = re.sub(r"//[^\n]*", " ", t)
    return [re.sub(r"\s+", " ", l).strip() for l in t.splitlines()
            if re.sub(r"\s+", " ", l).strip()]


def load():
    p = os.path.join(TABLES, "attempt_metrics.csv")
    if not os.path.exists(p):
        raise SystemExit("no attempt_metrics.csv -- run harness.py --evaluate")
    rows = []
    for r in csv.DictReader(open(p, encoding="utf-8")):
        rows.append(dict(
            model=r["model"], task_id=r["task_id"], rep=f(r["rep"], int),
            project=r["project"], domain=r["domain"],
            compiles=f(r["compiles"], int),
            compile_errors=f(r["compile_errors"], int),
            n_intents=f(r["n_intents"], int),
            n_intents_covered=f(r["n_intents_covered"], int),
            intents_covered=r.get("intents_covered", ""),
            intent_coverage=f(r["intent_coverage"]),
            behaviour=r["behaviour"],
            lines=f(r["lines"], int), legacy_lines=f(r["legacy_lines"], int),
            line_delta=f(r["line_delta"], int),
            identical_to_modern=f(r["identical_to_modern"], int),
            identical_to_legacy=f(r["identical_to_legacy"], int),
            structural_similarity_to_modern=f(
                r["structural_similarity_to_modern"]),
            change_fraction_task=f(r["change_fraction_task"]),
            error_head=r.get("error_head", ""),
        ))
    return rows


def main():
    rows = load()
    man = json.load(open(MAN, encoding="utf-8"))
    tasks = {t["task_id"]: t for t in man["tasks"]}

    # A pair whose two sides are identical once comments and whitespace are
    # normalised carries no migration. Such a pair can still clear a
    # line-multiset change threshold (adding one `#define` changes the multiset
    # but not the program), so it is excluded here and the exclusion is
    # reported rather than silently applied.
    degenerate = set()
    for tid in sorted({x["task_id"] for x in rows}):
        d = os.path.join(DATA, "sources", tid)
        lp, mp = os.path.join(d, "legacy.c"), os.path.join(d, "modern.c")
        if not (os.path.exists(lp) and os.path.exists(mp)):
            continue
        a = ordered_code(open(lp, encoding="utf-8", errors="ignore").read())
        b = ordered_code(open(mp, encoding="utf-8", errors="ignore").read())
        if a == b:
            degenerate.add(tid)
    clean = [x for x in rows if x["task_id"] not in degenerate]

    models = sorted({x["model"] for x in clean})
    out = {
        "population": dict(
            n_tasks_constructed=man["n_tasks"],
            n_tasks_attempted=len({x["task_id"] for x in clean}),
            n_attempts=len(clean),
            n_degenerate_pairs_excluded=len(degenerate),
            degenerate_task_ids=sorted(degenerate),
            models=models,
            reps_per_task=len({x["rep"] for x in clean}),
            projects=sorted({x["project"] for x in clean}),
            construction_screening=man["screening"]["stats"],
            context_line_budget=260,
        ),
    }

    # ---------------------------------------------------------------- RQ1
    per_model = []
    for m in models:
        g = [x for x in clean if x["model"] == m]
        cov = sorted(x["intent_coverage"] for x in g)
        sim = sorted(x["structural_similarity_to_modern"] for x in g)
        comp = [x["compiles"] for x in g]
        by_task = defaultdict(list)
        for x in g:
            by_task[x["task_id"]].append(x["intent_coverage"])
        full = [1 if min(v) >= 0.999 else 0 for v in by_task.values()]
        per_model.append(dict(
            model=m, n_attempts=len(g), n_tasks=len(by_task),
            coverage=ST.describe(cov, "intent_coverage"),
            coverage_mean=round(sum(cov) / len(cov), 4) if cov else 0,
            coverage_full_tasks=sum(full),
            coverage_full_share=round(sum(full) / len(full), 4) if full else 0,
            structural_similarity=ST.describe(sim, "sim_to_modern"),
            sim_mean=round(sum(sim) / len(sim), 4) if sim else 0,
            compiles=sum(comp),
            compile_rate=round(sum(comp) / len(comp), 4) if comp else 0,
            median_line_delta=ST.quantile(
                sorted(x["line_delta"] for x in g), 0.5),
            identical_to_modern=sum(x["identical_to_modern"] for x in g),
            identical_to_legacy=sum(x["identical_to_legacy"] for x in g),
            zero_coverage=sum(1 for c in cov if c == 0),
        ))
    out["rq1_per_model"] = per_model
    ST.write_csv(per_model, os.path.join(TABLES, "analysis_rq1_models.csv"))

    close = []
    for m in models:
        g = [x for x in clean if x["model"] == m]
        sims = [x["structural_similarity_to_modern"] for x in g]
        for thr in (0.25, 0.5, 0.75):
            n = sum(1 for s in sims if s >= thr)
            close.append(dict(model=m, threshold=thr, n=n,
                              share=round(n / len(sims), 4)))
    out["rq1_similarity_thresholds"] = close
    ST.write_csv(close, os.path.join(TABLES, "analysis_rq1_similarity.csv"))

    # ---------------------------------------------------------------- RQ2
    rq2 = {}
    if len(models) >= 2:
        for key, field in (("coverage", "intent_coverage"),
                           ("similarity", "structural_similarity_to_modern")):
            a = [x[field] for x in clean if x["model"] == models[0]]
            b = [x[field] for x in clean if x["model"] == models[1]]
            rq2[key] = dict(
                model_a=models[0], model_b=models[1],
                median_a=ST.quantile(sorted(a), 0.5),
                median_b=ST.quantile(sorted(b), 0.5),
                mannwhitney_u=ST.mannwhitney_u(sorted(a), sorted(b)),
                cliffs_delta=ST.cliffs_delta(a, b),
                delta_ci=ST.cliffs_delta_ci(a, b))
    out["rq2_model_comparison"] = rq2

    # ---------------------------------------------------------------- RQ3
    miss, tot = Counter(), Counter()
    for x in clean:
        want = [i["id"] for i in tasks[x["task_id"]]["intents"]]
        got = set(filter(None, x["intents_covered"].split(";")))
        for i in want:
            tot[i] += 1
            if i not in got:
                miss[i] += 1
    rows3 = [dict(intent=i, n_opportunities=tot[i], n_missed=miss[i],
                  miss_rate=round(miss[i] / tot[i], 4) if tot[i] else None)
             for i in sorted(tot, key=lambda k: -(miss[k] / tot[k])
                             if tot[k] else 0)]
    out["rq3_intent_miss_rate"] = rows3
    ST.write_csv(rows3, os.path.join(TABLES, "analysis_rq3_intents.csv"))

    spread = []
    by = defaultdict(list)
    for x in clean:
        by[(x["model"], x["task_id"])].append(x["intent_coverage"])
    for (m, t), v in sorted(by.items()):
        if len(v) < 2:
            continue
        spread.append(dict(model=m, task_id=t, n=len(v), min=min(v),
                           max=max(v), range=round(max(v) - min(v), 4),
                           identical=int(len({round(x, 6) for x in v}) == 1)))
    out["rq3_repetition_stability"] = dict(
        n_pairs=len(spread),
        n_identical=sum(s["identical"] for s in spread),
        median_range=ST.quantile([s["range"] for s in spread], 0.5)
        if spread else None,
        detail=spread)
    ST.write_csv(spread, os.path.join(TABLES, "analysis_rq3_stability.csv"))

    # ---------------------------------------------------------------- RQ4
    xs = [x["intent_coverage"] for x in clean]
    ys = [x["structural_similarity_to_modern"] for x in clean]
    sp = ST.spearman(xs, ys)
    out["rq4_coverage_vs_similarity"] = dict(
        spearman_rho=sp["rho"], p=sp["p"], n=len(xs),
        interpretation=("a positive association means an attempt that supplies "
                        "the requested idioms is also closer to the revision a "
                        "human committed, which is what makes the intent axis a "
                        "usable proxy rather than a tautology"))
    proj = defaultdict(list)
    for x in clean:
        proj[x["project"]].append(x["intent_coverage"])
    out["rq4_by_project"] = [
        dict(project=p, n_attempts=len(v),
             mean_coverage=round(sum(v) / len(v), 4),
             n_tasks=len({x["task_id"] for x in clean
                          if x["project"] == p}))
        for p, v in sorted(proj.items(), key=lambda kv: -sum(kv[1]) / len(kv[1]))]
    ST.write_csv(out["rq4_by_project"],
                 os.path.join(TABLES, "analysis_rq4_project.csv"))

    fails = Counter()
    for x in clean:
        if x["compiles"]:
            continue
        head = (x["error_head"] or "").strip()
        key = "other"
        for pat, name in (
                ("No module named", "toolchain-missing"),
                ("unterminated conditional", "truncated-output"),
                ("unterminated comment", "truncated-output"),
                ("unterminated string", "truncated-output"),
                ("expected '}'", "truncated-output"),
                ("use of undeclared identifier", "undeclared-identifier"),
                ("unknown type name", "unknown-type"),
                ("variable has incomplete type", "incomplete-type"),
                ("conflicting types", "conflicting-types"),
                ("implicit declaration", "implicit-declaration"),
                ("expected", "syntax")):
            if pat in head:
                key = name
                break
        fails[key] += 1
    out["compile_failure_taxonomy"] = dict(
        n_failed=sum(fails.values()), n_total=len(clean),
        note=("the compile axis is a quality marker, not a gate: a translation "
              "unit needs platform types and configuration macros that do not "
              "exist inside the unit, so a standalone build cannot be expected "
              "to succeed for most of this corpus"),
        by_cause=[dict(cause=k, n=v,
                       share=round(v / max(1, sum(fails.values())), 4))
                  for k, v in fails.most_common()])
    ST.write_csv(out["compile_failure_taxonomy"]["by_cause"],
                 os.path.join(TABLES, "analysis_compile_failures.csv"))

    ST.write_json(out, os.path.join(TABLES, "analysis_results.json"))
    for k in ("population", "rq1_per_model", "rq2_model_comparison",
              "rq3_intent_miss_rate", "rq3_repetition_stability",
              "rq4_coverage_vs_similarity", "rq4_by_project",
              "compile_failure_taxonomy"):
        ST.report(k, out[k])
    print("ANALYSIS COMPLETE ->", os.path.join(TABLES,
                                                "analysis_results.json"))


if __name__ == "__main__":
    main()

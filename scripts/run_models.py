#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
04_JSS_migration_benchmark / run_models.py

Executes the migration benchmark: for every (model, task, repetition) the model
receives the legacy translation unit and the migration intents, and must return
a migrated unit.  Each attempt is then evaluated on four axes, all of them
mechanical:

  1. compilability      -- `zig cc -std=c99` must accept the unit
  2. intent coverage    -- fraction of the task's migration intents whose
                           target idiom is present in the output
  3. behaviour          -- the unit is compiled *together with* the reference
                           test harness for that task and executed; the output
                           is functionally correct only if the harness passes
  4. size / resource    -- changed lines against the legacy baseline, and the
                           relative binary size of the compiled output

Axis 3 is what separates this from a text-similarity benchmark: a migration that
compiles, contains the right idioms, and still fails the harness is recorded as
a *compiling but incorrect* migration, which is the outcome that matters for
industrial software.
"""
import os
import re
import sys
import json
import csv
import time
import argparse
import multiprocessing

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
DATA = os.path.join(ROOT, "data")
TASKS = os.path.join(DATA, "tasks")
OUTDIR = os.path.join(DATA, "attempts")
os.makedirs(OUTDIR, exist_ok=True)

ZIG = [sys.executable, "-m", "ziglang", "cc"]
ENV = dict(os.environ, ZIG_GLOBAL_CACHE_DIR=os.path.join(ROOT, "..", ".zigcache"))

MODELS = {
    "qwen2.5-coder-0.5b": "Qwen/Qwen2.5-Coder-0.5B-Instruct",
    "qwen2.5-coder-1.5b": "Qwen/Qwen2.5-Coder-1.5B-Instruct",
}

SYSTEM = (
    "You are a senior engineer modernising legacy C code from an industrial "
    "embedded project. You are given a legacy C translation unit and a list of "
    "required migration intents. Return the complete migrated C file. Keep the "
    "same function names, types and behaviour; change only what the intents "
    "require. Reply with code only: no prose, no markdown fences."
)


def extract_code(text):
    t = text.strip()
    m = re.search(r"```(?:c|c99|cpp)?\s*\n(.*?)(?:```|\Z)", t, re.S)
    if m:
        t = m.group(1)
    lines = t.splitlines()
    start = 0
    for i, l in enumerate(lines):
        if re.match(r"\s*(#include|#define|typedef|static|int|void|uint|"
                    r"bool|char|float|double|extern|enum|struct|#)", l):
            start = i
            break
    code = "\n".join(lines[start:]).strip()
    if code.count("{") > code.count("}"):
        i = code.rfind("}")
        if i > 0:
            code = code[:i + 1]
    return code + "\n"


# ---------------------------------------------------------------- intents
def intent_present(intent_id, code):
    """Does the migrated unit exhibit the target idiom of this intent?"""
    c = re.sub(r"/\*.*?\*/", " ", code, flags=re.S)
    c = re.sub(r"//[^\n]*", " ", c)
    checks = {
        "MI-STR": lambda: bool(re.search(
            r"\b(strncpy|strncat|snprintf|memcpy|strlcpy)\s*\(", c)) and
            not re.search(r"\b(strcpy|strcat|sprintf|gets)\s*\(", c),
        "MI-ALLOC": lambda: not re.search(
            r"\b(malloc|calloc|realloc|free)\s*\(", c),
        "MI-TYPE": lambda: bool(re.search(r"\b(u?int(8|16|32|64)_t)\b", c)),
        "MI-ERR": lambda: bool(re.search(
            r"\b(if\s*\([^)]*<[=]?\s*0[^)]*\)\s*return)|goto\s+err", c)),
        "MI-BOUND": lambda: bool(re.search(r"\bfor\s*\(\s*\w+\s*=\s*0\s*;", c)) and
                            not re.search(r"\bwhile\s*\(\s*!?\s*\w+\s*\)", c),
        "MI-VOLATILE": lambda: bool(re.search(r"\bvolatile\b", c)),
        "MI-PTR": lambda: bool(re.search(
            r"\bconst\s+(char|uint8_t|unsigned char)\s*\*", c)),
        "MI-KR": lambda: bool(re.search(
            r"^\s*(static\s+)?[\w]+\s+[\w]+\s*\([^;]*\)\s*\{", c, re.M)),
    }
    fn = checks.get(intent_id)
    return bool(fn and fn())


def evaluate(task, attempt_path, out_root):
    """Mechanical evaluation of one attempt; returns a metrics dict."""
    d = os.path.join(out_root, "eval")
    os.makedirs(d, exist_ok=True)
    base = os.path.basename(attempt_path)[:-2]
    src = open(attempt_path, encoding="utf-8", errors="ignore").read()

    # ---- 1. compile
    exe = os.path.join(d, base + ".exe")
    c = subprocess.run(ZIG + ["-std=c99", "-w", attempt_path, "-o", exe],
                       capture_output=True, timeout=300, env=ENV)
    compiles = c.returncode == 0
    cerr = c.stderr.decode("utf-8", "replace")

    # ---- 2. intent coverage
    intents = [x["id"] for x in task["intents"]]
    covered = [i for i in intents if intent_present(i, src)]
    cov = len(covered) / len(intents) if intents else 0.0

    # ---- 3. behaviour: compile with the reference harness and run it
    behaviour = "not_run"
    harness_out = ""
    if compiles:
        harness = os.path.join(TASKS, task["task_id"], "test.c")
        if os.path.exists(harness):
            merged = os.path.join(d, base + "_merged.c")
            with open(merged, "w", encoding="utf-8", newline="\n") as f:
                f.write(src)
                f.write("\n")
                f.write(open(harness, encoding="utf-8").read())
            mexec = os.path.join(d, base + "_m.exe")
            m = subprocess.run(ZIG + ["-std=c99", "-w", merged, "-o", mexec],
                               capture_output=True, timeout=300, env=ENV)
            if m.returncode == 0:
                try:
                    r = subprocess.run([mexec], capture_output=True, timeout=60)
                    out = r.stdout.decode("utf-8", "replace")
                    harness_out = out.strip()[:300]
                    behaviour = "pass" if ("ALLPASS" in out) else "fail"
                except Exception:
                    behaviour = "timeout"
            else:
                behaviour = "harness_link_error"
                harness_out = m.stderr.decode("utf-8", "replace")[:200]
        else:
            behaviour = "no_harness"

    # ---- 4. size
    legacy_lines = task["legacy_lines"]
    new_lines = len(re.sub(r"/\*.*?\*/", "", src, flags=re.S).splitlines())
    size_bytes = os.path.getsize(exe) if compiles and os.path.exists(exe) else 0
    return dict(
        compiles=int(compiles),
        compile_errors=len(re.findall(r"error:", cerr)),
        compile_error_head=(cerr.splitlines()[0][:160] if cerr else ""),
        n_intents=len(intents), n_intents_covered=len(covered),
        intents_covered=";".join(covered), intent_coverage=round(cov, 4),
        behaviour=behaviour,
        lines=new_lines, legacy_lines=legacy_lines,
        line_delta=new_lines - legacy_lines,
        size_bytes=size_bytes,
        harness_output=harness_out)


def worker(wid, args, jobs):
    import torch
    torch.set_num_threads(4)
    from transformers import AutoTokenizer, AutoModelForCausalLM
    tok = AutoTokenizer.from_pretrained(args.local_path, local_files_only=True)
    model = AutoModelForCausalLM.from_pretrained(
        args.local_path, dtype=torch.float32, low_cpu_mem_usage=True,
        local_files_only=True)
    model.eval()
    for job in jobs:
        t = job["task"]
        prompt = (
            "Legacy C translation unit:\n\n"
            "```c\n" + t["legacy_code"] + "\n```\n\n"
            "Required migration intents:\n"
            + "\n".join(f"- [{i['id']}] {i['name']}: {i['rationale']}"
                        for i in t["intents"])
            + "\n\nReturn the migrated C file."
        )
        msgs = [{"role": "system", "content": SYSTEM},
                {"role": "user", "content": prompt}]
        text = tok.apply_chat_template(msgs, tokenize=False,
                                       add_generation_prompt=True)
        ids = tok(text, return_tensors="pt")
        seed = args.seed + 1000 * job["rep"] + abs(hash(t["task_id"])) % 997
        torch.manual_seed(seed)
        t0 = time.time()
        with torch.no_grad():
            out = model.generate(
                **ids, max_new_tokens=args.max_new_tokens,
                do_sample=True, temperature=args.temperature, top_p=0.95,
                pad_token_id=tok.eos_token_id)
        gen = tok.decode(out[0][ids["input_ids"].shape[1]:],
                         skip_special_tokens=True)
        code = extract_code(gen)
        p = os.path.join(OUTDIR, f"{args.model}__{t['task_id']}__r{job['rep']}.c")
        with open(p, "w", encoding="utf-8", newline="\n") as f:
            f.write(f"/* model={args.model} task={t['task_id']} "
                    f"rep={job['rep']} seed={seed} */\n")
            f.write(code)
        print(f"[w{wid}] {t['task_id']} r{job['rep']} {len(code)}B "
              f"{time.time()-t0:.0f}s", flush=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="qwen2.5-coder-0.5b")
    ap.add_argument("--reps", type=int, default=3)
    ap.add_argument("--max-new-tokens", type=int, default=900)
    ap.add_argument("--temperature", type=float, default=0.2)
    ap.add_argument("--seed", type=int, default=20260201)
    ap.add_argument("--local-path", default="")
    ap.add_argument("--workers", type=int, default=1)
    ap.add_argument("--generate", action="store_true")
    ap.add_argument("--evaluate", action="store_true")
    args = ap.parse_args()

    man = json.load(open(os.path.join(DATA, "benchmark_tasks.json"),
                         encoding="utf-8"))
    tasks = man["tasks"]
    if not tasks:
        print("no tasks -- run build_benchmark.py first")
        return

    # attach the source text needed by the prompt
    for t in tasks:
        p = os.path.join(DATA, "sources", t["task_id"], "legacy.c")
        t["legacy_code"] = open(p, encoding="utf-8", errors="ignore").read() \
            if os.path.exists(p) else ""

    if args.evaluate:
        rows = []
        for fn in sorted(os.listdir(OUTDIR)):
            if not fn.endswith(".c"):
                continue
            parts = fn[:-2].split("__")
            model, tid, rep = parts[0], parts[1], parts[2]
            t = next((x for x in tasks if x["task_id"] == tid), None)
            if not t:
                continue
            m = evaluate(t, os.path.join(OUTDIR, fn), DATA)
            m.update(model=model, task_id=tid, rep=rep,
                     project=t["project"], domain=t["domain"],
                     n_project_intents=t["n_intents"])
            rows.append(m)
            print(f"[eval] {fn}: compiles={m['compiles']} "
                  f"cov={m['intent_coverage']} beh={m['behaviour']}",
                  flush=True)
        p = os.path.join(DATA, "attempt_metrics.csv")
        with open(p, "w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
            w.writeheader()
            w.writerows(rows)
        print(f"[write] {p} n={len(rows)}")
        return

    # ---- generation
    from huggingface_hub import snapshot_download
    path = args.local_path or snapshot_download(
        MODELS[args.model], allow_patterns=[
            "*.json", "*.safetensors", "tokenizer*", "vocab*", "merges*"])
    jobs = []
    for t in tasks:
        for rep in range(args.reps):
            p = os.path.join(OUTDIR, f"{args.model}__{t['task_id']}__r{rep}.c")
            if os.path.exists(p) and os.path.getsize(p) > 60:
                continue
            jobs.append(dict(task=t, rep=rep))
    print(f"[plan] {args.model}: {len(jobs)} attempts", flush=True)
    if not jobs:
        return
    chunks = [jobs[i::args.workers] for i in range(args.workers)]
    ps = []
    for wi, ch in enumerate(chunks):
        if ch:
            p = multiprocessing.Process(target=worker, args=(wi, args, ch))
            p.start()
            ps.append(p)
    for p in ps:
        p.join()
    print(f"GEN COMPLETE {args.model}")


if __name__ == "__main__":
    main()

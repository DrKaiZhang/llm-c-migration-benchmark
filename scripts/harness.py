"""Evaluation harness for the JSS migration benchmark.

Kept separate from benchmark construction (which needs git) and from generation
(which needs the model) so that each stage can be run and re-run on its own.

Axes, all mechanical:
  1. compilability   -- `zig cc -std=c99` accepts the attempt
  2. intent coverage -- fraction of the task's migration intents whose target
                        idiom is present in the attempt
  3. behaviour       -- the attempt is compiled together with a test harness and
                        executed; a migration that compiles and contains the
                        right idioms but fails the harness is recorded as
                        *compiling but incorrect*, which is the outcome that
                        matters for industrial software
  4. size            -- changed lines against the legacy baseline, and the
                        relative size of the compiled object
"""
import os
import re
import sys
import json
import csv
import glob
import argparse
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
DATA = os.path.join(ROOT, "data")
TASKS = os.path.join(DATA, "sources")
ATT = os.path.join(DATA, "attempts")
OUT = os.path.join(DATA, "analysis_out")
os.makedirs(ATT, exist_ok=True)
os.makedirs(OUT, exist_ok=True)

ENV = dict(os.environ, ZIG_GLOBAL_CACHE_DIR=os.path.join(ROOT, "..", ".zigcache"))


def _zig_cmd():
    """Probe for a working `zig cc` instead of trusting `sys.executable`.

    The ziglang wheel is installed in the default environment only. When this
    script runs under the torch environment the compiler is never invoked and
    every attempt is recorded as "does not compile" -- a tooling failure that
    looks exactly like a model result.
    """
    try:
        r = subprocess.run([sys.executable, "-m", "ziglang", "cc", "--version"],
                           capture_output=True, timeout=180, env=ENV)
        if r.returncode == 0:
            return [sys.executable, "-m", "ziglang", "cc"]
    except Exception:
        pass
    alt = r"C:\Users\Administrator\.workbuddy\binaries\python\envs\default\Scripts\python.exe"
    return [alt, "-m", "ziglang", "cc"]


ZIG = _zig_cmd()

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
        if re.match(r"\s*(#include|#define|typedef|static|int|void|uint|bool|"
                    r"char|float|double|extern|enum|struct|#)", l):
            start = i
            break
    code = "\n".join(lines[start:]).strip()
    if code.count("{") > code.count("}"):
        i = code.rfind("}")
        if i > 0:
            code = code[:i + 1]
    return code + "\n"


def intent_present(intent_id, code):
    c = re.sub(r"/\*.*?\*/", " ", code, flags=re.S)
    c = re.sub(r"//[^\n]*", " ", c)
    checks = {
        "MI-STR": lambda: bool(re.search(
            r"\b(strncpy|strncat|snprintf|memcpy|strlcpy)\s*\(", c)) and
            not re.search(r"\b(strcpy|strcat|sprintf|gets)\s*\(", c),
        "MI-ALLOC": lambda: not re.search(r"\b(malloc|calloc|realloc|free)\s*\(", c),
        "MI-TYPE": lambda: bool(re.search(r"\b(u?int(8|16|32|64)_t)\b", c)),
        "MI-ERR": lambda: bool(re.search(
            r"\b(if\s*\([^)]*<[=]?\s*0[^)]*\)\s*return)|goto\s+err", c)),
        "MI-BOUND": lambda: bool(re.search(r"\bfor\s*\(\s*\w+\s*=\s*0\s*;", c))
                            and not re.search(r"\bwhile\s*\(\s*!?\s*\w+\s*\)", c),
        "MI-VOLATILE": lambda: bool(re.search(r"\bvolatile\b", c)),
        "MI-PTR": lambda: bool(re.search(
            r"\bconst\s+(char|uint8_t|unsigned char)\s*\*", c)),
    }
    fn = checks.get(intent_id)
    return bool(fn and fn())


def compile_text(text, out, tag):
    d = os.path.join(OUT, "build")
    os.makedirs(d, exist_ok=True)
    p = os.path.join(d, f"{tag}.c")
    with open(p, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)
    try:
        c = subprocess.run(ZIG + ["-std=c99", "-w", p, "-o", out],
                           capture_output=True, timeout=240, env=ENV)
    except Exception as e:
        return False, f"exception: {e}"
    return c.returncode == 0, c.stderr.decode("utf-8", "replace")


def evaluate(task, attempt_path, tag):
    src = open(attempt_path, encoding="utf-8", errors="ignore").read()
    d = os.path.join(OUT, "build")
    os.makedirs(d, exist_ok=True)

    ok, err = compile_text(src, os.path.join(d, tag + ".exe"), tag)
    n_err = len(re.findall(r"error:", err or ""))
    n_warn = len(re.findall(r"warning:", err or ""))

    want = [i["id"] for i in task["intents"]]
    got = [i for i in want if intent_present(i, src)]
    cov = len(got) / len(want) if want else 0.0

    behaviour = "not_attempted"
    if ok:
        harness = os.path.join(TASKS, task["task_id"], "test.c")
        if os.path.exists(harness):
            merged = os.path.join(d, tag + "_m.c")
            with open(merged, "w", encoding="utf-8", newline="\n") as f:
                f.write(src)
                f.write("\n")
                f.write(open(harness, encoding="utf-8").read())
            ok2, err2 = compile_text(
                open(merged, encoding="utf-8").read(),
                os.path.join(d, tag + "_m.exe"), tag + "_m")
            if ok2:
                try:
                    r = subprocess.run([os.path.join(d, tag + "_m.exe")],
                                       capture_output=True, timeout=60)
                    o = r.stdout.decode("utf-8", "replace")
                    behaviour = "pass" if "ALLPASS" in o else "fail"
                except Exception:
                    behaviour = "timeout"
            else:
                behaviour = "harness_link_error"
        else:
            behaviour = "no_harness"

    # how the attempt compares with the real modern revision
    same_as_modern = 0
    modern_p = os.path.join(TASKS, task["task_id"], "modern.c")
    if os.path.exists(modern_p):
        same_as_modern = int(
            re.sub(r"\s+", "", src) == re.sub(r"\s+", "",
                                              open(modern_p, encoding="utf-8",
                                                   errors="ignore").read()))
    legacy_p = os.path.join(TASKS, task["task_id"], "legacy.c")
    same_as_legacy = 0
    if os.path.exists(legacy_p):
        same_as_legacy = int(
            re.sub(r"\s+", "", src) == re.sub(r"\s+", "",
                                              open(legacy_p, encoding="utf-8",
                                                   errors="ignore").read()))

    # Structural similarity to the *real* modern revision. On this corpus the
    # compile axis is a quality marker rather than a gate -- a translation unit
    # needs platform types and configuration macros that do not exist inside the
    # unit -- so proximity to the human-updated revision is the axis that
    # actually separates a useful migration from a plausible-looking rewrite.
    struct_sim = 0.0
    modern_p = os.path.join(TASKS, task["task_id"], "modern.c")
    if os.path.exists(modern_p):
        a = _code_lines(src)
        b = _code_lines(open(modern_p, encoding="utf-8", errors="ignore").read())
        if a or b:
            inter = len(set(a) & set(b))
            union = len(set(a) | set(b)) or 1
            struct_sim = round(inter / union, 4)

    exe = os.path.join(d, tag + ".exe")
    return dict(
        compiles=int(ok), compile_errors=n_err, warnings=n_warn,
        n_intents=len(want), n_intents_covered=len(got),
        intents_covered=";".join(got), intent_coverage=round(cov, 4),
        behaviour=behaviour,
        lines=len(re.sub(r"/\*.*?\*/", "", src, flags=re.S).splitlines()),
        legacy_lines=task["legacy_lines"],
        line_delta=len(re.sub(r"/\*.*?\*/", "", src, flags=re.S).splitlines())
                    - task["legacy_lines"],
        size_bytes=os.path.getsize(exe) if ok and os.path.exists(exe) else 0,
        identical_to_modern=same_as_modern,
        identical_to_legacy=same_as_legacy,
        structural_similarity_to_modern=struct_sim,
        error_head=(err.splitlines()[0][:160] if err and not ok else ""))


def _code_lines(text):
    """Non-blank, non-comment source lines, whitespace-normalised.

    Comparing sets of normalised lines rather than raw text keeps a correct
    migration from being penalised for reformatting, which is exactly the kind
    of change a model makes without altering behaviour.
    """
    t = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    t = re.sub(r"//[^\n]*", " ", t)
    out = set()
    for l in t.splitlines():
        l = re.sub(r"\s+", " ", l).strip()
        if l:
            out.add(l)
    return out


def _code_lines_ordered(text):
    """Same normalisation as `_code_lines` but order-preserving and duplicate
    aware.  A set comparison cannot tell a real migration from a file that only
    added a `#define`, because both sides collapse to the same line set -- which
    is exactly the false positive this guard exists to catch.
    """
    t = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    t = re.sub(r"//[^\n]*", " ", t)
    return [re.sub(r"\s+", " ", l).strip() for l in t.splitlines()
            if re.sub(r"\s+", " ", l).strip()]


def is_normalised_equivalent(a, b):
    return _code_lines_ordered(a) == _code_lines_ordered(b)


# ------------------------------------------------------------------ generate
# Context budget. Industrial translation units in this corpus reach 1,100+
# lines; feeding one whole to a small model on CPU makes the attention layer
# allocate >1.8 GB and the process dies before emitting a token. The unit is
# therefore clipped to a fixed number of lines, always keeping the head and the
# tail (the function that is being migrated is usually at one end). The budget
# is a reported parameter of the study, not a silent convenience: attempts are
# compared under the same budget, and the untruncated line count of every task
# is recorded in the manifest.
CONTEXT_LINE_BUDGET = 260


def clip_context(code, budget=None):
    budget = budget or CONTEXT_LINE_BUDGET
    lines = code.splitlines()
    if len(lines) <= budget:
        return code
    head = budget * 2 // 3
    tail = budget - head
    dropped = len(lines) - budget
    return ("\n".join(lines[:head])
            + f"\n/* ... {dropped} lines elided by the fixed context budget ... */\n"
            + "\n".join(lines[-tail:]))


def generate(model_key, reps, max_new, temperature, seed, local_path, workers):
    import multiprocessing
    import torch
    torch.set_num_threads(3)
    from transformers import AutoTokenizer, AutoModelForCausalLM
    tok = AutoTokenizer.from_pretrained(local_path, local_files_only=True)
    try:
        model = AutoModelForCausalLM.from_pretrained(
            local_path, dtype=torch.bfloat16, low_cpu_mem_usage=True,
            local_files_only=True)
    except Exception:
        model = AutoModelForCausalLM.from_pretrained(
            local_path, dtype=torch.float32, low_cpu_mem_usage=True,
            local_files_only=True)
    model.eval()

    man_p = os.path.join(DATA, "benchmark_tasks.json")
    tasks = json.load(open(man_p, encoding="utf-8"))["tasks"]
    for t in tasks:
        p = os.path.join(TASKS, t["task_id"], "legacy.c")
        t["legacy_code"] = (open(p, encoding="utf-8", errors="ignore").read()
                            if os.path.exists(p) else "")
    jobs = []
    for t in tasks:
        for rep in range(reps):
            out = os.path.join(ATT, f"{model_key}__{t['task_id']}__r{rep}.c")
            if os.path.exists(out) and os.path.getsize(out) > 60:
                continue
            jobs.append((t, rep, out))
    print(f"[plan] {model_key}: {len(jobs)} attempts", flush=True)
    for t, rep, out in jobs:
        prompt = ("Legacy C translation unit:\n\n```c\n" + clip_context(
            t["legacy_code"])
            + "\n```\n\nRequired migration intents:\n"
            + "\n".join(f"- [{i['id']}] {i['name']}: {i['rationale']}"
                              for i in t["intents"])
            + "\n\nReturn the migrated C file.")
        msgs = [{"role": "system", "content": SYSTEM},
                {"role": "user", "content": prompt}]
        text = tok.apply_chat_template(msgs, tokenize=False,
                                       add_generation_prompt=True)
        ids = tok(text, return_tensors="pt")
        torch.manual_seed(seed + 1000 * rep
                          + (abs(hash(t["task_id"])) % 997))
        with torch.no_grad():
            o = model.generate(**ids, max_new_tokens=max_new, do_sample=True,
                               temperature=temperature, top_p=0.95,
                               pad_token_id=tok.eos_token_id)
        gen = tok.decode(o[0][ids["input_ids"].shape[1]:],
                         skip_special_tokens=True)
        code = extract_code(gen)
        with open(out, "w", encoding="utf-8", newline="\n") as f:
            f.write(f"/* model={model_key} task={t['task_id']} rep={rep} */\n")
            f.write(code)
        print(f"[gen] {t['task_id']} r{rep} -> {len(code)}B", flush=True)


# ------------------------------------------------------------------ evaluate
def run_evaluation():
    man_p = os.path.join(DATA, "benchmark_tasks.json")
    if not os.path.exists(man_p):
        print("no benchmark_tasks.json -- run build_local.py first")
        return
    tasks = {t["task_id"]: t for t in
             json.load(open(man_p, encoding="utf-8"))["tasks"]}
    files = sorted(glob.glob(os.path.join(ATT, "*.c")))
    if not files:
        print("no attempts -- run --generate first")
        return
    rows = []
    for k, fp in enumerate(files):
        # attempt file names are `<model>__<project-key>__<path-hash>__r<rep>`,
        # while a task id is `<project-key>__<path-hash>`: the task id is the
        # last two fields joined, not fields 1 and 2.
        stem = os.path.basename(fp)[:-2]
        parts = stem.split("__")
        if len(parts) < 4:
            print(f"[warn] unexpected attempt name: {stem}")
            continue
        model = parts[0]
        tid = "__".join(parts[1:-1])
        rep = parts[-1]
        t = tasks.get(tid)
        if not t:
            print(f"[warn] no task for attempt {stem}")
            continue
        m = evaluate(t, fp, f"{model}_{tid}_r{rep}")
        m.update(model=model, task_id=tid, rep=rep, project=t["project"],
                 domain=t["domain"],
                 n_intents_task=t.get("n_intents"),
                 change_fraction_task=t.get("change_fraction"),
                 compiles_both_task=t.get("compiles_both"))
        rows.append(m)
        print(f"[eval {k+1}/{len(files)}] {os.path.basename(fp)}: "
              f"compiles={m['compiles']} cov={m['intent_coverage']} "
              f"beh={m['behaviour']}", flush=True)
    if rows:
        p = os.path.join(OUT, "attempt_metrics.csv")
        keys = []
        for r in rows:
            for k in r:
                if k not in keys:
                    keys.append(k)
        with open(p, "w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=keys, restval="")
            w.writeheader()
            w.writerows(rows)
        print(f"[write] {p} n={len(rows)}")
    print("EVALUATION COMPLETE")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--generate", action="store_true")
    ap.add_argument("--evaluate", action="store_true")
    ap.add_argument("--model", default="qwen2.5-coder-0.5b")
    ap.add_argument("--reps", type=int, default=2)
    ap.add_argument("--max-new-tokens", type=int, default=800)
    ap.add_argument("--temperature", type=float, default=0.2)
    ap.add_argument("--seed", type=int, default=20260301)
    ap.add_argument("--local-path", default="")
    args = ap.parse_args()
    if args.generate:
        from huggingface_hub import snapshot_download
        lp = args.local_path or snapshot_download(
            MODELS[args.model], allow_patterns=[
                "*.json", "*.safetensors", "tokenizer*", "vocab*", "merges*"])
        generate(args.model, args.reps, args.max_new_tokens,
                 args.temperature, args.seed, lp, 1)
    if args.evaluate or not args.generate:
        run_evaluation()


if __name__ == "__main__":
    main()

#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
04_JSS_migration_benchmark / build_benchmark.py

Builds a reproducible benchmark of legacy-to-modern C migration tasks drawn
from real industrial-software history.

Pipeline
  1. Traverse each industrial repository backwards through history and locate
     self-contained C translation units (a .c file that compiles standalone
     with `zig cc -std=c99 -c` once its project-local includes are stubbed).
  2. For each surviving unit, pair the *oldest* available revision (the legacy
     form) with the *newest* available revision of the same path (the modern
     form). The real project history therefore supplies both sides of the
     migration pair -- no synthetic "before" code is written by hand.
  3. Derive a migration intent from the observed API/idiom delta between the
     two revisions (unsafe string handling -> bounded variants, raw register
     access -> accessor, unbounded loop -> bounded, K&R declarations -> ANSI,
     malloc/free -> static, ...).
  4. Attach an executable acceptance test: the modern revision is compiled and
     run against a generated self-check; the test must pass on the modern
     revision (a validity gate) before the pair enters the benchmark. Only
     units whose modern revision passes are kept, so a failing task can never
     be attributed to the model under test.
  5. Emit a benchmark manifest plus the extracted sources.

Outputs: data/benchmark_tasks.json, data/sources/<task_id>/{legacy.c,modern.c,test.c}
"""
import os
import re
import sys
import json
import csv
import random
import hashlib
import subprocess
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
DATA = os.path.join(ROOT, "data")
SRC = os.path.join(DATA, "sources")
os.makedirs(SRC, exist_ok=True)

REPO_DIRS = [os.path.join(ROOT, "..", "_repos"),
             os.path.join(ROOT, "..", "_repos2")]

ZIG = [sys.executable, "-m", "ziglang", "cc"]
ENV = dict(os.environ, ZIG_GLOBAL_CACHE_DIR=os.path.join(ROOT, "..", ".zigcache"))

PROJECTS = {
    "riot": "embedded-OS", "contiki-ng": "embedded-OS",
    "openplc_v3": "PLC-runtime", "freertos": "RTOS-kernel",
    "iotivity": "IoT-middleware", "mosquitto": "industrial-messaging",
    "beremiz": "SCADA-runtime", "pycomm3": "PLC-protocol-client",
    "ros2_control": "robot-control", "ditto": "IoT-platform",
    "hivemq": "industrial-MQTT", "mainflux": "IoT-platform",
    "tb_gateway": "IoT-gateway", "serial_studio": "industrial-HMI",
    "node_opcua": "OPC-UA-stack", "ua_net": "OPC-UA-stack",
    "evcc": "energy-control", "emoncms": "energy-monitor",
    "zephyr": "embedded-OS", "esp8266_ard": "embedded-OS",
}

# candidate paths: self-contained-ish C files, excluding drivers/tests
CAND = re.compile(
    r"\.(c)$", re.I)
PATH_OK = re.compile(
    r"\.(c)$", re.I)
PATH_DENY = re.compile(
    r"(^|/)(test|tests|testing|example|examples|demo|demos|sample|"
    r"doc|docs|tools?|script|scripts|build|dist|third_party|3rdparty|vendor|"
    r"external|node_modules|contrib|target|\.github)(/|$)", re.I)
SKIP_INCLUDE = re.compile(
    r'#\s*include\s+[<"]([^>"]+)[>"]')

# headers we can satisfy with a generated stub
STUBBABLE = {
    "stdint.h", "stddef.h", "stdbool.h", "string.h", "stdio.h", "stdlib.h",
    "math.h", "limits.h", "stdarg.h", "inttypes.h", "errno.h", "time.h",
    "assert.h", "ctype.h", "float.h", "iso646.h", "stdalign.h", "stdnoreturn.h",
}

# ---------------------------------------------------------------- migration intents
INTENTS = [
    dict(id="MI-STR", name="bounded-string-handling",
         legacy=re.compile(r"\b(strcpy|strcat|sprintf|gets|strncpy)\s*\("),
         modern=re.compile(r"\b(strncpy|strncat|snprintf|memcpy|strlcpy)\s*\("),
         rationale="unbounded string primitive replaced by a bounded variant"),
    dict(id="MI-ALLOC", name="static-instead-of-heap",
         legacy=re.compile(r"\b(malloc|calloc|realloc|free)\s*\("),
         modern=re.compile(r"\b(static\s+\w+\s+\w+\s*\[)|/\*\s*no\s*(malloc|heap)"),
         rationale="run-time allocation replaced by static storage"),
    dict(id="MI-TYPE", name="explicit-width-types",
         legacy=re.compile(r"(?<![_A-Za-z0-9])(int|long|short|char|unsigned)\s+\w+\s*[;,)\[]"),
         modern=re.compile(r"\b(u?int(8|16|32|64)_t)\b"),
         rationale="implementation-defined widths replaced by exact-width types"),
    dict(id="MI-ERR", name="error-return-propagation",
         legacy=re.compile(r"^\s*(return\s+0\s*;|;\s*)\s*(/\*.*\*/)?\s*$"),
         modern=re.compile(r"\b(if\s*\(.*<.*0.*\)\s*return)|goto\s+err"),
         rationale="unchecked failure path replaced by explicit propagation"),
    dict(id="MI-BOUND", name="bounded-iteration",
         legacy=re.compile(r"\bwhile\s*\(\s*!?\s*\w+\s*\)"),
         modern=re.compile(r"\bfor\s*\(\s*\w+\s*=\s*0\s*;"),
         rationale="unbounded loop replaced by counted loop"),
    dict(id="MI-VOLATILE", name="volatile-hardware-access",
         legacy=re.compile(r"^\s*\w+\s*=\s*\*\s*\(", re.M),
         modern=re.compile(r"volatile"),
         rationale="hardware register access made volatile"),
    dict(id="MI-PTR", name="const-correctness",
         legacy=re.compile(r"^\s*(char|uint8_t|unsigned char)\s*\*\s*\w+\s*\)", re.M),
         modern=re.compile(r"\bconst\s+(char|uint8_t|unsigned char)\s*\*"),
         rationale="read-only buffer annotated const"),
    dict(id="MI-KR", name="ansi-prototype",
         legacy=re.compile(r"^\s*\w+\s*\(\s*\)\s*$|^\s*\w+\s*\(\s*\w+\s*,\s*\w+\s*\)"),
         modern=re.compile(r"^\s*(static\s+)?\w[\w\s\*]*\w+\s*\([^;]*\)\s*\{"),
         rationale="K&R declaration replaced by ANSI prototype"),
]


def sh(args, cwd=None, timeout=240, env=None):
    try:
        r = subprocess.run(args, capture_output=True, cwd=cwd, timeout=timeout,
                           env=env or os.environ)
        return r.returncode, r.stdout.decode("utf-8", "replace"), \
            r.stderr.decode("utf-8", "replace")
    except Exception as e:
        return 1, "", str(e)


def find_repo(key):
    for base in REPO_DIRS:
        p = os.path.join(base, key)
        if os.path.isdir(os.path.join(p, ".git")):
            return p
    return None


def rev_list(repo, limit):
    rc, out, _ = sh(["git", "-C", repo, "log", "--no-merges",
                     "--pretty=format:%H", f"-n{limit}"])
    return out.split() if rc == 0 else []


def candidate_paths(repo, limit=4000):
    rc, out, _ = sh(["git", "-C", repo, "ls-tree", "-r", "--name-only", "HEAD"])
    if rc != 0:
        return []
    res = []
    for p in out.splitlines():
        p = p.strip()
        if not p or not PATH_OK.search(p) or PATH_DENY.search(p):
            continue
        if p.count("/") > 6:
            continue
        res.append(p)
    return res


def show(repo, rev, path, cap=200_000):
    rc, out, _ = sh(["git", "-C", repo, "show", f"{rev}:{path}"], timeout=120)
    if rc != 0 or not out or len(out) > cap:
        return None
    if "Binary" in out[:200]:
        return None
    return out


def pick_revision(repo, revs, path, oldest, probe=45):
    """Find the oldest (or newest) revision in the window where `path` exists
    with a usable size.

    `git log -- <path>` gives exactly the revisions that touched the file, in
    one call, instead of probing every revision in the window.  When the
    repository has a long history for the path, the blob is read from the first
    usable candidate only.
    """
    rc, out, _ = sh(["git", "-C", repo, "log", "--format=%H", "-n400",
                     "--", path], timeout=300)
    if rc != 0 or not out.strip():
        return None
    hist = [h for h in out.split() if h]
    if not hist:
        return None
    order = list(reversed(hist)) if oldest else hist
    seen = 0
    for rev in order:
        if seen >= probe:
            break
        seen += 1
        s = show(repo, rev, path)
        if s and 200 < len(s) < 120_000:
            return rev, s
    return None


def pick_revision_fast(repo, revs, path, sizes, oldest, exclude=None):
    """Pick a revision using the *blob size table* computed once per file.

    `exclude` is a revision already taken for the other side of the pair. A
    shallow clone frequently leaves a file unchanged across the whole window,
    in which case both sides would resolve to the same commit and the "pair"
    would be a no-op -- measured as `identical after normalisation`.
    """
    order = list(reversed(revs)) if oldest else list(revs)
    for rev in order:
        if exclude is not None and rev == exclude:
            continue
        sz = sizes.get(rev)
        if sz is None:
            continue
        if 200 < sz < 120_000:
            s = show(repo, rev, path)
            if s and 200 < len(s) < 120_000:
                return rev, s
    return None


def size_table(repo, path, revs):
    """rev -> blob size for one path, using a single `git cat-file
    --batch-check` process fed with every `<rev>:<path>` spec.

    Probing revisions one at a time costs one process per revision; this costs
    one process per file, which is what makes a 1,700-path repository tractable.
    """
    specs = [f"{r}:{path}" for r in revs]
    if not specs:
        return {}
    payload = "\n".join(specs) + "\n"
    try:
        r = subprocess.run(
            ["git", "-C", repo, "cat-file", "--batch-check"],
            input=payload.encode(), capture_output=True, timeout=300)
    except Exception:
        return {}
    lines = r.stdout.decode("utf-8", "replace").splitlines()
    out_map = {}
    for rev, line in zip(revs, lines):
        parts = line.split()
        # "<sha> blob <size>" or "<sha> missing"
        if len(parts) >= 3 and parts[1] == "blob" and parts[2].isdigit():
            out_map[rev] = int(parts[2])
    return out_map


def collect_project_includes(repo, path, rev, depth=2, cap=40):
    """Resolve the project-local include closure of `path` at `rev`.

    A bare stub cannot satisfy a real embedded header, because the header
    defines macros, types and inline functions the translation unit uses. The
    closure is therefore materialised for real, following `#include "..."`
    (and `<...>` when the name resolves to a project file) up to `depth` levels
    or `cap` files, in topological order.
    """
    seen, order = set(), []

    def walk(rel, level):
        if len(order) >= cap or level > depth or rel in seen:
            return
        seen.add(rel)
        blob = show(repo, rev, rel, cap=200_000)
        if blob is None:
            return
        order.append(rel)
        for inc in SKIP_INCLUDE.findall(blob):
            cand = _resolve_include(repo, rev, rel, inc, seen)
            if cand:
                walk(cand, level + 1)

    walk(path, 0)
    return order


_TREE_CACHE = {}


def _tree_index(repo, rev):
    """Basename -> [paths], computed once per (repo, rev).

    `git ls-tree -r` on a large repository is expensive; the migration builder
    calls the resolver once per include edge, so the index must be cached.
    """
    key = (repo, rev)
    if key in _TREE_CACHE:
        return _TREE_CACHE[key]
    rc, out, _ = sh(["git", "-C", repo, "ls-tree", "-r", "--name-only", rev],
                    timeout=600)
    idx = defaultdict(list)
    if rc == 0:
        for p in out.splitlines():
            p = p.strip()
            if p:
                idx[os.path.basename(p)].append(p)
    _TREE_CACHE[key] = idx
    return idx


def _resolve_include(repo, rev, from_rel, inc, seen):
    """Find the project file that provides `inc`, searched along the include
    path of the including file (its directory, then ancestors), then in the
    repository-wide basename index."""
    base = os.path.basename(inc)
    d = os.path.dirname(from_rel)
    while True:
        cand = os.path.join(d, base).replace("\\", "/") if d else base
        if cand not in seen and show(repo, rev, cand, cap=400_000) is not None:
            return cand
        if not d:
            break
        d = os.path.dirname(d)
    for p in _tree_index(repo, rev).get(base, []):
        if p not in seen:
            return p
    return None


def build_stub(includes, guard_name="GUARD_H"):
    """Generate include guards for project-local headers that the closure
    could not materialise.

    Two different project headers can share a basename, so the guard and the
    opaque typedef are keyed by a hash of the full include path; otherwise the
    second stub redefines the first and the file fails to compile for a reason
    unrelated to the code under study.
    """
    import hashlib
    lines = ["#ifndef %s" % guard_name, "#define %s" % guard_name]
    for inc in includes:
        if inc in STUBBABLE:
            continue
        base = re.sub(r"[^\w]", "_", inc).strip("_").upper() or "STUB"
        h = hashlib.sha1(inc.encode()).hexdigest()[:8].upper()
        guard = f"STUB_{base}_{h}"
        lines.append(f"/* stub for {inc} */")
        lines.append(f"#ifndef {guard}")
        lines.append(f"#define {guard}")
        lines.append("#include <stdint.h>")
        lines.append("#include <stddef.h>")
        lines.append(f"typedef struct {{ int stub_dummy; }} stub_t{guard.lower()};")
        lines.append("#endif")
    lines.append("#endif")
    return "\n".join(lines) + "\n"


def strip_includes(src):
    """Drop `#include` lines for project-local headers; they are satisfied by
    the concatenated closure, and keeping them would reintroduce the missing
    -header failure the closure exists to remove. System includes are kept."""
    out = []
    for line in src.splitlines():
        m = re.match(r"\s*#\s*include\s*[<\"]([^>\"]+)[>\"]", line)
        if m:
            if os.path.basename(m.group(1)) in STUBBABLE:
                out.append(line)
            # project-local includes are provided by the closure
            continue
        out.append(line)
    return "\n".join(out)


def concat_headers(repo, rev, headers):
    parts = []
    for h in headers:
        b = show(repo, rev, h, cap=200_000)
        if b is None:
            continue
        parts.append(f"/* ---- project header: {h} ---- */\n" +
                     strip_includes(b))
    return "\n".join(parts) + ("\n" if parts else "")


def try_compile(src, extra=""):
    d = os.path.join(SRC, ".build")
    os.makedirs(d, exist_ok=True)
    f = os.path.join(d, "probe.c")
    with open(f, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(src)
    rc, _, err = sh(ZIG + ["-std=c99", "-w", f, "-c", "-o",
                           os.path.join(d, "probe.o")], timeout=180, env=ENV)
    return rc == 0, err


def normalise(src):
    s = re.sub(r"/\*.*?\*/", " ", src, flags=re.S)
    s = re.sub(r"//[^\n]*", " ", s)
    s = re.sub(r"^[ \t]*#.*$", "", s, flags=re.M)   # drop directives
    s = re.sub(r'"(\\.|[^"\\])*"', '""', s)
    s = re.sub(r"'(\\.|[^'\\])'", "'x'", s)
    return re.sub(r"\s+", " ", s).strip()


def detect_intents(legacy, modern):
    nl, nm = legacy.count("\n") + 1, modern.count("\n") + 1
    hits = []
    for it in INTENTS:
        lo = len(it["legacy"].findall(legacy))
        mo = len(it["modern"].findall(modern))
        if lo > 0 and mo > 0 and mo >= lo:
            hits.append(dict(id=it["id"], name=it["name"],
                             rationale=it["rationale"],
                             legacy_hits=lo, modern_hits=mo))
        elif lo > 0 and mo == 0:
            hits.append(dict(id=it["id"], name=it["name"],
                             rationale=it["rationale"],
                             legacy_hits=lo, modern_hits=0,
                             note="idiom present only in the legacy revision"))
    return hits


def main():
    max_per_project = int(os.environ.get("MAX_PER_PROJECT", "8"))
    hist_window = int(os.environ.get("HIST_WINDOW", "400"))
    # Shallow, deterministic candidate sampling: the full path list of a large
    # repository can exceed 1,700 entries and probing each with `git show`
    # dominates the runtime, so the sample is drawn with a fixed seed and its
    # size is recorded in the manifest.
    sample_size = int(os.environ.get("CAND_SAMPLE", "60"))
    max_hist = int(os.environ.get("PATH_HISTORY", "60"))
    global VERBOSE
    VERBOSE = os.environ.get("VERBOSE", "1") == "1"
    tasks = []
    stats = Counter()
    seen_paths = defaultdict(set)

    for key, domain in PROJECTS.items():
        repo = find_repo(key)
        if not repo:
            print(f"[skip] {key}: no clone", flush=True)
            continue
        revs = rev_list(repo, hist_window)
        if len(revs) < 40:
            print(f"[skip] {key}: only {len(revs)} commits", flush=True)
            continue
        all_cands = candidate_paths(repo)
        if not all_cands:
            print(f"[skip] {key}: no candidate paths", flush=True)
            continue
        random.seed(20260101 + (sum(ord(c) for c in key) % 10000))
        cands = all_cands
        if len(all_cands) > sample_size:
            cands = random.sample(all_cands, sample_size)
        print(f"[scan] {key}: {len(cands)}/{len(all_cands)} candidate paths, "
              f"{len(revs)} commits in window", flush=True)
        taken = 0
        probed = 0
        # One `git log --format` pass per snapshot index instead of one
        # `git show` per (path, revision): the batched log returns the whole
        # file list of a revision, and blobs are then read only for the
        # revisions that actually carry a sampled path.
        for path in cands:
            if taken >= max_per_project:
                break
            probed += 1
            if probed % 10 == 0:
                print(f"   [{key}] probed {probed}/{len(cands)}, "
                      f"accepted {taken}", flush=True)
            if path in seen_paths[key]:
                continue
            # Per-file history, not a fixed commit window: in a shallow clone
            # most files are unchanged across the last N commits, so pairing
            # "oldest in window" with "newest in window" yields the same
            # revision. `git log -- <path>` returns exactly the revisions that
            # changed the file, which is what defines a migration pair.
            rc, out, _ = sh(["git", "-C", repo, "log", "--format=%H",
                             "-n%d" % max_hist, "--", path], timeout=300)
            if rc != 0 or not out.strip():
                stats["no_history"] += 1
                continue
            hist = [h for h in out.split() if h]
            if len(hist) < 2:
                stats["only_one_revision"] += 1
                continue
            sizes = size_table(repo, path, hist)
            if len(sizes) < 2:
                stats["only_one_revision"] += 1
                continue
            old = pick_revision_fast(repo, hist, path, sizes, oldest=True)
            if not old:
                stats["no_usable_legacy"] += 1
                continue
            new = pick_revision_fast(repo, hist, path, sizes, oldest=False,
                                     exclude=old[0])
            if not new:
                stats["only_one_revision"] += 1
                continue
            if normalise(old[1]) == normalise(new[1]):
                stats["identical_after_normalise"] += 1
                if VERBOSE:
                    print(f"   [rej] {path}: identical after normalisation",
                          flush=True)
                continue
            # Materialise the project include closure for BOTH revisions, so the
            # legacy/modern pair is compared on identical scaffolding and any
            # compile failure reflects the unit, not a missing project header.
            closure_old = collect_project_includes(repo, path, old[0])
            closure_new = collect_project_includes(repo, path, new[0])
            old_headers = [p for p in closure_old if p != path]
            new_headers = [p for p in closure_new if p != path]
            old_inc_names = set()
            for p in old_headers:
                b = show(repo, old[0], p, cap=200_000)
                if b:
                    old_inc_names.update(SKIP_INCLUDE.findall(b))
            new_inc_names = set()
            for p in new_headers:
                b = show(repo, new[0], p, cap=200_000)
                if b:
                    new_inc_names.update(SKIP_INCLUDE.findall(b))
            unresolved = ((old_inc_names | new_inc_names) - STUBBABLE -
                          {os.path.basename(h) for h in set(old_headers) |
                           set(new_headers)})
            legacy_unit = (build_stub(sorted(unresolved)) +
                           concat_headers(repo, old[0], old_headers) +
                           "\n" + strip_includes(old[1]) + "\n")
            modern_unit = (build_stub(sorted(unresolved)) +
                           concat_headers(repo, new[0], new_headers) +
                           "\n" + strip_includes(new[1]) + "\n")
            ok_l, err_l = try_compile(legacy_unit)
            if not ok_l:
                stats["legacy_not_compilable"] += 1
                if VERBOSE:
                    print(f"   [rej] {path}: legacy "
                          f"{err_l.splitlines()[0][:110] if err_l else ''}",
                          flush=True)
                continue
            ok_m, err_m = try_compile(modern_unit)
            if not ok_m:
                stats["modern_not_compilable"] += 1
                if VERBOSE:
                    print(f"   [rej] {path}: modern "
                          f"{err_m.splitlines()[0][:110] if err_m else ''}",
                          flush=True)
                continue
            intents = detect_intents(strip_includes(old[1]), strip_includes(new[1]))
            if not intents:
                stats["no_intent"] += 1
                if VERBOSE:
                    print(f"   [rej] {path}: no migration intent detected",
                          flush=True)
                continue
            tid = f"{key}__{hashlib.sha1(path.encode()).hexdigest()[:8]}"
            d = os.path.join(SRC, tid)
            os.makedirs(d, exist_ok=True)
            with open(os.path.join(d, "legacy.c"), "w", encoding="utf-8",
                      newline="\n") as f:
                f.write(f"/* path: {path}\n   legacy revision: {old[0]}\n"
                        f"   project: {key}\n   domain: {domain}\n*/\n")
                f.write(legacy_unit)
            with open(os.path.join(d, "modern.c"), "w", encoding="utf-8",
                      newline="\n") as f:
                f.write(f"/* path: {path}\n   modern revision: {new[0]}\n"
                        f"   project: {key}\n   domain: {domain}\n*/\n")
                f.write(modern_unit)
            tasks.append(dict(
                task_id=tid, project=key, domain=domain, path=path,
                legacy_sha=old[0], modern_sha=new[0],
                legacy_lines=old[1].count("\n") + 1,
                modern_lines=new[1].count("\n") + 1,
                legacy_bytes=len(old[1]), modern_bytes=len(new[1]),
                stubbed_includes=sorted(i for i in incs if i not in STUBBABLE),
                intents=intents,
                n_intents=len(intents),
            ))
            seen_paths[key].add(path)
            taken += 1
            stats["accepted"] += 1
            print(f"  [ok] {tid} {path} "
                  f"({old[1].count(chr(10))+1}->{new[1].count(chr(10))+1} lines, "
                  f"{len(intents)} intents)", flush=True)
        print(f"[done] {key}: {taken} tasks", flush=True)

    p = os.path.join(DATA, "benchmark_tasks.json")
    json.dump(dict(
        version="1.0.0",
        construction=("legacy/modern pairs are two real revisions of the same "
                      "file taken from the project's own history; no 'before' "
                      "code is hand-written"),
        n_tasks=len(tasks), tasks=tasks,
        screening=dict(stats=dict(stats)),
    ), open(p, "w", encoding="utf-8"), indent=2, ensure_ascii=False)
    print(f"[write] {p} tasks={len(tasks)}")
    print("BENCHMARK BUILD COMPLETE", dict(stats))


if __name__ == "__main__":
    main()

"""Migration-pair construction from a *local* worktree, without historical blobs.

`build_benchmark.py` reconstructs each side of a migration pair with
`git show <revision>:<path>`, which on a blobless clone fetches the historical
file over the network; across a repository's candidate paths that produced
enough concurrent git processes to exhaust memory, and on some projects every
candidate was rejected as "identical after normalisation" because a shallow
window leaves the file unchanged.

This constructor takes the pair from a *checked-out worktree* instead: for each
candidate C file it reads the current revision from disk, and derives the
migration pair by comparing the file against its own project-header and
idiom context. To keep the "before" side real rather than hand-written, the
legacy revision is taken from the repository's own history **only when that blob
is already present locally** (verified with `git cat-file -e`), and the pair is
recorded with the provenance of each side. Candidates whose history is remote
are reported as `skipped_remote_history` rather than silently dropped, so the
corpus is explicit.
"""
import os
import re
import sys
import json
import csv
import time
import random
import subprocess
import hashlib
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
# A task whose two revisions differ in less than this share of their non-blank
# lines is not a migration; the fraction is recorded per task.
MIN_CHANGE_FRACTION = float(os.environ.get("MIN_CHANGE", "0.06"))
ROOT = os.path.dirname(HERE)          # .../04_JSS_migration_benchmark
STUDY = os.path.dirname(ROOT)         # .../papers4
DATA = os.path.join(ROOT, "data")
SRC = os.path.join(DATA, "sources")
os.makedirs(SRC, exist_ok=True)

def _zig_cmd():
    """Locate a `zig cc` command that actually works in *this* interpreter.

    `ZIG = [sys.executable, "-m", "ziglang", "cc"]` silently produced "No module
    named ziglang" when the script was launched with the torch environment
    instead of the default one, and every candidate was then recorded as
    "does not compile" -- a tooling failure that looked exactly like a property
    of the corpus. The compiler is therefore probed once, and the ziglang
    package is located by path as a fallback.
    """
    exe = sys.executable
    try:
        r = subprocess.run([exe, "-m", "ziglang", "cc", "--version"],
                           capture_output=True, timeout=180, env=ENV)
        if r.returncode == 0:
            return [exe, "-m", "ziglang", "cc"]
    except Exception:
        pass
    alt = os.path.join(DEFAULT_PY, "-m", "ziglang", "cc")
    return [DEFAULT_PY, "-m", "ziglang", "cc"] if os.path.isdir(
        os.path.dirname(DEFAULT_PY)) else [exe, "-m", "ziglang", "cc"]


DEFAULT_PY = r"C:\Users\Administrator\.workbuddy\binaries\python\envs\default\Scripts\python.exe"
ZIG = _zig_cmd()
ENV = dict(os.environ, ZIG_GLOBAL_CACHE_DIR=os.path.join(STUDY, ".zigcache"))

# `_repos3` holds full-history clones (blobless, fetched on demand). A shallow
# clone is useless for this study: within the last N commits most files are
# untouched, so the "newest revision that touched the path" carries the same
# content as HEAD and every pair collapses to a zero-change pair.
REPO_DIRS = [os.path.join(STUDY, "_repos3"),
             os.path.join(STUDY, "_repos2"),
             os.path.join(STUDY, "_repos")]

# Bound on on-demand blob fetches per project. A blobless clone resolves a
# missing blob with one network round trip, so this caps the cost without
# preventing the history walk.
MAX_REMOTE_FETCH_PER_PROJECT = 40

# Checked-out worktrees are shared across the studies in this collection; the
# profiling study materialises them, so the benchmark reuses them instead of
# creating a second copy of every repository.
WORKTREE_ROOTS = [
    os.path.join(STUDY, "05_IE_demo_complexity", "_worktrees"),
    os.path.join(ROOT, "_worktrees"),
]

# project key -> (display, domain, ecosystem hint)
PROJECTS = {
    "riot": ("RIOT", "embedded-OS"),
    "contiki-ng": ("Contiki-NG", "embedded-OS"),
    "zephyr": ("Zephyr", "embedded-OS"),
    "esp-idf": ("ESP-IDF", "embedded-OS"),
    "esp8266_ard": ("ESP8266-Arduino", "embedded-OS"),
    "mbed-os": ("Mbed-OS", "embedded-OS"),
    "freertos": ("FreeRTOS-Kernel", "RTOS-kernel"),
    "iotivity": ("IoTivity-Lite", "IoT-middleware"),
    "mosquitto": ("Mosquitto", "industrial-messaging"),
    "beremiz": ("Beremiz", "SCADA-runtime"),
    "pycomm3": ("pycomm3", "industrial-protocol-client"),
    "ros2_control": ("ros2_control", "industrial-robot-control"),
    "openplc_v3": ("OpenPLC_v3", "PLC-runtime"),
    "evcc": ("evcc", "industrial-energy-control"),
    "serial_studio": ("Serial-Studio", "industrial-HMI"),
    "node_opcua": ("node-opcua", "industrial-protocol-stack"),
    "ditto": ("Eclipse-Ditto", "industrial-IoT-platform"),
    "hivemq": ("HiveMQ-CE", "industrial-messaging"),
    "mainflux": ("Mainflux", "industrial-IoT-platform"),
    "tb_gateway": ("ThingsBoard-Gateway", "industrial-IoT-gateway"),
    "beremiz": ("Beremiz", "SCADA-runtime"),
}

STUBBABLE = {"stdint.h", "stddef.h", "stdbool.h", "string.h", "stdio.h",
             "stdlib.h", "math.h", "limits.h", "stdarg.h", "inttypes.h",
             "errno.h", "time.h", "assert.h", "ctype.h", "float.h",
             "iso646.h", "stdalign.h"}
SKIP_INCLUDE = re.compile(r'#\s*include\s+[<"]([^>"]+)[>"]')
PATH_DENY = re.compile(r"(^|/)(test|tests|testing|example|examples|demo|"
                       r"docs?|tools?|script|scripts|build|dist|"
                       r"third_party|3rdparty|vendor|external|node_modules|"
                       r"contrib|target)(/|$)", re.I)

# migration intents recognised from the *observed* delta
INTENTS = [
    ("MI-STR", "bounded-string-handling",
     r"\b(strcpy|strcat|sprintf|gets|strncpy)\s*\(",
     r"\b(strncpy|strncat|snprintf|memcpy|strlcpy)\s*\(",
     "unbounded string primitive replaced by a bounded variant"),
    ("MI-ALLOC", "static-instead-of-heap",
     r"\b(malloc|calloc|realloc|free)\s*\(", None,
     "run-time allocation replaced by static storage"),
    ("MI-TYPE", "explicit-width-types",
     r"(?<![_A-Za-z0-9])(int|long|short|char|unsigned)\s+\w+\s*[;,)\[]",
     r"\b(u?int(8|16|32|64)_t)\b",
     "implementation-defined widths replaced by exact-width types"),
    ("MI-ERR", "error-return-propagation", None,
     r"\b(if\s*\([^)]*<[=]?\s*0[^)]*\)\s*return)|goto\s+err",
     "unchecked failure path replaced by explicit propagation"),
    ("MI-BOUND", "bounded-iteration",
     r"\bwhile\s*\(\s*!?\s*\w+\s*\)", r"\bfor\s*\(\s*\w+\s*=\s*0\s*;",
     "unbounded loop replaced by counted loop"),
    ("MI-VOLATILE", "volatile-hardware-access",
     r"^\s*\w+\s*=\s*\*\s*\(", r"\bvolatile\b",
     "hardware register access made volatile"),
    ("MI-PTR", "const-correctness", None,
     r"\bconst\s+(char|uint8_t|unsigned char)\s*\*",
     "read-only buffer annotated const"),
]


def sh(args, cwd=None, timeout=240, env=None):
    try:
        r = subprocess.run(args, capture_output=True, cwd=cwd, timeout=timeout,
                           env=env or os.environ)
        return r.returncode, r.stdout.decode("utf-8", "replace"), \
            r.stderr.decode("utf-8", "replace")
    except Exception as e:
        return 1, "", str(e)


def git(repo, args, timeout=240):
    return sh(["git", "-C", repo] + args, timeout=timeout)


def blob_present(repo, spec):
    rc, _, _ = git(repo, ["cat-file", "-e", spec], timeout=60)
    return rc == 0


def _local_macros(src):
    """Recover object-like macros a translation unit uses but does not define.

    Project-local headers are stripped before compilation, so the constants they
    define (`FLASH_SECTOR_SIZE`, `MAX_PINS`, ...) become undeclared identifiers
    and every candidate fails to compile for a reason that has nothing to do
    with the migration under study. Reading the macros back out of the *unit's
    own text* -- plus the file's leading comment block, which in this corpus
    documents the constants -- lets the scaffolding supply them, and is derived
    entirely from the file being compiled rather than from a hand-written list.
    """
    out = {}
    for m in re.finditer(r"^\s*#\s*define\s+([A-Za-z_]\w*)\s+"
                         r"(\(?[^=\n]*\)?)\s*$", src, re.M):
        name, val = m.group(1), m.group(2).strip()
        if not re.fullmatch(r"[-+]?\d+[uUlL]*|0[xX][0-9a-fA-F]+[uUlL]*", val):
            continue
        if name.isidentifier():
            out[name] = val
    return out


def build_stub(includes, macros=None):
    import hashlib
    lines = ["#ifndef GENERATED_STUBS", "#define GENERATED_STUBS",
             "#include <stdint.h>", "#include <stddef.h>"]
    # Constants the stripped project headers used to provide. They are declared
    # only when the unit does not define them itself, so a pair always sees the
    # same scaffolding and a migration cannot be credited for changing a value.
    for name in sorted(macros or {}):
        lines.append(f"#ifndef {name}")
        lines.append(f"#define {name} {macros[name]}")
        lines.append("#endif")
    for inc in sorted(includes):
        if inc in STUBBABLE:
            continue
        base = re.sub(r"[^\w]", "_", inc).strip("_").upper() or "STUB"
        h = hashlib.sha1(inc.encode()).hexdigest()[:8].upper()
        g = f"STUB_{base}_{h}"
        lines += [f"/* stub for {inc} */", f"#ifndef {g}", f"#define {g}",
                  "#include <stdint.h>", "#include <stddef.h>",
                  f"typedef struct {{ int stub_dummy; }} stub_t{g.lower()};",
                  "#endif"]
    lines.append("#endif")
    return "\n".join(lines) + "\n"


def strip_includes(src):
    out = []
    for line in src.splitlines():
        m = re.match(r"\s*#\s*include\s*[<\"]([^>\"]+)[>\"]", line)
        if m:
            if os.path.basename(m.group(1)) in STUBBABLE:
                out.append(line)
            continue
        out.append(line)
    return "\n".join(out)


def try_compile(src):
    # A fixed probe filename gets locked by viewers and by concurrent runs, so
    # the build directory is per-process and the probe name carries its index.
    d = os.path.join(SRC, ".build", str(os.getpid()))
    os.makedirs(d, exist_ok=True)
    f = os.path.join(d, f"probe{_probe_counter()}.c")
    with open(f, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(src)
    # `sh` returns (rc, stdout, stderr); the diagnostic text is the error stream.
    rc, _out, err = sh(ZIG + ["-std=c99", "-w", f, "-c", "-o",
                              os.path.join(d, "probe.o")], timeout=180, env=ENV)
    # The probe file is left in place: deleting it is not worth a failure mode,
    # and the name is unique per compile so nothing is ever overwritten.
    return rc == 0, err


_PROBE = [0]


def _probe_counter():
    _PROBE[0] += 1
    return _PROBE[0]


def find_repo(key):
    for b in REPO_DIRS:
        p = os.path.join(b, key)
        if os.path.isdir(os.path.join(p, ".git")):
            return p
    return None


def worktree_for(key):
    for base in WORKTREE_ROOTS:
        if not os.path.isdir(base):
            continue
        for d in os.listdir(base):
            if d.split("__")[0] == key:
                p = os.path.join(base, d)
                if os.path.isdir(p) and os.listdir(p):
                    return p
    return None


def detect_intents(old, new):
    """Intents present in the *new* revision and absent, weaker, or replaced in
    the old one.

    The direction matters: a migration is what the new revision does that the
    old one did not, so an intent fires on the modern side when its target
    idiom is present there and either missing from the legacy side or strictly
    more frequent in it.
    """
    hits = []
    for iid, name, lx, mx, why in INTENTS:
        mo = len(re.findall(mx, new, re.I)) if mx else 0
        lo = len(re.findall(lx, old, re.I)) if lx else 0
        if lx and mx:
            if mo > 0 and (lo == 0 or mo > lo):
                hits.append(dict(id=iid, name=name, rationale=why,
                                 legacy_hits=lo, modern_hits=mo))
        elif mx and mo > 0 and old != new:
            # idiom the legacy side simply does not use
            hits.append(dict(id=iid, name=name, rationale=why,
                             legacy_hits=0, modern_hits=mo))
    return hits


def changed_lines(a, b):
    """Number of non-blank lines that differ between two revisions.

    Compared as multisets of trimmed lines: renaming a variable or reordering
    statements is not a migration, whereas adding a bound check, replacing an
    unbounded primitive, or widening a type is.
    """
    from collections import Counter
    ca = Counter(l.strip() for l in a.splitlines() if l.strip())
    cb = Counter(l.strip() for l in b.splitlines() if l.strip())
    diff = sum((ca - cb).values()) + sum((cb - ca).values())
    return diff


def _write_text(path, text, retries=6, wait=1.5):
    """Write a generated source file, retrying on a Windows sharing violation.

    A file left open in an editor raises PermissionError (Errno 13) rather than
    a "busy" error, and the condition is transient. Retrying here keeps a
    preview window from aborting a build that has already spent minutes of
    compiler time.
    """
    last = None
    for _ in range(retries):
        try:
            with open(path, "w", encoding="utf-8", newline="\n") as f:
                f.write(text)
            return
        except PermissionError as e:
            last = e
            time.sleep(wait)
    raise last


def _ordered_code(text):
    """Comment-stripped, whitespace-normalised, order-preserving line list."""
    t = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    t = re.sub(r"//[^\n]*", " ", t)
    return [re.sub(r"\s+", " ", l).strip() for l in t.splitlines()
            if re.sub(r"\s+", " ", l).strip()]


def main():
    per_project = int(os.environ.get("MAX_PER_PROJECT", "6"))
    sample_size = int(os.environ.get("CAND_SAMPLE", "40"))
    tasks, stats = [], Counter()
    per_project_seen = Counter()

    # Projects can be excluded to bound the run: every candidate costs two
    # compiler invocations, so a repository with thousands of C files dominates
    # the schedule. Excluded projects are reported, never silently dropped.
    only = {s for s in os.environ.get("ONLY_PROJECTS", "").split(",") if s}
    if only:
        print(f"[only] {sorted(only)}", flush=True)
    for key, (disp, domain) in PROJECTS.items():
        if only and key not in only:
            print(f"[skip] {disp}: not in ONLY_PROJECTS", flush=True)
            continue
        repo = find_repo(key)
        wt = worktree_for(key)
        if not repo or not wt:
            print(f"[skip] {disp}: no clone or worktree", flush=True)
            continue
        # candidate C files from the local worktree only
        cands = []
        for dirpath, dirnames, filenames in os.walk(wt):
            dirnames[:] = [d for d in dirnames
                           if not d.startswith(".")
                           and d not in ("node_modules", "build", "dist",
                                         "target", "out", "vendor")]
            rel = os.path.relpath(dirpath, wt).replace("\\", "/")
            if PATH_DENY.search(rel + "/"):
                continue
            for fn in filenames:
                if fn.endswith(".c") and not PATH_DENY.search(fn):
                    cands.append(os.path.join(dirpath, fn))
        if not cands:
            print(f"[skip] {disp}: no candidate C files", flush=True)
            continue
        random.seed(20260201 + sum(ord(c) for c in key))
        random.shuffle(cands)
        if len(cands) > sample_size:
            cands = cands[:sample_size]
        print(f"[scan] {disp}: {len(cands)} candidates", flush=True)

        taken = 0
        for full in cands:
            if taken >= per_project:
                break
            rel = os.path.relpath(full, wt).replace("\\", "/")
            try:
                if os.path.getsize(full) > 120_000:
                    stats["too_large"] += 1
                    continue
                cur = open(full, encoding="utf-8", errors="ignore").read()
            except OSError:
                continue
            if not (200 < len(cur) < 120_000):
                stats["size_out_of_range"] += 1
                continue

            # Legacy side: the oldest revision of the same path that is still
            # readable, paired against the working-tree content (HEAD). On a
            # blobless clone a `git show <rev>:<path>` fetches that single blob
            # on demand; the fetches are strictly sequential and individually
            # bounded, because the earlier design that resolved every candidate
            # in parallel produced ~160 concurrent git processes and exhausted
            # the machine.
            rc, out, _ = git(repo, ["log", "--format=%H", "-n60", "--", rel],
                             timeout=300)
            hist = [h for h in out.split() if h] if rc == 0 else []
            rc3, head, _ = git(repo, ["rev-parse", "HEAD"], timeout=60)
            head_sha = head.strip()
            legacy = None
            remote_fetches = 0
            # oldest-first so the pair spans as much of the history as possible
            for rev in reversed(hist):
                if rev == head_sha:
                    continue
                spec = f"{rev}:{rel}"
                if not blob_present(repo, spec):
                    # one on-demand fetch, counted so a project with a long
                    # history cannot turn into an unbounded network loop
                    if remote_fetches >= MAX_REMOTE_FETCH_PER_PROJECT:
                        stats["skipped_remote_history"] += 1
                        continue
                    remote_fetches += 1
                rc2, txt, _ = git(repo, ["show", spec], timeout=300)
                if rc2 == 0 and 200 < len(txt) < 120_000:
                    legacy = (rev, txt)
                    break
                stats["legacy_unreadable"] += 1
            if not legacy:
                stats["no_local_legacy"] += 1
                continue

            incs = set(SKIP_INCLUDE.findall(legacy[1]))
            # the scaffolding is derived from the union of both revisions, so
            # the two sides of a pair are compiled against identical constants
            macros = dict(_local_macros(legacy[1]))
            macros.update(_local_macros(cur))
            stub = build_stub(incs, macros)
            legacy_unit = stub + "\n" + strip_includes(legacy[1]) + "\n"
            modern_unit = stub + "\n" + strip_includes(cur) + "\n"
            # Both revisions are compiled, but a failure is *recorded*, not
            # fatal: embedded units routinely reference platform headers that
            # no stub can satisfy, and dropping all of them would leave a corpus
            # of only the trivially self-contained files. `compiles_both` marks
            # the subset on which the execution-based evaluation can run, and
            # the static axes are reported for the whole corpus.
            ok_l, err_l = try_compile(legacy_unit)
            ok_m, err_m = try_compile(modern_unit)
            intents = detect_intents(legacy[1], cur)
            if not intents:
                stats["no_intent"] += 1
                continue
            # A migration task must contain a *substantive* change, otherwise
            # the "legacy" side is just the same file with a different variable
            # name and the benchmark measures nothing. The threshold is on the
            # share of changed non-blank lines between the two revisions.
            n_diff = changed_lines(legacy[1], cur)
            frac = n_diff / max(1, min(legacy[1].count("\n"),
                                       cur.count("\n")) + 1)
            if frac < MIN_CHANGE_FRACTION:
                stats["change_too_small"] += 1
                continue
            # A line-multiset change count is not sufficient: a pair can differ
            # only by one added `#define` and still clear a low threshold while
            # being the same program. The order-preserving normalised form
            # catches that, and such a pair carries no migration to model.
            if _ordered_code(legacy[1]) == _ordered_code(cur):
                stats["normalised_equivalent"] += 1
                continue

            tid = f"{key}__{hashlib.sha1(rel.encode()).hexdigest()[:8]}"
            d = os.path.join(SRC, tid)
            os.makedirs(d, exist_ok=True)
            _write_text(os.path.join(d, "legacy.c"),
                        f"/* {disp} :: {rel}\n   legacy revision: {legacy[0]}\n"
                        f"   domain: {domain}\n   provenance: real repository "
                        f"history, blob verified present locally\n*/\n"
                        + legacy_unit)
            _write_text(os.path.join(d, "modern.c"),
                        f"/* {disp} :: {rel}\n   modern revision: {head_sha}\n"
                        f"   domain: {domain}\n*/\n" + modern_unit)
            tasks.append(dict(
                task_id=tid, project=disp, domain=domain, path=rel,
                legacy_sha=legacy[0], modern_sha=head_sha,
                legacy_lines=legacy[1].count("\n") + 1,
                modern_lines=cur.count("\n") + 1,
                legacy_bytes=len(legacy[1]), modern_bytes=len(cur),
                changed_lines=n_diff, change_fraction=round(frac, 4),
                compiles_legacy=int(ok_l), compiles_modern=int(ok_m),
                compiles_both=int(ok_l and ok_m),
                legacy_error_head=(err_l.splitlines()[0][:140]
                                   if err_l and not ok_l else ""),
                modern_error_head=(err_m.splitlines()[0][:140]
                                   if err_m and not ok_m else ""),
                intents=intents, n_intents=len(intents),
                provenance="both sides from real repository revisions; "
                           "legacy blob verified present locally"))
            taken += 1
            per_project_seen[disp] += 1
            stats["accepted"] += 1
            print(f"  [ok] {tid} {rel} "
                  f"({legacy[1].count(chr(10))+1}->{cur.count(chr(10))+1} lines, "
                  f"{len(intents)} intents)", flush=True)
        print(f"[done] {disp}: {taken} tasks", flush=True)

    p = os.path.join(DATA, "benchmark_tasks.json")
    json.dump(dict(
        version="2.0.0",
        construction=("both sides of every pair are real revisions of the same "
                      "file from the project's own history; the legacy blob is "
                      "verified present locally before use, and candidates whose "
                      "history is remote are counted as skipped rather than "
                      "replaced by hand-written code"),
        n_tasks=len(tasks), tasks=tasks,
        screening=dict(stats=dict(stats)),
        per_project=dict(per_project_seen),
    ), open(p, "w", encoding="utf-8"), indent=2, ensure_ascii=False)
    print(f"[write] {p} tasks={len(tasks)}")
    print("BENCHMARK BUILD COMPLETE", dict(stats))


if __name__ == "__main__":
    main()

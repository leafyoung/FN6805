#!/usr/bin/env bash
#
# smoke_test.sh — build and run every example project in this repo.
#
# A project passes if it compiles cleanly with clang++ and then runs to
# completion without crashing. Program output is never checked for correctness,
# only for the absence of a crash.
#
# Unlike the root Makefile (which globs every .cpp in the repo into a single
# `main` binary, and therefore tolerates only one active main()), this script
# compiles each numbered directory in isolation, so all projects can be tested
# in one pass.
#
# Human-readable results go to stdout; a machine-readable record of every
# project goes to $SMOKE_DIR/results.jsonl (one JSON object per line).

set -uo pipefail

usage() {
  cat <<'USAGE'
Usage: ./smoke_test.sh [options] [filter...]

  filter...        only test projects whose directory name contains a filter
                   (e.g. `./smoke_test.sh 52 64`)

Options:
  --build-only     compile only, skip the run phase
  --verbose, -v    echo each program's captured output
  --timeout SECS   per-program run timeout (default 60)
  --keep, -k       keep run sandboxes after a successful pass
  --wasm           build/run for wasm32-wasip1 with the CPPBox classroom
                   toolchain instead of the host compiler
  --help, -h       show this help

Environment overrides:
  CXX              compiler (default clang++)
  CXXFLAGS         compile flags (default matches the root Makefile)
  SMOKE_DIR        scratch directory for binaries, logs, sandboxes
  CPPBOX_ROOT      CPPBox checkout (default ~/devv/fin/classroom)
  WASI_SDK         wasi-sdk root (default: newest under
                   $CPPBOX_ROOT/wasi-toolchain/wasi-sdk-*)
  WASM_RUNNER      wasmtime | node (default: wasmtime if on PATH, else node)
USAGE
}

# --- Configuration -----------------------------------------------------------

REPO_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
readonly REPO_ROOT

# Match the root Makefile so the script tests what students actually build.
CXX="${CXX:-clang++}"
CXXFLAGS="${CXXFLAGS:--g -Wmost -Werror --std=c++17}"

# Scratch area for binaries, logs, and run sandboxes. Kept out of the repo so a
# smoke run never dirties `git status`.
SMOKE_DIR="${SMOKE_DIR:-${TMPDIR:-/tmp}/fn6805-smoke}"

RUN_TIMEOUT=60
TARGET=host
BUILD_ONLY=0
VERBOSE=0
KEEP=0
FILTERS=()

# Projects that are *supposed* to fail to build. AGENTS.md explains why: both
# demonstrate the "multiple definition" linker error you get from putting
# function definitions in a header. Fixing them would destroy the lesson, so the
# script asserts that they still fail — and reports XPASS if they start passing.
#
# Kept here rather than in a data file on purpose: two entries do not justify a
# YAML/jq dependency, and the rationale belongs next to the list.
EXPECTED_BUILD_FAILURES=(
  "30-header-file"
  "72-multiple_inclusion"
)

# An XFAIL that accepts *any* build error is a weak assertion: a typo elsewhere
# in the file satisfies it while the lesson it is supposed to demonstrate stays
# masked. Each expected failure therefore declares the diagnostic it must
# produce, and a mismatch is reported as a real failure.
expected_failure_pattern() {
  case "$1" in
    # GNU ld says "multiple definition"; wasm-ld says "duplicate symbol".
    # Both are the lesson; a typo-induced parse error is not.
    30-header-file | 72-multiple_inclusion)
      echo 'redefinition|multiple definition|duplicate symbol' ;;
    *) echo '' ;;
  esac
}

# --- wasm target (CPPBox classroom parity) -----------------------------------
#
# The CPPBox teaching IDE (~/devv/fin/classroom) compiles and runs student code
# against wasm32-wasip1 with a bundled wasi-sdk and an embedded wasmtime, and
# falls back to podman only for code it cannot run that way. `--wasm` reproduces
# that path so this repo can be validated against the environment students
# actually use.

CPPBOX_ROOT="${CPPBOX_ROOT:-$HOME/devv/fin/classroom}"

# Flags copied from cppbox-core/src/wasi_exec.rs::compile. Each one is load
# bearing there, and the comments explaining why live in that file:
#   -fwasm-exceptions + the two -mllvm flags   non-legacy wasm EH
#   -Wl,--initial-memory / --max-memory        memory limits
#   -lunwind                                   unwinder for wasm EH
#   -lc-printscan-long-double                  wasi-libc trims long double
#                                              printf/scanf by default
#   -Wl,-z,stack-size                          see WASM_STACK_SIZE below
#
# WASM_STACK_SIZE is the one addition to CPPBox's own flag list. wasi-sdk
# defaults the wasm stack to 64 KiB, so an ordinary local array overflows it and
# traps with "memory access out of bounds" - 73-cache_locality's
# array<array<double,512>,512> is 2 MiB and did exactly that. 8 MiB matches the
# usual host default. CPPBox should pass the same flag; until it does, this
# repo's wasm runs are slightly more forgiving than the classroom's.
WASM_STACK_SIZE="${WASM_STACK_SIZE:-8388608}"
readonly WASM_TARGET_FLAGS="--target=wasm32-wasip1 -O2 -Wall -Wextra \
-fwasm-exceptions -mllvm -wasm-enable-eh -mllvm -wasm-use-legacy-eh=false \
-Wl,--initial-memory=67108864 -Wl,--max-memory=4294967296 \
-Wl,-z,stack-size=$WASM_STACK_SIZE \
-lunwind -lc-printscan-long-double"

# CPPBox routes any project textually mentioning one of these headers to podman,
# because wasm32-wasip1-threads is non-functional upstream (see
# wasi_exec.rs::uses_threading). Matching that list exactly keeps this test
# honest: a module skipped here is a module CPPBox would not run on wasm either.
readonly WASM_THREAD_MARKERS='<thread>|<future>|<mutex>|<condition_variable>|<atomic>|<shared_mutex>'

# Headers wasi-sdk's libc++ cannot serve at all, beyond CPPBox's own list.
# <execution> brings in the parallel algorithms (std::execution::par), which
# need threads and are simply absent from the wasm sysroot - 52-stl fails to
# compile because of it. CPPBox's uses_threading() does not list <execution>, so
# it currently tries wasm and hands the student a compile error; adding it there
# would route such code to podman instead.
readonly WASM_UNSUPPORTED_MARKERS='<execution>'

# Known wasm incompatibilities. Reported as XFAIL-WASM with the reason, so the
# suite stays green while the limitation stays visible.
# Empty today. 52-stl is now skipped via WASM_UNSUPPORTED_MARKERS (<execution>),
# and 73-cache_locality passes since WASM_STACK_SIZE raised the 64 KiB default.
wasm_expected_failure_reason() {
  case "$1" in
    *) echo "" ;;
  esac
}

WASM_CXX=""
WASM_SYSROOT=""
resolve_wasm_toolchain() {
  local sdk="${WASI_SDK:-}"
  if [[ -z "$sdk" ]]; then
    # Newest wasi-sdk-* under the CPPBox checkout.
    sdk="$(find "$CPPBOX_ROOT/wasi-toolchain" -maxdepth 1 -type d -name 'wasi-sdk-*' 2>/dev/null | sort -V | tail -1)"
  fi
  if [[ -z "$sdk" || ! -x "$sdk/bin/clang++" ]]; then
    echo "smoke_test.sh: no wasi-sdk found (looked under $CPPBOX_ROOT/wasi-toolchain)." >&2
    echo "  Set WASI_SDK=/path/to/wasi-sdk-NN.0, or CPPBOX_ROOT to your CPPBox checkout." >&2
    return 1
  fi
  WASM_CXX="$sdk/bin/clang++"
  WASM_SYSROOT="$sdk/share/wasi-sysroot"
  [[ -d "$WASM_SYSROOT" ]] || { echo "smoke_test.sh: missing sysroot $WASM_SYSROOT" >&2; return 1; }
}

# CPPBox embeds wasmtime as a library; the closest shell equivalent is the
# wasmtime CLI. Node ships a WASI preview1 implementation and is used when
# wasmtime is absent - the same ABI, a different host.
WASM_RUN_KIND=""
WASM_NODE_SHIM=""
WASMTIME_BIN=""
resolve_wasm_runner() {
  local want="${WASM_RUNNER:-}"
  if [[ "$want" == "node" ]] || { [[ -z "$want" ]] && ! command -v wasmtime >/dev/null 2>&1; }; then
    command -v node >/dev/null 2>&1 || { WASM_RUN_KIND=""; return 0; }
    WASM_RUN_KIND="node"
    WASM_NODE_SHIM="$SMOKE_DIR/wasi_run.mjs"
    cat >"$WASM_NODE_SHIM" <<'SHIM'
// Minimal WASI preview1 host, mirroring CPPBox's WasiCtxBuilder: stdin piped,
// the job directory preopened as ".". Exits 125 on a wasm trap so the caller
// can tell a trap from a program's own non-zero exit.
import { WASI } from 'node:wasi';
import { readFile } from 'node:fs/promises';
const [wasmPath, dir] = process.argv.slice(2);
const wasi = new WASI({ version: 'preview1', args: ['main'], env: {},
  preopens: { '.': dir }, returnOnExit: true });
const module = await WebAssembly.compile(await readFile(wasmPath));
try {
  const instance = await WebAssembly.instantiate(module, wasi.getImportObject());
  process.exitCode = wasi.start(instance);
} catch (err) {
  console.error(String((err && err.stack) || err));
  process.exitCode = 125;
}
SHIM
    return 0
  fi
  if command -v wasmtime >/dev/null 2>&1; then
    WASMTIME_BIN="$(command -v wasmtime)"
    WASM_RUN_KIND="wasmtime"
  fi
}

# Echoes the reason this project cannot run on wasm, or "" if it can.
wasm_skip_reason() { # wasm_skip_reason <project>
  local project="$1"
  if _matches_markers "$project" "$WASM_THREAD_MARKERS"; then
    echo "uses threads; CPPBox routes these to podman"
  elif _matches_markers "$project" "$WASM_UNSUPPORTED_MARKERS"; then
    echo "uses <execution>; wasi-sdk libc++ has no parallel algorithms"
  else
    echo ""
  fi
}

_matches_markers() { # _matches_markers <project> <regex>
  find "$REPO_ROOT/$1" -maxdepth 1 \( -name '*.cpp' -o -name '*.h' \) -print0 2>/dev/null |
    xargs -0 -r grep -l -E "$2" 2>/dev/null | head -1 | grep -q .
}

# --- Argument parsing --------------------------------------------------------

while (($# > 0)); do
  case "$1" in
    --build-only) BUILD_ONLY=1 ;;
    --verbose | -v) VERBOSE=1 ;;
    --keep | -k) KEEP=1 ;;
    --wasm) TARGET=wasm ;;
    --timeout)
      RUN_TIMEOUT="${2:?--timeout needs a value in seconds}"
      [[ "$RUN_TIMEOUT" =~ ^[0-9]+$ ]] || {
        echo "smoke_test.sh: --timeout expects whole seconds, got '$RUN_TIMEOUT'" >&2
        exit 2
      }
      shift
      ;;
    --help | -h) usage; exit 0 ;;
    -*) echo "smoke_test.sh: unknown option '$1'" >&2; usage >&2; exit 2 ;;
    *) FILTERS+=("$1") ;;
  esac
  shift
done

# --- Terminal styling (plain text when not a tty, e.g. piped into a file) ----

if [[ -t 1 ]]; then
  readonly C_RESET=$'\033[0m' C_RED=$'\033[31m' C_GREEN=$'\033[32m'
  readonly C_YELLOW=$'\033[33m' C_BLUE=$'\033[34m' C_DIM=$'\033[2m'
else
  readonly C_RESET='' C_RED='' C_GREEN='' C_YELLOW='' C_BLUE='' C_DIM=''
fi

# --- Portability -------------------------------------------------------------
#
# GNU coreutils and GNU date are not everywhere: stock macOS has no `timeout`
# (Homebrew installs it as `gtimeout`) and its BSD `date` has no %N. Both are
# resolved once, loudly, instead of failing per project.

TIMEOUT_CMD=""
resolve_timeout_cmd() {
  local candidate
  for candidate in timeout gtimeout; do
    if command -v "$candidate" >/dev/null 2>&1; then
      TIMEOUT_CMD="$candidate"
      return 0
    fi
  done
  printf '%swarning: no timeout/gtimeout on PATH; programs run without a time limit%s\n' \
    "$C_YELLOW" "$C_RESET" >&2
  return 0
}

# Milliseconds since the epoch. Prefers bash 5's EPOCHREALTIME (no subprocess,
# no GNU dependency) and degrades to whole seconds where neither is available.
now_ms() {
  if [[ -n "${EPOCHREALTIME:-}" ]]; then
    local microseconds="${EPOCHREALTIME/[.,]/}"
    echo $((microseconds / 1000))
    return
  fi
  local nanoseconds
  nanoseconds="$(date +%s%N 2>/dev/null)"
  if [[ "$nanoseconds" =~ ^[0-9]+$ ]]; then
    echo $((nanoseconds / 1000000))
  else
    echo $(($(date +%s) * 1000)) # BSD date: second granularity only
  fi
}

# --- Helpers -----------------------------------------------------------------

is_expected_build_failure() {
  local project="$1" expected
  ((${#EXPECTED_BUILD_FAILURES[@]} == 0)) && return 1
  for expected in "${EXPECTED_BUILD_FAILURES[@]}"; do
    [[ "$project" == "$expected" ]] && return 0
  done
  return 1
}

matches_filter() {
  ((${#FILTERS[@]} == 0)) && return 0
  local project="$1" filter
  for filter in "${FILTERS[@]}"; do
    [[ "$project" == *"$filter"* ]] && return 0
  done
  return 1
}

json_escape() { # minimal string escaping for the JSONL record
  local text="$1"
  text="${text//\\/\\\\}"
  text="${text//\"/\\\"}"
  text="${text//$'\n'/ }"
  text="${text//$'\t'/ }"
  printf '%s' "$text"
}

# A "project" is any top-level directory holding at least one .cpp file. Sources
# come from the directory itself, never recursively, so each project stays
# self-contained and one directory = one program.
discover_projects() {
  local dir
  for dir in "$REPO_ROOT"/*/; do
    dir="$(basename "${dir%/}")"
    compgen -G "$REPO_ROOT/$dir/*.cpp" >/dev/null || continue
    echo "$dir"
  done
}

# --- Per-project phases ------------------------------------------------------

build_project() { # build_project <project> <binary> <log>
  local project="$1" binary="$2" log="$3"
  local sources=()
  while IFS= read -r source_file; do sources+=("$source_file"); done < <(
    find "$REPO_ROOT/$project" -maxdepth 1 -name '*.cpp' | sort
  )
  SOURCE_COUNT=${#sources[@]}

  if [[ "$TARGET" == wasm ]]; then
    { echo "# $WASM_CXX --sysroot=$WASM_SYSROOT $WASM_TARGET_FLAGS --std=c++17 ${sources[*]} -o $binary"; } >"$log"
    # shellcheck disable=SC2086  # flag strings are intentionally word-split
    "$WASM_CXX" "--sysroot=$WASM_SYSROOT" $WASM_TARGET_FLAGS --std=c++17 \
      "${sources[@]}" -o "$binary" >>"$log" 2>&1
    return
  fi

  # shellcheck disable=SC2086  # CXXFLAGS is intentionally word-split
  "$CXX" $CXXFLAGS "${sources[@]}" -o "$binary" >"$log" 2>&1
}

# Runs the built binary in a disposable copy of its directory, so data files the
# program writes (54-fstream_demo's test.txt, 50-binaryinoutfile's out_test.dat)
# never touch the working tree. Returns the program's exit code, or 99 if the
# sandbox itself could not be prepared.
run_project() { # run_project <project> <binary> <log>
  local project="$1" binary="$2" log="$3"
  local sandbox="$SMOKE_DIR/run/$project"

  rm -rf "$sandbox"
  if ! mkdir -p "$sandbox" || ! cp -R "$REPO_ROOT/$project/." "$sandbox/" >"$log" 2>&1; then
    echo "harness: could not prepare sandbox $sandbox" >>"$log"
    return 99
  fi

  # Examples disagree about their working directory: most open "./data.txt"
  # (relative to the project dir), while 50-binaryinoutfile opens
  # "50-binaryinoutfile/out_test.dat" (relative to the repo root, matching the
  # Makefile's single-binary layout). This self-referential symlink makes both
  # spellings resolve inside the sandbox, so neither convention needs a
  # per-project special case.
  ln -sfn . "$sandbox/$project"

  # stdin is /dev/null so interactive examples (20-simple_calc) hit EOF and exit
  # instead of blocking forever.
  #
  # For wasm the program is a module, not an executable: it runs under a WASI
  # host with the sandbox preopened as ".", mirroring CPPBox's
  # WasiCtxBuilder::preopened_dir(job_dir, ".").
  local -a command=()
  if [[ "$TARGET" == wasm ]]; then
    case "$WASM_RUN_KIND" in
      # -W exceptions=y is required: the modules are built with
      # -fwasm-exceptions, and the CLI (unlike CPPBox, which sets
      # Config::wasm_exceptions) leaves the proposal off by default, failing
      # with "exceptions proposal not enabled". Verified with wasmtime 46.0.3,
      # the version CPPBox pins.
      wasmtime) command=("$WASMTIME_BIN" run -W exceptions=y --dir "$sandbox::." "$binary") ;;
      node) command=(node --no-warnings "$WASM_NODE_SHIM" "$binary" "$sandbox") ;;
      *) echo "harness: no wasm runner available" >>"$log"; return 99 ;;
    esac
  else
    command=("$binary")
  fi

  (
    cd "$sandbox" || exit 99
    if [[ -n "$TIMEOUT_CMD" ]]; then
      "$TIMEOUT_CMD" -k 5 "$RUN_TIMEOUT" "${command[@]}" </dev/null
    else
      "${command[@]}" </dev/null
    fi
  ) >"$log" 2>&1
}

# smoke_one_project <project>
#
# Runs both phases and reports the outcome through three named globals rather
# than a packed string, so callers never parse a message to learn the verdict:
#   PROJECT_STATUS      pass | build_fail | xfail | xpass | crash | timeout | exit | harness
#   PROJECT_DETAIL      human-readable reason ("" when passing)
#   PROJECT_DURATION_MS wall-clock milliseconds of the run phase (0 if not run)
#
# Exit-status policy, the one real judgement call in this script:
#   * a fatal signal (128+N) is always a crash — segfault, abort, uncaught
#     exception, failed assert;
#   * a timeout means the program hung, or waited on input we did not feed;
#   * a plain non-zero exit counts as a failure, because no example here is
#     meant to report an error status. Loosen this branch if one ever does.
PROJECT_STATUS=""
PROJECT_DETAIL=""
PROJECT_DURATION_MS=0
SOURCE_COUNT=0

smoke_one_project() {
  local project="$1" binary="$2" build_log="$3" run_log="$4"
  PROJECT_STATUS=""
  PROJECT_DETAIL=""
  PROJECT_DURATION_MS=0

  # Projects wasm cannot serve at all are skipped rather than failed.
  if [[ "$TARGET" == wasm ]]; then
    local skip_reason
    skip_reason="$(wasm_skip_reason "$project")"
    if [[ -n "$skip_reason" ]]; then
      PROJECT_STATUS="skip_wasm"
      PROJECT_DETAIL="$skip_reason"
      return
    fi
  fi

  local wasm_known=""
  [[ "$TARGET" == wasm ]] && wasm_known="$(wasm_expected_failure_reason "$project")"

  if ! build_project "$project" "$binary" "$build_log"; then
    if [[ -n "$wasm_known" ]]; then
      PROJECT_STATUS="xfail_wasm"
      PROJECT_DETAIL="$wasm_known"
    elif is_expected_build_failure "$project"; then
      local pattern
      pattern="$(expected_failure_pattern "$project")"
      if [[ -n "$pattern" ]] && ! grep -qiE "$pattern" "$build_log"; then
        PROJECT_STATUS="xfail_wrong_reason"
        PROJECT_DETAIL="failed, but not with the expected diagnostic ($pattern) - is the lesson masked by another error?"
      else
        PROJECT_STATUS="xfail"
        PROJECT_DETAIL="build fails by design (see AGENTS.md)"
      fi
    else
      PROJECT_STATUS="build_fail"
      PROJECT_DETAIL="compile or link failed"
    fi
    return
  fi

  if is_expected_build_failure "$project"; then
    PROJECT_STATUS="xpass"
    PROJECT_DETAIL="built cleanly, but AGENTS.md says it should not — has the lesson been fixed away?"
    return
  fi

  if ((BUILD_ONLY)); then
    PROJECT_STATUS="pass"
    return
  fi

  local started_ms code
  started_ms="$(now_ms)"
  run_project "$project" "$binary" "$run_log"
  code=$?
  PROJECT_DURATION_MS=$(($(now_ms) - started_ms))

  if ((code == 0)); then
    if [[ -n "$wasm_known" ]]; then
      # It was expected to fail and did not: say so rather than quietly passing.
      PROJECT_STATUS="xpass_wasm"
      PROJECT_DETAIL="expected to fail on wasm but succeeded - update wasm_expected_failure_reason"
    else
      PROJECT_STATUS="pass"
    fi
  elif [[ "$TARGET" == wasm ]] && ((code == 125)); then
    PROJECT_STATUS="crash"
    PROJECT_DETAIL="wasm trap (see run log)"
  elif ((code == 99)); then
    PROJECT_STATUS="harness"
    PROJECT_DETAIL="could not prepare or enter the run sandbox"
  elif ((code == 124 || code == 137)); then
    PROJECT_STATUS="timeout"
    PROJECT_DETAIL="no exit within ${RUN_TIMEOUT}s"
  elif ((code > 128)); then
    local signal=$((code - 128))
    PROJECT_STATUS="crash"
    PROJECT_DETAIL="killed by signal ${signal} ($(kill -l "$signal" 2>/dev/null || echo unknown))"
  else
    PROJECT_STATUS="exit"
    PROJECT_DETAIL="non-zero exit status ${code}"
  fi

  if [[ -n "$wasm_known" && "$PROJECT_STATUS" != pass && "$PROJECT_STATUS" != xpass_wasm ]]; then
    PROJECT_STATUS="xfail_wasm"
    PROJECT_DETAIL="$wasm_known"
  fi
}

# --- Main --------------------------------------------------------------------

mkdir -p "$SMOKE_DIR/bin" "$SMOKE_DIR/log" "$SMOKE_DIR/run"
readonly RESULTS_JSONL="$SMOKE_DIR/results-$TARGET.jsonl"
: >"$RESULTS_JSONL"

resolve_timeout_cmd

if [[ "$TARGET" == wasm ]]; then
  resolve_wasm_toolchain || exit 2
  resolve_wasm_runner
fi

printf '%sFN6805 smoke test%s %s(target: %s)%s\n' "$C_BLUE" "$C_RESET" "$C_DIM" "$TARGET" "$C_RESET"
if [[ "$TARGET" == wasm ]]; then
  printf '%s  compiler : %s%s\n' "$C_DIM" "$("$WASM_CXX" --version 2>/dev/null | head -1)" "$C_RESET"
  printf '%s  sysroot  : %s%s\n' "$C_DIM" "$WASM_SYSROOT" "$C_RESET"
  printf '%s  runner   : %s%s\n' "$C_DIM" "${WASM_RUN_KIND:-none (build only)}" "$C_RESET"
  ((BUILD_ONLY)) || [[ -n "$WASM_RUN_KIND" ]] || printf '%swarning: no wasmtime or node found; wasm modules are built but not run%s\n' "$C_YELLOW" "$C_RESET" >&2
else
  printf '%s  compiler : %s%s\n' "$C_DIM" "$("$CXX" --version 2>/dev/null | head -1)" "$C_RESET"
  printf '%s  flags    : %s%s\n' "$C_DIM" "$CXXFLAGS" "$C_RESET"
fi
printf '%s  scratch  : %s%s\n\n' "$C_DIM" "$SMOKE_DIR" "$C_RESET"

declare -a failures=()
pass_count=0 xfail_count=0 skip_count=0 wasm_skip_count=0

while IFS= read -r project; do
  matches_filter "$project" || { ((++skip_count)); continue; }

  binary="$SMOKE_DIR/bin/$project"
  [[ "$TARGET" == wasm ]] && binary="$binary.wasm"
  build_log="$SMOKE_DIR/log/$project.build.log"
  run_log="$SMOKE_DIR/log/$project.run.log"

  printf '%-26s ' "$project"
  smoke_one_project "$project" "$binary" "$build_log" "$run_log"

  case "$PROJECT_STATUS" in
    pass)
      ((++pass_count))
      if ((BUILD_ONLY)); then
        printf '%sBUILD OK%s\n' "$C_GREEN" "$C_RESET"
      else
        printf '%sOK%s %s(%d ms, %d lines out)%s\n' "$C_GREEN" "$C_RESET" \
          "$C_DIM" "$PROJECT_DURATION_MS" "$(wc -l <"$run_log")" "$C_RESET"
        ((VERBOSE)) && sed 's/^/    /' "$run_log"
      fi
      ;;
    xfail)
      ((++xfail_count))
      printf '%sXFAIL%s %s%s%s\n' "$C_YELLOW" "$C_RESET" "$C_DIM" "$PROJECT_DETAIL" "$C_RESET"
      ;;
    skip_wasm)
      ((++wasm_skip_count))
      printf '%sSKIP%s %s%s%s\n' "$C_BLUE" "$C_RESET" "$C_DIM" "$PROJECT_DETAIL" "$C_RESET"
      ;;
    xfail_wasm)
      ((++xfail_count))
      printf '%sXFAIL-WASM%s %s%s%s\n' "$C_YELLOW" "$C_RESET" "$C_DIM" "$PROJECT_DETAIL" "$C_RESET"
      ;;
    xpass_wasm | xfail_wrong_reason)
      failures+=("$project: $PROJECT_DETAIL")
      printf '%s%s%s %s%s%s\n' "$C_RED" "${PROJECT_STATUS^^}" "$C_RESET" "$C_DIM" "$PROJECT_DETAIL" "$C_RESET"
      ;;
    build_fail)
      failures+=("$project: $PROJECT_DETAIL")
      printf '%sBUILD FAIL%s %s%s%s\n' "$C_RED" "$C_RESET" "$C_DIM" "$build_log" "$C_RESET"
      grep -m3 -E 'error:' "$build_log" | sed 's/^/    /'
      ;;
    xpass)
      failures+=("$project: expected build failure, but it compiled")
      printf '%sXPASS%s %s%s%s\n' "$C_RED" "$C_RESET" "$C_DIM" "$PROJECT_DETAIL" "$C_RESET"
      ;;
    *)
      failures+=("$project: $PROJECT_DETAIL")
      printf '%s%s%s %s (%d ms) — %s%s%s\n' "$C_RED" "${PROJECT_STATUS^^}" "$C_RESET" \
        "$PROJECT_DETAIL" "$PROJECT_DURATION_MS" "$C_DIM" "$run_log" "$C_RESET"
      tail -n5 "$run_log" | sed 's/^/    /'
      ;;
  esac

  printf '{"event":"project_result","target":"%s","project":"%s","status":"%s","detail":"%s","duration_ms":%d,"sources":%d,"build_log":"%s","run_log":"%s"}\n' \
    "$TARGET" "$(json_escape "$project")" "$PROJECT_STATUS" "$(json_escape "$PROJECT_DETAIL")" \
    "$PROJECT_DURATION_MS" "$SOURCE_COUNT" \
    "$(json_escape "$build_log")" "$(json_escape "$run_log")" >>"$RESULTS_JSONL"
done < <(discover_projects)

# --- Summary -----------------------------------------------------------------

printf '\n%s―――%s\n' "$C_DIM" "$C_RESET"
printf 'passed %d   expected-fail %d   failed %d' "$pass_count" "$xfail_count" "${#failures[@]}"
((wasm_skip_count > 0)) && printf '   skipped-on-wasm %d' "$wasm_skip_count"
((skip_count > 0)) && printf '   filtered-out %d' "$skip_count"
printf '\n'

printf '{"event":"run_summary","target":"%s","passed":%d,"expected_fail":%d,"failed":%d,"filtered_out":%d,"skipped_on_wasm":%d}\n' \
  "$TARGET" "$pass_count" "$xfail_count" "${#failures[@]}" "$skip_count" "$wasm_skip_count" >>"$RESULTS_JSONL"

printf '%sResults: %s%s\n' "$C_DIM" "$RESULTS_JSONL" "$C_RESET"

if ((${#failures[@]} > 0)); then
  printf '\n%sFailures:%s\n' "$C_RED" "$C_RESET"
  printf '  %s\n' "${failures[@]}"
  printf 'Logs: %s/log\n' "$SMOKE_DIR"
  exit 1
fi

((KEEP)) || rm -rf "$SMOKE_DIR/run"
printf '%sAll projects compile and run without crashing (target: %s).%s\n' "$C_GREEN" "$TARGET" "$C_RESET"

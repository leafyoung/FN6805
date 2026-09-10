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
  --help, -h       show this help

Environment overrides:
  CXX              compiler (default clang++)
  CXXFLAGS         compile flags (default matches the root Makefile)
  SMOKE_DIR        scratch directory for binaries, logs, sandboxes
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

# --- Argument parsing --------------------------------------------------------

while (($# > 0)); do
  case "$1" in
    --build-only) BUILD_ONLY=1 ;;
    --verbose | -v) VERBOSE=1 ;;
    --keep | -k) KEEP=1 ;;
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
  (
    cd "$sandbox" || exit 99
    if [[ -n "$TIMEOUT_CMD" ]]; then
      "$TIMEOUT_CMD" -k 5 "$RUN_TIMEOUT" "$binary" </dev/null
    else
      "$binary" </dev/null
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

  if ! build_project "$project" "$binary" "$build_log"; then
    if is_expected_build_failure "$project"; then
      PROJECT_STATUS="xfail"
      PROJECT_DETAIL="build fails by design (see AGENTS.md)"
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
    PROJECT_STATUS="pass"
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
}

# --- Main --------------------------------------------------------------------

mkdir -p "$SMOKE_DIR/bin" "$SMOKE_DIR/log" "$SMOKE_DIR/run"
readonly RESULTS_JSONL="$SMOKE_DIR/results.jsonl"
: >"$RESULTS_JSONL"

resolve_timeout_cmd

printf '%sFN6805 smoke test%s\n' "$C_BLUE" "$C_RESET"
printf '%s  compiler : %s%s\n' "$C_DIM" "$("$CXX" --version 2>/dev/null | head -1)" "$C_RESET"
printf '%s  flags    : %s%s\n' "$C_DIM" "$CXXFLAGS" "$C_RESET"
printf '%s  scratch  : %s%s\n\n' "$C_DIM" "$SMOKE_DIR" "$C_RESET"

declare -a failures=()
pass_count=0 xfail_count=0 skip_count=0

while IFS= read -r project; do
  matches_filter "$project" || { ((++skip_count)); continue; }

  binary="$SMOKE_DIR/bin/$project"
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

  printf '{"event":"project_result","project":"%s","status":"%s","detail":"%s","duration_ms":%d,"sources":%d,"build_log":"%s","run_log":"%s"}\n' \
    "$(json_escape "$project")" "$PROJECT_STATUS" "$(json_escape "$PROJECT_DETAIL")" \
    "$PROJECT_DURATION_MS" "$SOURCE_COUNT" \
    "$(json_escape "$build_log")" "$(json_escape "$run_log")" >>"$RESULTS_JSONL"
done < <(discover_projects)

# --- Summary -----------------------------------------------------------------

printf '\n%s―――%s\n' "$C_DIM" "$C_RESET"
printf 'passed %d   expected-fail %d   failed %d' "$pass_count" "$xfail_count" "${#failures[@]}"
((skip_count > 0)) && printf '   filtered-out %d' "$skip_count"
printf '\n'

printf '{"event":"run_summary","passed":%d,"expected_fail":%d,"failed":%d,"filtered_out":%d}\n' \
  "$pass_count" "$xfail_count" "${#failures[@]}" "$skip_count" >>"$RESULTS_JSONL"

printf '%sResults: %s%s\n' "$C_DIM" "$RESULTS_JSONL" "$C_RESET"

if ((${#failures[@]} > 0)); then
  printf '\n%sFailures:%s\n' "$C_RED" "$C_RESET"
  printf '  %s\n' "${failures[@]}"
  printf 'Logs: %s/log\n' "$SMOKE_DIR"
  exit 1
fi

((KEEP)) || rm -rf "$SMOKE_DIR/run"
printf '%sAll projects compile and run without crashing.%s\n' "$C_GREEN" "$C_RESET"

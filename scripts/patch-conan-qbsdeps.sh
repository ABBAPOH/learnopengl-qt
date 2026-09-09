#!/usr/bin/env bash
# Backport conan-io/conan#20329 onto the live Conan install:
# QbsDeps must emit pkg_config_name / qbs_file_name (clipper -> polyclipping)
# so generated JSON filenames match the requires list.
#
# Conan 2.32.0 still has the bug; newer develop may already include the fix.
set -eu
set -o pipefail

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

function help() {
    cat <<EOF
usage: patch-conan-qbsdeps.sh [--dry-run]

Applies conan-io/conan#20329 to the qbsdeps.py that belongs to the \`conan\`
on PATH (string replace — more reliable than patch(1) across BSD/GNU).
EOF
}

DRY_RUN=

while [ $# -gt 0 ]; do
    case "$1" in
        --dry-run)
            DRY_RUN=1
            ;;
        --help|-h)
            help
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            help
            exit 1
            ;;
    esac
    shift
done

if ! command -v conan >/dev/null 2>&1; then
    echo "conan is not on PATH." >&2
    exit 1
fi

CONAN_BIN=$(command -v conan)
if command -v python3 >/dev/null 2>&1; then
    CONAN_BIN=$(python3 -c "import os, sys; print(os.path.realpath(sys.argv[1]))" "${CONAN_BIN}")
fi

SHEBANG=$(head -n1 "${CONAN_BIN}")
case "${SHEBANG}" in
    "#!"*)
        SHEBANG_REST=${SHEBANG#\#!}
        FIRST_WORD=${SHEBANG_REST%% *}
        case "${FIRST_WORD}" in
            */env)
                PY=${SHEBANG_REST#* }
                PY=${PY%% *}
                ;;
            *)
                PY=${FIRST_WORD}
                ;;
        esac
        ;;
    *)
        PY=python3
        ;;
esac

QBSDEPS=$("${PY}" -c "from conan.tools.qbs import qbsdeps; import inspect; print(inspect.getfile(qbsdeps))")

if [ ! -f "${QBSDEPS}" ]; then
    echo "Could not locate qbsdeps.py via ${PY}." >&2
    exit 1
fi

echo "Conan:    ${CONAN_BIN}"
echo "Python:   ${PY}"
echo "qbsdeps:  ${QBSDEPS}"

export QBSDEPS DRY_RUN
"${PY}" <<'PY'
import os
import sys
from pathlib import Path

path = Path(os.environ["QBSDEPS"])
dry_run = bool(os.environ.get("DRY_RUN"))
text = path.read_text(encoding="utf-8")

old = (
    "                for deprequire, _ in self._dep.dependencies.direct_host.items():\n"
    "                    requires.append((deprequire.ref.name, deprequire.ref.version))\n"
)
new = (
    "                # Use _get_package_name so dependency names match generated JSON\n"
    "                # filenames when pkg_config_name / qbs_file_name is set\n"
    "                # (e.g. recipe \"clipper\" -> file \"polyclipping.json\").\n"
    "                for _, dep in self._dep.dependencies.direct_host.items():\n"
    "                    requires.append((_get_package_name(dep), dep.ref.version))\n"
)

if "_get_package_name(dep)" in text and "deprequire.ref.name" not in text:
    print("Already patched / fixed upstream (conan-io/conan#20329).")
    sys.exit(0)

if old not in text:
    print(
        f"Could not find expected buggy snippet in {path}.\n"
        "qbsdeps.py may have changed; update scripts/patch-conan-qbsdeps.sh.",
        file=sys.stderr,
    )
    sys.exit(1)

if dry_run:
    print(f"Dry-run: would patch {path}")
    sys.exit(0)

backup = path.with_suffix(path.suffix + ".bak")
backup.write_text(text, encoding="utf-8")
path.write_text(text.replace(old, new, 1), encoding="utf-8")

pycache = path.parent / "__pycache__"
if pycache.is_dir():
    for pyc in pycache.glob("qbsdeps*.pyc"):
        pyc.unlink()

print(f"Patched {path}")
print(f"Backup:  {backup}")
PY

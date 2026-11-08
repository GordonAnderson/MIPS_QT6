#!/bin/bash
# Fetches the pinned Python runtime for the Device Scripts compiler into
# tools/python/macos/python/ (macOS, Apple silicon). The result is not committed
# (see .gitignore); MIPS.pro copies it into MIPS.app/Contents/MacOS/python.
#
# Usage: tools/python/fetch_python.sh
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
# shellcheck disable=SC1091
source "$HERE/versions.txt"

DEST="$HERE/macos"
STAMP="$PY_VERSION+$PBS_TAG"
if [ -x "$DEST/python/bin/python3" ] && [ "$(cat "$DEST/.version" 2>/dev/null)" = "$STAMP" ]; then
    echo "Python $PY_VERSION is already in $DEST/python"
    exit 0
fi

NAME="cpython-$PY_VERSION+$PBS_TAG-aarch64-apple-darwin-install_only_stripped.tar.gz"
URL="https://github.com/astral-sh/python-build-standalone/releases/download/$PBS_TAG/${NAME//+/%2B}"

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

echo "Downloading $NAME"
curl --fail --location --silent --show-error --output "$WORK/python.tar.gz" "$URL"

ACTUAL="$(shasum -a 256 "$WORK/python.tar.gz" | cut -d' ' -f1)"
if [ "$ACTUAL" != "$PBS_MACOS_ARM64_SHA256" ]; then
    echo "error: checksum mismatch for $NAME" >&2
    echo "  expected $PBS_MACOS_ARM64_SHA256" >&2
    echo "  got      $ACTUAL" >&2
    exit 1
fi

mkdir "$WORK/x"
tar -xzf "$WORK/python.tar.gz" -C "$WORK/x"
PY="$WORK/x/python"

# gsc.py needs only the standard library's core: drop what it never uses.
LIB="$(echo "$PY"/lib/python3.*)"
rm -rf "$PY/include" "$PY/share" "$PY/lib/pkgconfig" "$PY/lib/"*.a
rm -rf "$LIB/test" "$LIB/idlelib" "$LIB/tkinter" "$LIB/turtledemo" "$LIB/ensurepip" \
       "$LIB/pydoc_data" "$LIB/site-packages"/* "$LIB/config-"*
rm -f  "$LIB/lib-dynload/_tkinter"*.so "$LIB/turtle.py"
rm -rf "$PY/lib/tcl"* "$PY/lib/tk"* "$PY/lib/itcl"* "$PY/lib/thread"* "$PY/lib/libtcl"* "$PY/lib/libtk"*
rm -f  "$PY/bin/pip"* "$PY/bin/idle"* "$PY/bin/pydoc"* "$PY/bin/2to3"*

# The compiler must work before it is shipped.
"$PY/bin/python3" -I -B -c "import argparse, difflib, json, re, struct, sys, time; assert sys.version_info >= (3, 9)"

mkdir -p "$DEST"
rm -rf "$DEST/python"
mv "$PY" "$DEST/python"
echo "$STAMP" > "$DEST/.version"
echo "Python $PY_VERSION is in $DEST/python ($(du -sh "$DEST/python" | cut -f1))"

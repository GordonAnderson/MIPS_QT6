#!/bin/bash
# Code-signs the Python runtime inside MIPS.app (Contents/Resources/python).
#
# `codesign --deep` on the app does not reliably reach executables and libraries
# in a plain folder like this one, and notarization rejects any unsigned
# Mach-O file, so sign every Mach-O file in the runtime explicitly BEFORE
# signing the app bundle itself.
#
# Usage: tools/python/sign_macos_python.sh MIPS.app [IDENTITY]
#   IDENTITY  a Developer ID identity (hardened runtime + secure timestamp, as
#             notarization needs), or "-" (the default) for an ad-hoc signature.
set -euo pipefail

APP="${1:?usage: sign_macos_python.sh MIPS.app [IDENTITY]}"
IDENTITY="${2:--}"
PYDIR="$APP/Contents/Resources/python"

if [ ! -d "$PYDIR" ]; then
    echo "No bundled Python in $APP; nothing to sign."
    exit 0
fi

OPTS=(--force --sign "$IDENTITY" --options runtime)
if [ "$IDENTITY" = "-" ]; then OPTS+=(--timestamp=none); else OPTS+=(--timestamp); fi

count=0
while IFS= read -r -d '' f; do
    if file -b "$f" | grep -q "Mach-O"; then
        codesign "${OPTS[@]}" "$f"
        count=$((count + 1))
    fi
done < <(find "$PYDIR" -type f -print0)

echo "Signed $count Mach-O file(s) in $PYDIR"
[ "$count" -gt 0 ] || { echo "error: found nothing to sign in $PYDIR" >&2; exit 1; }

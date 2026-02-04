#!/usr/bin/env bash
set -e

APPDIR="$1"
PY_SRC="https://github.com/astral-sh/python-build-standalone/releases/download/20221106/cpython-3.10.8+20221106-x86_64-unknown-linux-gnu-install_only.tar.gz"

if [[ -z "$APPDIR" ]]; then
  echo "Usage: setup_python.sh <AppDir>"
  exit 1
fi

echo "→ Cloning kedro-umbrella"
rm -rf kedro-umbrella
git clone https://github.com/CPS-research-group/kedro-umbrella.git

mkdir -p "$APPDIR/usr"

echo "→ Extracting embedded Python"
curl -L "$PY_SRC" | tar -xz -C "$APPDIR/usr/"

echo "→ Installing Python dependencies"
"$APPDIR/usr/python/bin/python3" -m pip install --upgrade pip
"$APPDIR/usr/python/bin/python3" -m pip install \
  --extra-index-url https://download.pytorch.org/whl/cpu \
  "$(pwd)/kedro-umbrella"

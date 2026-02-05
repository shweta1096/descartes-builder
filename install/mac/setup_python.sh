#!/usr/bin/env bash
set -e

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
PY_ENV="$ROOT_DIR/install/mac/embedded-python"
PY_SOURCE="https://github.com/astral-sh/python-build-standalone/releases/download/20221106/cpython-3.10.8+20221106-aarch64-apple-darwin-install_only.tar.gz"
mkdir -p "$PY_ENV"
tar -xzf <(curl -L "$PY_SOURCE") -C "$PY_ENV"

echo "Creating Python env at $PY_ENV using $PY_SOURCE"
PYTHON_BIN="$PY_ENV/python/bin/python3"
xattr -dr com.apple.quarantine "$PY_ENV/python"
codesign --force --deep --sign - "$PYTHON_BIN" 

$PYTHON_BIN -m pip install --upgrade pip
git clone https://github.com/CPS-research-group/kedro-umbrella.git "$ROOT_DIR/install/mac/kedro-umbrella"
$PYTHON_BIN -m pip install --extra-index-url https://download.pytorch.org/whl/cpu "$ROOT_DIR/install/mac/kedro-umbrella"

echo "Python env ready"

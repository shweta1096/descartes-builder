#!/usr/bin/env bash
set -e

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
APP="$ROOT_DIR/app/build/bin/DescartesBuilder.app"
PY_ENV="$ROOT_DIR/install/mac/embedded-python"

TARGET="$APP/Contents/Resources"

echo "Embedding Python into app:"
echo "  $TARGET"

# Remove any old Python inside the app
rm -rf "$TARGET"

# Create target folder and copy venv
mkdir -p "$TARGET"
cp -R "$PY_ENV/"* "$TARGET/"

# Ensure Python executable is executable
chmod +x "$TARGET/python/bin/python3"

echo "Embedded Python successfully"

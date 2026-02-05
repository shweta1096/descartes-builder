#!/usr/bin/env bash
set -e

APP_NAME="DescartesBuilder"
APPDIR="$(pwd)/${APP_NAME}.AppDir"

if [[ ! -d "$APPDIR" ]]; then
  echo "ERROR: AppDir not found: $APPDIR"
  exit 1
fi

wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20251107-1/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage

./linuxdeploy-x86_64.AppImage \
  --appdir "$APPDIR" \
  --output appimage

mv "${APP_NAME}"-*.AppImage "${APP_NAME}.AppImage"
#!/bin/bash
set -e

APP_BUNDLE="${1}"
OUTPUT_DIR="${2:-.}"

if [ -z "$APP_BUNDLE" ] || [ ! -d "$APP_BUNDLE" ]; then
    echo "Usage: $0 <app_bundle> [output_dir]"
    echo "Example: $0 dist/PicaSim.app dist"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

# Extract version from VERSIONS.txt if it exists
VERSION="1.0.0"
if [ -f "VERSIONS.txt" ]; then
    VERSION=$(grep -o "Version [0-9.]*" VERSIONS.txt | head -1 | cut -d' ' -f2 || echo "1.0.0")
fi

APP_NAME="PicaSim"
DMG_FILE="$OUTPUT_DIR/PicaSim-${VERSION}.dmg"
TEMP_DIR="/tmp/picasim_dmg"

echo "Creating DMG for PicaSim v$VERSION..."
rm -rf "$TEMP_DIR" 2>/dev/null || true
mkdir -p "$TEMP_DIR"

cp -r "$APP_BUNDLE" "$TEMP_DIR/"
[ -f "LICENSE.txt" ] && cp LICENSE.txt "$TEMP_DIR/" || true
[ -f "README.md" ] && cp README.md "$TEMP_DIR/" || true
ln -s /Applications "$TEMP_DIR/Applications"

[ -f "$DMG_FILE" ] && rm -f "$DMG_FILE"

hdiutil create -volname "$APP_NAME" \
    -srcfolder "$TEMP_DIR" \
    -ov \
    -format UDZO \
    "$DMG_FILE"

rm -rf "$TEMP_DIR"

DMG_SIZE=$(du -h "$DMG_FILE" | awk '{print $1}')
echo "✓ DMG created: $DMG_FILE ($DMG_SIZE)"

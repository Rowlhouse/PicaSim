#!/bin/bash
# Script to create a .app bundle of PicaSim for macOS. 
# It packages only the architecture built on the current machine; the 
# universal merge happens in the GitHub Action workflow.
# Usage: ./macos_create_app_bundle.sh [source_dir] [output_dir]

set -e

# Source directory (where the executable and installed data are located)
SOURCE_DIR="${1:-.}"
if [ ! -d "$SOURCE_DIR" ] || [ ! -f "$SOURCE_DIR/PicaSim" ]; then
    echo "Error: Invalid source directory or PicaSim not found"
    echo "Usage: $0 <source_dir> [output_dir]"
    echo "Example: $0 ./dist/PicaSim-1_0_1 ./dist"
    exit 1
fi

# Output directory (where the .app will be created)
OUTPUT_DIR="${2:-dist}"
mkdir -p "$OUTPUT_DIR"

# Determine version from VERSIONS.txt if present
VERSION="1.0.0"
if [ -f "$SOURCE_DIR/VERSIONS.txt" ]; then
    VERSION=$(grep -oP 'Version \K[0-9.]+' "$SOURCE_DIR/VERSIONS.txt" || echo "1.0.0")
fi

APP_NAME="PicaSim"
APP_BUNDLE="$OUTPUT_DIR/$APP_NAME.app"
CONTENTS="$APP_BUNDLE/Contents"
MACOS_DIR="$CONTENTS/MacOS"
RESOURCES_DIR="$CONTENTS/Resources"

echo "Creating macOS bundle for PicaSim v$VERSION..."
echo "Source: $SOURCE_DIR"
echo "Output: $APP_BUNDLE"

# Clean up if already exists
if [ -d "$APP_BUNDLE" ]; then
    echo "Removing previous bundle..."
    rm -rf "$APP_BUNDLE"
fi

# Create directory structure
mkdir -p "$MACOS_DIR"
mkdir -p "$RESOURCES_DIR"

# Copy the executable
echo "Copying executable..."
cp "$SOURCE_DIR/PicaSim" "$MACOS_DIR/PicaSim"
chmod +x "$MACOS_DIR/PicaSim"

# Copy data to Resources
echo "Copying data..."
cp -r "$SOURCE_DIR/SystemData" "$RESOURCES_DIR/"
cp -r "$SOURCE_DIR/SystemSettings" "$RESOURCES_DIR/"
cp -r "$SOURCE_DIR/Menus" "$RESOURCES_DIR/"
cp -r "$SOURCE_DIR/Fonts" "$RESOURCES_DIR/"
if [ -f "$SOURCE_DIR/LICENSE.txt" ]; then
    cp "$SOURCE_DIR/LICENSE.txt" "$RESOURCES_DIR/"
fi
if [ -f "$SOURCE_DIR/VERSIONS.txt" ]; then
    cp "$SOURCE_DIR/VERSIONS.txt" "$RESOURCES_DIR/"
fi

# Copy app icon if available
if [ -f "data/SystemData/Icons/PicaSim.icns" ]; then
    echo "Copying app icon..."
    cp "data/SystemData/Icons/PicaSim.icns" "$RESOURCES_DIR/"
fi

# Create launcher script that changes directory and runs the app
echo "Creating launcher script..."
cat > "$MACOS_DIR/$APP_NAME-launcher" << 'EOF'
#!/bin/bash
# Launcher script for PicaSim
# Changes directory to Resources (where it expects to find data) and runs the app

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
APP_DIR="$SCRIPT_DIR/../Resources"

cd "$APP_DIR"
exec "$SCRIPT_DIR/PicaSim" "$@"
EOF

chmod +x "$MACOS_DIR/$APP_NAME-launcher"

# Create Info.plist
echo "Creating Info.plist..."
cat > "$CONTENTS/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>it</string>
    <key>CFBundleExecutable</key>
    <string>$APP_NAME-launcher</string>
    <key>CFBundleIconFile</key>
    <string>PicaSim</string>
    <key>CFBundleIdentifier</key>
    <string>com.peterchristian.picasim</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>$VERSION</string>
    <key>CFBundleVersion</key>
    <string>$VERSION</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.13</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSHumanReadableCopyright</key>
    <string>Copyright © 2024</string>
</dict>
</plist>
EOF

# Create PkgInfo (optional but recommended)
echo "APPL????" > "$CONTENTS/PkgInfo"

echo "✓ Bundle created successfully!"
echo "You can launch the app with: open $APP_BUNDLE"
echo "Or double-click $APP_BUNDLE in Finder"

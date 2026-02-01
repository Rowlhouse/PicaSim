#!/bin/bash
# Helper for macOS signing + notarization via CLI, with Xcode project hint.
# - Assumes local single-arch build in dist/PicaSim.app (universal merge is done in GitHub Actions).
# - Requires Developer ID certs already created via Xcode; paid account needed.

# Guard: require Developer ID identity name in env
if [ -z "${PICASIM_MACOS_CERT:-}" ]; then
    echo "This script is for Apple Developer accounts only. Set PICASIM_MACOS_CERT to your 'Developer ID Application: ...' identity and rerun."
    exit 1
fi

# (Optional) Download universal app from GitHub Actions artifacts
# (...)

# Check certs in keychain; need "Developer ID Application"
security find-identity -v -p codesigning

# Sign .app
codesign --force --deep --options runtime --timestamp --sign "$PICASIM_MACOS_CERT" dist/PicaSim.app

# Verify .app signature
codesign --verify --deep --strict --verbose=2 dist/PicaSim.app

# Check Gatekeeper assessment for the app
spctl --assess --type open --context context:primary-signature --verbose dist/PicaSim.app

# Create DMG from app
./macos_create_dmg.sh dist/PicaSim.app dist

# Prereq to notarize via CLI: credential profile in keychain (one-time)
# xcrun notarytool store-credentials "picasim-profile"

# Submit DMG for notarization
xcrun notarytool submit dist/PicaSim-1.0.1.dmg --keychain-profile "picasim-profile" --wait

# Staple ticket to DMG
xcrun stapler staple dist/PicaSim-1.0.1.dmg

# Optionally staple the app for standalone testing, though DMG is the distributed artifact
xcrun stapler staple dist/PicaSim.app

# Final Gatekeeper check on the app
spctl --assess --type open --context context:primary-signature --verbose dist/PicaSim.app

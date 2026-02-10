#!/usr/bin/env python3
"""
Generate assets_manifest.txt listing all files under data/ directory.
This manifest is bundled in the APK and used by AndroidAssets to extract
files to internal storage on first launch.
"""

import os
import sys

def generate_manifest(data_dir, output_file):
    """Walk data_dir and write all relative file paths to output_file."""
    count = 0
    with open(output_file, 'w', newline='\n') as f:
        for root, dirs, files in os.walk(data_dir):
            # Skip directories that shouldn't be in the APK
            dirs[:] = [d for d in dirs if d not in ('UserData', 'UserSettings', '.git')]
            for filename in sorted(files):
                filepath = os.path.join(root, filename)
                relpath = os.path.relpath(filepath, data_dir)
                # Normalize to forward slashes
                relpath = relpath.replace('\\', '/')
                f.write(relpath + '\n')
                count += 1
    print(f"Generated manifest with {count} files: {output_file}")

if __name__ == '__main__':
    if len(sys.argv) >= 3:
        data_dir = sys.argv[1]
        output_file = sys.argv[2]
    else:
        # Default: run from project root
        script_dir = os.path.dirname(os.path.abspath(__file__))
        project_dir = os.path.dirname(script_dir)
        data_dir = os.path.join(project_dir, 'data')
        output_file = os.path.join(data_dir, 'assets_manifest.txt')

    if not os.path.isdir(data_dir):
        print(f"Error: data directory not found: {data_dir}", file=sys.stderr)
        sys.exit(1)

    generate_manifest(data_dir, output_file)

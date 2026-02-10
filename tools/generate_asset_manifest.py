#!/usr/bin/env python3
"""
Generate assets_manifest.txt listing all files under data/ directory.
This manifest is bundled in the APK and used by AndroidAssets to extract
files to internal storage on first launch.
"""

import os
import sys


def parse_excludes(excludes_file):
    """Parse packaging_excludes.txt into excluded paths and xml-only paths."""
    excluded_paths = []
    xml_only_paths = []
    with open(excludes_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            if line.startswith('xml-only:'):
                xml_only_paths.append(line[len('xml-only:'):])
            else:
                excluded_paths.append(line)
    return excluded_paths, xml_only_paths


def is_excluded(relpath, excluded_paths, xml_only_paths):
    """Check if a relative path should be excluded from the manifest."""
    # Normalize to forward slashes for comparison
    normalized = relpath.replace('\\', '/')
    for exc in excluded_paths:
        # Match as exact path, directory prefix, or file prefix (e.g.
        # "Aeroplane/Ahi" matches both "Aeroplane/Ahi/" and "Aeroplane/Ahi.xml")
        if normalized == exc or normalized.startswith(exc + '/') or \
           normalized.startswith(exc + '.'):
            return True
    for xo in xml_only_paths:
        if normalized.startswith(xo + '/'):
            if not normalized.lower().endswith('.xml'):
                return True
    return False


def generate_manifest(data_dir, output_file, excludes_file=None):
    """Walk data_dir and write all relative file paths to output_file."""
    excluded_paths = []
    xml_only_paths = []
    if excludes_file and os.path.isfile(excludes_file):
        excluded_paths, xml_only_paths = parse_excludes(excludes_file)
        print(f"Loaded {len(excluded_paths)} exclusions and "
              f"{len(xml_only_paths)} xml-only rules from {excludes_file}")

    count = 0
    excluded_count = 0
    with open(output_file, 'w', newline='\n') as f:
        for root, dirs, files in os.walk(data_dir):
            # Skip directories that shouldn't be in the APK
            dirs[:] = [d for d in dirs if d not in ('UserData', 'UserSettings', '.git')]
            for filename in sorted(files):
                filepath = os.path.join(root, filename)
                relpath = os.path.relpath(filepath, data_dir)
                # Normalize to forward slashes
                relpath = relpath.replace('\\', '/')
                if is_excluded(relpath, excluded_paths, xml_only_paths):
                    excluded_count += 1
                    continue
                f.write(relpath + '\n')
                count += 1
    print(f"Generated manifest with {count} files "
          f"({excluded_count} excluded): {output_file}")


if __name__ == '__main__':
    if len(sys.argv) >= 3:
        data_dir = sys.argv[1]
        output_file = sys.argv[2]
        excludes_file = sys.argv[3] if len(sys.argv) >= 4 else None
    else:
        # Default: run from project root
        script_dir = os.path.dirname(os.path.abspath(__file__))
        project_dir = os.path.dirname(script_dir)
        data_dir = os.path.join(project_dir, 'data')
        output_file = os.path.join(data_dir, 'assets_manifest.txt')
        excludes_file = os.path.join(project_dir, 'packaging_excludes.txt')

    if not os.path.isdir(data_dir):
        print(f"Error: data directory not found: {data_dir}", file=sys.stderr)
        sys.exit(1)

    generate_manifest(data_dir, output_file, excludes_file)

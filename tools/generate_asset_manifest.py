#!/usr/bin/env python3
"""
Generate assets_manifest.txt listing all files under data/ directory.
This manifest is bundled in the APK and used by AndroidAssets to extract
files to internal storage on first launch.

It also writes assets_version.txt, a digest of the paths and contents of all
the packaged files. AndroidAssets re-extracts whenever it changes, so data
changes reach the device even when the version number hasn't changed.
"""

import hashlib
import os
import shutil
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


def copy_versions_file(data_dir):
    """Copy the project's VERSIONS.txt into data_dir, so it is packaged with the data.

    PicaSim reads it at runtime (What's New, Help > Versions). CMake copies it into
    data/ after desktop builds, but Android builds don't run that step.
    """
    project_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    source = os.path.join(project_dir, 'VERSIONS.txt')
    if os.path.isfile(source):
        shutil.copyfile(source, os.path.join(data_dir, 'VERSIONS.txt'))


def generate_manifest(data_dir, output_file, excludes_file=None):
    """Write the asset list, and a digest that changes with the asset contents."""
    excluded_paths = []
    xml_only_paths = []
    if excludes_file and os.path.isfile(excludes_file):
        excluded_paths, xml_only_paths = parse_excludes(excludes_file)
        print(f"Loaded {len(excluded_paths)} exclusions and "
              f"{len(xml_only_paths)} xml-only rules from {excludes_file}")

    copy_versions_file(data_dir)

    output_path = os.path.abspath(output_file)
    version_file = os.path.join(data_dir, 'assets_version.txt')
    included_files = []
    excluded_count = 0
    for root, dirs, files in os.walk(data_dir):
        # Skip directories that shouldn't be in the APK. Sorted so the digest doesn't
        # depend on the order the filesystem returns them in.
        dirs[:] = sorted(d for d in dirs if d not in ('UserData', 'UserSettings', '.git'))
        for filename in sorted(files):
            filepath = os.path.join(root, filename)
            # The manifest and digest are generated here, so leave them out of the digest
            if os.path.abspath(filepath) in (output_path, os.path.abspath(version_file)):
                continue
            # Normalize to forward slashes
            relpath = os.path.relpath(filepath, data_dir).replace('\\', '/')
            if is_excluded(relpath, excluded_paths, xml_only_paths):
                excluded_count += 1
                continue
            included_files.append((relpath, filepath))

    digest = hashlib.sha256()
    for relpath, filepath in included_files:
        digest.update(relpath.encode('utf-8'))
        digest.update(b'\0')
        with open(filepath, 'rb') as asset:
            for chunk in iter(lambda: asset.read(1024 * 1024), b''):
                digest.update(chunk)
    with open(version_file, 'w', newline='\n') as f:
        f.write(digest.hexdigest() + '\n')

    manifest_paths = sorted([relpath for relpath, _ in included_files] + ['assets_version.txt'])
    with open(output_file, 'w', newline='\n') as f:
        for relpath in manifest_paths:
            f.write(relpath + '\n')
    print(f"Generated manifest with {len(manifest_paths)} files "
          f"({excluded_count} excluded): {output_file}")
    print(f"Asset digest {digest.hexdigest()[:16]}...: {version_file}")


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

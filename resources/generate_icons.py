"""Generate Windows .ico and Android mipmap icons from source PNG."""

import os
import sys

try:
    from PIL import Image
except ImportError:
    print("Pillow not found, installing...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "Pillow"])
    from PIL import Image

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.dirname(SCRIPT_DIR)
SOURCE_IMAGE = os.path.join(SCRIPT_DIR, "IconFree.png")

# Windows .ico sizes
ICO_SIZES = [256, 128, 64, 48, 32, 16]

# Android mipmap densities
ANDROID_MIPMAPS = {
    "mipmap-mdpi": 48,
    "mipmap-hdpi": 72,
    "mipmap-xhdpi": 96,
    "mipmap-xxhdpi": 144,
    "mipmap-xxxhdpi": 192,
}


def generate_windows_ico(source: Image.Image) -> None:
    ico_path = os.path.join(SCRIPT_DIR, "PicaSim.ico")
    sizes = []
    for s in ICO_SIZES:
        resized = source.resize((s, s), Image.LANCZOS)
        sizes.append(resized)
    # Save as .ico with all sizes
    sizes[0].save(ico_path, format="ICO", sizes=[(s, s) for s in ICO_SIZES],
                  append_images=sizes[1:])
    print(f"Created {ico_path} with sizes: {ICO_SIZES}")


def generate_android_mipmaps(source: Image.Image) -> None:
    res_dir = os.path.join(PROJECT_DIR, "android", "app", "src", "main", "res")
    for density, size in ANDROID_MIPMAPS.items():
        out_dir = os.path.join(res_dir, density)
        os.makedirs(out_dir, exist_ok=True)
        out_path = os.path.join(out_dir, "ic_launcher.png")
        resized = source.resize((size, size), Image.LANCZOS)
        resized.save(out_path, format="PNG")
        print(f"Created {out_path} ({size}x{size})")


def main() -> None:
    if not os.path.exists(SOURCE_IMAGE):
        print(f"Error: Source image not found: {SOURCE_IMAGE}")
        sys.exit(1)

    source = Image.open(SOURCE_IMAGE).convert("RGBA")
    print(f"Source image: {source.size[0]}x{source.size[1]} {source.mode}")

    generate_windows_ico(source)
    generate_android_mipmaps(source)
    print("Done!")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Generate Windows and Android icons from source images.

Source images (in AndroidIcon/):
  ic_launcher_foreground.png - plane on transparent background (432x432)
  ic_launcher_background.png - blue gradient background (432x432)

Generates:
  Android: adaptive icon layers at all mipmap densities
  Windows: composited multi-size ICO at resources/PicaSim.ico
"""

import os
from PIL import Image, ImageDraw

script_dir = os.path.dirname(os.path.abspath(__file__))
project_dir = os.path.dirname(script_dir)
source_dir = os.path.join(script_dir, "AndroidIcon")

fg = Image.open(os.path.join(source_dir, "ic_launcher_foreground.png"))
bg = Image.open(os.path.join(source_dir, "ic_launcher_background.png"))

# --- Android adaptive icon layers ---
print("Android:")
ANDROID_SIZES = {
    "mipmap-mdpi": 108,
    "mipmap-hdpi": 162,
    "mipmap-xhdpi": 216,
    "mipmap-xxhdpi": 324,
    "mipmap-xxxhdpi": 432,
}
res_dir = os.path.join(project_dir, "android", "app", "src", "main", "res")

for name, src in (("ic_launcher_foreground", fg), ("ic_launcher_background", bg)):
    for folder, size in ANDROID_SIZES.items():
        out_dir = os.path.join(res_dir, folder)
        os.makedirs(out_dir, exist_ok=True)
        resized = src.resize((size, size), Image.LANCZOS)
        resized.save(os.path.join(out_dir, f"{name}.png"))
        print(f"  {folder}/{name}.png ({size}x{size})")

# --- Windows ICO (composited with rounded corners) ---
print("Windows:")
ICO_SIZES = [256, 128, 64, 48, 32, 16]


def make_rounded_mask(size, radius):
    """Create a rounded rectangle alpha mask."""
    mask = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(mask)
    draw.rounded_rectangle([0, 0, size - 1, size - 1], radius=radius, fill=255)
    return mask


ico_images = []
for size in ICO_SIZES:
    # Composite foreground onto background
    bg_resized = bg.resize((size, size), Image.LANCZOS).convert("RGBA")
    fg_resized = fg.resize((size, size), Image.LANCZOS).convert("RGBA")
    composited = Image.alpha_composite(bg_resized, fg_resized)
    # Apply rounded corners
    radius = max(size // 8, 2)
    mask = make_rounded_mask(size, radius)
    composited.putalpha(mask)
    ico_images.append(composited)
    print(f"  {size}x{size}")

ico_path = os.path.join(script_dir, "PicaSim.ico")
ico_images[0].save(ico_path, format="ICO", sizes=[(s, s) for s in ICO_SIZES],
                    append_images=ico_images[1:])
print(f"  -> {ico_path}")

print("Done")

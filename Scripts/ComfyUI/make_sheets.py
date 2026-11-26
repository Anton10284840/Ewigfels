#!/usr/bin/env python3
"""Ewigfels – setzt generierte Bilder zu Übersichtsblättern zusammen.

- Moodboards: alle Bilder einer Region als Raster  -> art/concepts/moodboards/<Region>/_SHEET.jpg
- Figuren:    die 6 Emotionen als Ausdrucks-Sheet -> art/concepts/characters/<Figur>/_EXPRESSIONS.jpg
              (+ Porträt/Ganzkörper als _SHEET.jpg)

Braucht Pillow. Am einfachsten mit dem Python von ComfyUI Portable (hat Pillow dabei):
    D:\\AI\\ComfyUI_windows_portable\\python_embeded\\python.exe Scripts\\ComfyUI\\make_sheets.py
Optional: --variant 1 (nimmt _v1 statt _v0), --root <Ordner>.
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

try:
    from PIL import Image, ImageDraw
except ImportError:
    sys.exit("Pillow fehlt: 'pip install pillow' oder das Python von ComfyUI Portable verwenden.")

REPO = Path(__file__).resolve().parent.parent.parent
EMOTIONS = ["happy", "sad", "angry", "afraid", "surprised", "disgusted"]


def grid(paths: list[Path], cols: int, cell_w: int, out: Path, labels: bool = True) -> None:
    imgs = [Image.open(p).convert("RGB") for p in paths]
    cell_h = max(int(cell_w * im.height / im.width) for im in imgs)
    rows = (len(imgs) + cols - 1) // cols
    pad, label_h = 12, 28 if labels else 0
    sheet = Image.new("RGB", (cols * (cell_w + pad) + pad, rows * (cell_h + pad + label_h) + pad), (18, 18, 20))
    draw = ImageDraw.Draw(sheet)
    for i, (im, p) in enumerate(zip(imgs, paths)):
        im = im.resize((cell_w, int(cell_w * im.height / im.width)), Image.LANCZOS)
        x = pad + (i % cols) * (cell_w + pad)
        y = pad + (i // cols) * (cell_h + pad + label_h)
        sheet.paste(im, (x, y))
        if labels:
            draw.text((x + 4, y + im.height + 6), p.stem, fill=(220, 210, 190))
    sheet.save(out, quality=90)
    print(f"  ✓ {out.relative_to(REPO) if out.is_relative_to(REPO) else out}")


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(errors="replace")  # Windows-Konsole ohne UTF-8
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=str(REPO / "art" / "concepts"))
    ap.add_argument("--variant", type=int, default=0)
    args = ap.parse_args()
    root = Path(args.root)
    suffix = f"_v{args.variant}.png"

    for folder in sorted((root / "moodboards").glob("*/")):
        shots = sorted(p for p in folder.glob(f"*{suffix}"))
        if shots:
            grid(shots, cols=4, cell_w=480, out=folder / "_SHEET.jpg")

    for folder in sorted((root / "characters").glob("*/")):
        emo = [p for e in EMOTIONS for p in folder.glob(f"*_emotion_{e}{suffix}")]
        if emo:
            grid(emo, cols=3, cell_w=420, out=folder / "_EXPRESSIONS.jpg")
        main_shots = [p for k in ("portrait", "fullbody") for p in folder.glob(f"*_{k}{suffix}")]
        if main_shots:
            grid(main_shots, cols=2, cell_w=520, out=folder / "_SHEET.jpg")

    for sub in ("creatures", "houses"):
        for folder in sorted((root / sub).glob("*/")):
            shots = sorted(folder.glob(f"*{suffix}"))
            if len(shots) > 1:
                grid(shots, cols=2, cell_w=560, out=folder / "_SHEET.jpg")
    return 0


if __name__ == "__main__":
    sys.exit(main())

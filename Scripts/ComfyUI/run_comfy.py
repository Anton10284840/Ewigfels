#!/usr/bin/env python3
"""Ewigfels – Batch-Generierung von Concept Art über die lokale ComfyUI-API.

Liest Prompt-Listen (prompts/*.json), schickt jeden Job an ComfyUI (http://127.0.0.1:8188),
lädt die Bilder nach art/concepts/<folder>/ herunter und schreibt dort ein manifest.json
(Prompt, Seed, Modell, Datum) – damit ist jedes Bild als KI-generiert nachvollziehbar.

Nur Standardbibliothek. Beispiele (im Repo-Ordner):
    python Scripts/ComfyUI/run_comfy.py --list moodboards --dry-run
    python Scripts/ComfyUI/run_comfy.py --list moodboards --only R01
    python Scripts/ComfyUI/run_comfy.py --list characters --only C02_Lio --variants 3
    python Scripts/ComfyUI/run_comfy.py --list all
Bereits vorhandene Bilder werden übersprungen (Abbruch und Neustart sind also gefahrlos).
"""
from __future__ import annotations

import argparse
import copy
import datetime as dt
import hashlib
import json
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
import uuid
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent.parent
LISTS = ["moodboards", "characters", "creatures", "houses"]
MODELS = {
    "flux": HERE / "workflows" / "WF_Concept_FluxSchnell_api.json",
    "sdxl": HERE / "workflows" / "WF_Concept_SDXL_api.json",
}
# Knoten-IDs in unseren API-Workflows (siehe workflows/*.json)
N_CKPT, N_POS, N_NEG, N_LATENT, N_SAMPLER, N_SAVE = "4", "6", "7", "5", "3", "9"
DEFAULT_NEGATIVE = (
    "cartoon, anime, illustration, lowres, blurry, oversaturated, plastic skin, "
    "deformed hands, extra fingers, text, watermark, logo, signature"
)


def stable_seed(text: str) -> int:
    """Gleicher Text = gleicher Seed (reproduzierbar über Rechner hinweg)."""
    return int(hashlib.sha256(text.encode("utf-8")).hexdigest()[:12], 16) % (2**32)


def expand_jobs(data: dict, list_name: str) -> list[dict]:
    """Macht aus einer Prompt-Liste eine flache Job-Liste."""
    style = data.get("style", "")
    defaults = data.get("defaults", {})
    templates = data.get("templates", {})
    jobs = []
    for group in data["groups"]:
        gid = group["id"]
        desc = group.get("desc", "")
        context = group.get("context", "")
        shots = list(group.get("shots", []))
        for tname in group.get("use_templates", data.get("use_templates", [])):
            t = templates[tname]
            shots.append({"id": tname, "prompt": t["prompt"], **{k: v for k, v in t.items() if k != "prompt"}})
        for shot in shots:
            sid = f"{gid}_{shot['id']}"
            prompt = shot["prompt"].replace("{desc}", desc).replace("{context}", context)
            if context and "{context}" not in shot["prompt"]:
                prompt = f"{prompt}, {context}"
            if style:
                prompt = f"{prompt}, {style}"
            # Figuren: ein Seed pro Gruppe (gleiches Gesicht über alle Emotionen), sonst pro Bild
            seed = shot.get("seed", group.get("seed", stable_seed(gid if group.get("shared_seed") else sid)))
            jobs.append({
                "id": sid,
                "list": list_name,
                "folder": group.get("folder", f"{list_name}/{gid}"),
                "prompt": prompt,
                "negative": shot.get("negative", data.get("negative", DEFAULT_NEGATIVE)),
                "width": shot.get("width", group.get("width", defaults.get("width", 1344))),
                "height": shot.get("height", group.get("height", defaults.get("height", 768))),
                "seed": int(seed),
            })
    return jobs


def build_workflow(template: dict, job: dict, model: str, variant: int, ckpt: str | None) -> dict:
    wf = copy.deepcopy(template)
    wf[N_POS]["inputs"]["text"] = job["prompt"]
    if model == "sdxl":
        wf[N_NEG]["inputs"]["text"] = job["negative"]
    wf[N_LATENT]["inputs"]["width"] = job["width"]
    wf[N_LATENT]["inputs"]["height"] = job["height"]
    wf[N_SAMPLER]["inputs"]["seed"] = (job["seed"] + variant * 1009) % (2**32)
    wf[N_SAVE]["inputs"]["filename_prefix"] = f"ewigfels/{job['folder']}/{job['id']}_v{variant}"
    if ckpt:
        wf[N_CKPT]["inputs"]["ckpt_name"] = ckpt
    return wf


class Comfy:
    def __init__(self, server: str, timeout: int):
        self.server = server.rstrip("/")
        self.timeout = timeout
        self.client_id = str(uuid.uuid4())

    def _req(self, path: str, payload: dict | None = None) -> bytes:
        data = json.dumps(payload).encode("utf-8") if payload is not None else None
        req = urllib.request.Request(self.server + path, data=data,
                                     headers={"Content-Type": "application/json"} if data else {})
        with urllib.request.urlopen(req, timeout=30) as r:
            return r.read()

    def check(self) -> None:
        try:
            self._req("/system_stats")
        except (urllib.error.URLError, OSError) as e:
            sys.exit(f"ComfyUI nicht erreichbar unter {self.server} ({e}). Läuft run_nvidia_gpu.bat?")

    def run(self, wf: dict) -> list[dict]:
        try:
            res = json.loads(self._req("/prompt", {"prompt": wf, "client_id": self.client_id}))
        except urllib.error.HTTPError as e:
            raise RuntimeError(f"ComfyUI lehnt den Workflow ab: {e.read().decode('utf-8', 'replace')[:500]}")
        pid = res["prompt_id"]
        start = time.time()
        while time.time() - start < self.timeout:
            hist = json.loads(self._req(f"/history/{pid}"))
            if pid in hist:
                entry = hist[pid]
                status = entry.get("status", {})
                if status.get("status_str") == "error":
                    raise RuntimeError(f"Fehler in ComfyUI: {status.get('messages')}")
                imgs = []
                for out in entry.get("outputs", {}).values():
                    imgs.extend(out.get("images", []))
                if imgs:
                    return imgs
            time.sleep(1.0)
        raise TimeoutError(f"Kein Ergebnis nach {self.timeout} s")

    def download(self, img: dict) -> bytes:
        q = urllib.parse.urlencode({"filename": img["filename"], "subfolder": img.get("subfolder", ""),
                                    "type": img.get("type", "output")})
        return self._req(f"/view?{q}")


def load_manifest(path: Path) -> dict:
    if path.exists():
        return json.loads(path.read_text(encoding="utf-8"))
    return {"_hinweis": "KI-generiert (Referenz, kein Spiel-Asset). Modell und Lizenz siehe CREDITS.md.", "images": {}}


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(errors="replace")  # Windows-Konsole ohne UTF-8
    ap = argparse.ArgumentParser(description="Ewigfels Concept-Art-Batch für ComfyUI")
    ap.add_argument("--list", default="moodboards", help=f"{', '.join(LISTS)} oder all")
    ap.add_argument("--only", default="", help="nur Jobs, deren ID damit beginnt (Komma-getrennt möglich)")
    ap.add_argument("--variants", type=int, default=1, help="Bilder pro Job mit verschiedenen Seeds")
    ap.add_argument("--model", choices=sorted(MODELS), default="flux")
    ap.add_argument("--ckpt", default=None, help="anderer Checkpoint-Dateiname als im Workflow")
    ap.add_argument("--server", default="http://127.0.0.1:8188")
    ap.add_argument("--out", default=str(REPO / "art" / "concepts"))
    ap.add_argument("--timeout", type=int, default=600, help="Sekunden pro Bild")
    ap.add_argument("--dry-run", action="store_true", help="nur anzeigen, was erzeugt würde")
    args = ap.parse_args()

    names = LISTS if args.list == "all" else [n.strip() for n in args.list.split(",")]
    jobs = []
    for name in names:
        path = HERE / "prompts" / f"{name}.json"
        if not path.exists():
            sys.exit(f"Prompt-Liste fehlt: {path}")
        jobs += expand_jobs(json.loads(path.read_text(encoding="utf-8")), name)
    if args.only:
        prefixes = [p.strip() for p in args.only.split(",") if p.strip()]
        jobs = [j for j in jobs if any(j["id"].startswith(p) for p in prefixes)]
    if not jobs:
        sys.exit("Keine Jobs gefunden (Filter --only prüfen).")

    out_root = Path(args.out)
    total = len(jobs) * args.variants
    print(f"{len(jobs)} Jobs × {args.variants} Variante(n) = {total} Bilder, Modell {args.model}")
    if args.dry_run:
        for j in jobs:
            print(f"  {j['id']:<40} {j['width']}x{j['height']} seed={j['seed']}\n      {j['prompt'][:140]}…")
        return 0

    template = json.loads(MODELS[args.model].read_text(encoding="utf-8"))
    comfy = Comfy(args.server, args.timeout)
    comfy.check()
    done = skipped = failed = 0
    t0 = time.time()
    for j in jobs:
        folder = out_root / j["folder"]
        folder.mkdir(parents=True, exist_ok=True)
        mpath = folder / "manifest.json"
        manifest = load_manifest(mpath)
        for v in range(args.variants):
            fname = f"{j['id']}_v{v}.png"
            target = folder / fname
            if target.exists():
                skipped += 1
                continue
            wf = build_workflow(template, j, args.model, v, args.ckpt)
            try:
                imgs = comfy.run(wf)
                target.write_bytes(comfy.download(imgs[0]))
            except Exception as e:  # weiter mit dem nächsten Bild
                failed += 1
                print(f"  ✗ {fname}: {e}")
                continue
            manifest["images"][fname] = {
                "prompt": j["prompt"],
                "negative": j["negative"] if args.model == "sdxl" else "",
                "seed": wf[N_SAMPLER]["inputs"]["seed"],
                "size": [j["width"], j["height"]],
                "model": wf[N_CKPT]["inputs"]["ckpt_name"],
                "steps": wf[N_SAMPLER]["inputs"]["steps"],
                "cfg": wf[N_SAMPLER]["inputs"]["cfg"],
                "created": dt.datetime.now().isoformat(timespec="seconds"),
            }
            mpath.write_text(json.dumps(manifest, ensure_ascii=False, indent=2), encoding="utf-8")
            done += 1
            rate = (time.time() - t0) / done
            rest = total - done - skipped - failed
            print(f"  ✓ {fname}  ({done + skipped + failed}/{total}, noch ca. {rest * rate / 60:.0f} min)")
    print(f"Fertig: {done} neu, {skipped} übersprungen, {failed} Fehler. Ordner: {out_root}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())

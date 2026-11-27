#!/usr/bin/env python3
"""
Prüfskript für den Charakter-Editor-Katalog (Nocturna / Ewigfels).

Liest Content/Data/Appearance/*.json und prüft dieselben Regeln wie der C++-Lader
(EwigfelsAppearanceTypes.cpp, ParseCatalogJson + Sanitize):
  - gültiges JSON, Pflichtfelder, doppelte IDs, Name in "de" und "en"
  - unlock: start | shop | flag:<Flag>  (Flag muss in Content/Data/World/WorldState.json stehen)
  - Kleidung: Slot bekannt, jeder Slot hat mindestens ein "start"-Stück
  - jede Kategorie hat mindestens eine "start"-Option (sonst ist der Editor leer)
  - Regler: Gruppe "koerper" <=> id beginnt mit "koerper_" (Laden-Bereiche hängen daran), Morph-Namen gesetzt
  - Farben als #RRGGBB, Asset-Pfade beginnen mit /Game/
  - defaults zeigen nur auf "start"-Optionen, alle Slots belegt
  - Läden: Bereiche gültig, Preise >= 0
Zeigt am Ende eine kurze Übersicht (wie viel es im Editor und in Läden gibt).

Aufruf (aus dem Projektordner):
  python Scripts/Tools/validate_appearance.py           # Bericht, Exit-Code 1 bei Fehlern
  python Scripts/Tools/validate_appearance.py --strict  # Warnungen zählen als Fehler
Nur Python-Standardbibliothek.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DATA = ROOT / "Content" / "Data" / "Appearance"
WORLD = ROOT / "Content" / "Data" / "World" / "WorldState.json"

LISTS = {
    "body_types": "body", "face_presets": "face", "hair_styles": "hair", "beards": "beard", "brows": "brows",
    "marks": "marks", "voices": "voice", "outfit": "outfit", "patterns": "pattern", "fabrics": "fabric",
    "ring_metals": "metal",
}
MARK_KINDS = {"freckles", "scar", "makeup", "tattoo", "paint", "rune"}
PALETTES = {"skin", "hair", "eyes", "dye", "quartz", "makeup"}
AREAS = {"name", "body", "face", "skin", "eyes", "hair", "beard", "brows", "marks", "voice", "outfit", "outfit_colors", "rings"}
SLIDER_GROUPS = {"kopf", "brauen", "augen", "nase", "wangen", "mund", "kiefer", "ohren", "koerper"}
COLOR_RE = re.compile(r"^#[0-9A-Fa-f]{6}$")
ID_RE = re.compile(r"^[a-z0-9_]+$")


class Report:
    def __init__(self):
        self.errors: list[str] = []
        self.warnings: list[str] = []

    def error(self, where, msg):
        self.errors.append(f"FEHLER  {where}: {msg}")

    def warn(self, where, msg):
        self.warnings.append(f"WARNUNG {where}: {msg}")


def load(path: Path, rep: Report):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as e:
        rep.error(path.name, f"kein gültiges JSON: {e}")
        return None


def check_name(obj, where, rep, field="name"):
    name = obj.get(field)
    if not isinstance(name, dict) or not name.get("de") or not name.get("en"):
        rep.error(where, f"'{field}' braucht \"de\" und \"en\"")


def check_unlock(unlock, where, rep, flags):
    if unlock in ("start", "shop"):
        return
    if isinstance(unlock, str) and unlock.startswith("flag:"):
        flag = unlock[5:]
        if flag not in flags:
            rep.error(where, f"Flag '{flag}' fehlt in WorldState.json (Tippfehler?)")
        return
    rep.error(where, f"unlock '{unlock}' unbekannt (start, shop, flag:<Flag>)")


def validate(rep: Report):
    world = load(WORLD, rep) or {}
    flags = set((world.get("flags") or {}).keys())

    files = sorted(DATA.glob("*.json"))
    if not files:
        rep.error("Appearance", "keine Dateien gefunden")
        return {}

    options: dict[str, dict[str, dict]] = {cat: {} for cat in LISTS.values()}
    sliders: dict[str, dict] = {}
    palettes: dict[str, list] = {}
    slots: list[str] = []
    shops: dict[str, dict] = {}
    defaults = None
    limits = {}

    for path in files:
        data = load(path, rep)
        if data is None:
            continue
        src = path.name
        if "outfit_slots" in data:
            slots += [s for s in data["outfit_slots"] if s not in slots]
        limits.update(data.get("limits", {}))
        for key, cat in LISTS.items():
            for i, opt in enumerate(data.get(key, [])):
                oid = opt.get("id")
                where = f"{src}/{key}/{oid or i}"
                if not oid:
                    rep.error(where, "id fehlt")
                    continue
                if not ID_RE.match(oid):
                    rep.warn(where, "id nur aus a-z, 0-9 und _ (keine Umlaute)")
                if oid in options[cat]:
                    rep.error(where, "id doppelt")
                check_name(opt, where, rep)
                check_unlock(opt.get("unlock", "start"), where, rep, flags)
                asset = opt.get("asset", "")
                if asset and not asset.startswith("/Game/"):
                    rep.error(where, f"Asset-Pfad '{asset}' muss mit /Game/ beginnen")
                price = opt.get("price", 0)
                if not isinstance(price, int) or price < 0:
                    rep.error(where, "price muss eine ganze Zahl >= 0 sein")
                if opt.get("unlock") == "shop" and price == 0:
                    rep.warn(where, "Laden-Option ohne Preis")
                if cat == "outfit" and opt.get("slot") not in slots:
                    rep.error(where, f"Slot '{opt.get('slot')}' unbekannt")
                if cat == "marks" and opt.get("kind") not in MARK_KINDS:
                    rep.error(where, f"kind '{opt.get('kind')}' unbekannt ({', '.join(sorted(MARK_KINDS))})")
                options[cat][oid] = opt
        for i, sl in enumerate(data.get("sliders", [])):
            sid = sl.get("id")
            where = f"{src}/sliders/{sid or i}"
            if not sid or sid in sliders:
                rep.error(where, "id fehlt oder doppelt")
                continue
            check_name(sl, where, rep)
            check_name(sl, where, rep, "group_name")
            group = sl.get("group")
            if group not in SLIDER_GROUPS:
                rep.error(where, f"Gruppe '{group}' unbekannt")
            if (group == "koerper") != sid.startswith("koerper_"):
                rep.error(where, "Körper-Regler müssen mit 'koerper_' beginnen und in Gruppe 'koerper' stehen (und umgekehrt)")
            if not sl.get("morph_pos") or not sl.get("morph_neg"):
                rep.error(where, "morph_pos und morph_neg nötig")
            sliders[sid] = sl
        for name, colors in data.get("palettes", {}).items():
            if name not in PALETTES:
                rep.warn(f"{src}/palettes/{name}", "Palette wird vom Editor nicht benutzt")
            for c in colors:
                if not COLOR_RE.match(str(c)):
                    rep.error(f"{src}/palettes/{name}", f"'{c}' ist kein #RRGGBB")
            palettes[name] = colors
        if "defaults" in data:
            defaults = data["defaults"]
        for i, shop in enumerate(data.get("shops", [])):
            sid = shop.get("id")
            where = f"{src}/shops/{sid or i}"
            if not sid or sid in shops:
                rep.error(where, "id fehlt oder doppelt")
                continue
            check_name(shop, where, rep)
            for area in shop.get("categories", []):
                base = area.split(":", 1)[0]
                if base not in AREAS:
                    rep.error(where, f"Bereich '{area}' unbekannt")
                if ":" in area and area.split(":", 1)[1] not in slots:
                    rep.error(where, f"Slot in '{area}' unbekannt")
            if shop.get("base_price", 0) < 0:
                rep.error(where, "base_price < 0")
            if shop.get("npc") and shop["npc"] not in (world.get("npcs") or []):
                rep.warn(where, f"NPC '{shop['npc']}' fehlt in WorldState.json")
            for region in shop.get("regions", []):
                if region not in (world.get("regions") or []):
                    rep.warn(where, f"Region '{region}' fehlt in WorldState.json")
            shops[sid] = shop

    # Jede Kategorie braucht eine Start-Option
    for cat, opts in options.items():
        if not any(o.get("unlock", "start") == "start" for o in opts.values()):
            rep.error(f"Kategorie {cat}", "keine 'start'-Option – im Editor wäre nichts wählbar")
    for slot in slots:
        if not any(o.get("slot") == slot and o.get("unlock", "start") == "start" for o in options["outfit"].values()):
            rep.error(f"Slot {slot}", "kein 'start'-Kleidungsstück")
    for p in PALETTES:
        if not palettes.get(p):
            rep.error("palettes", f"Palette '{p}' fehlt oder ist leer")
    if len(options["face"]) and sum(1 for o in options["face"].values() if not o.get("morph")) != 1:
        rep.error("face_presets", "genau ein Grundgesicht braucht \"morph\": \"\" (die Grundform)")

    # Standard-Aussehen
    if defaults is None:
        rep.error("Appearance.json", "defaults fehlen")
    else:
        refs = {"body_type": "body", "face_a": "face", "face_b": "face", "hair_style": "hair", "beard": "beard",
                "brows": "brows", "voice": "voice", "ring_metal": "metal", "outfit_pattern": "pattern",
                "outfit_fabric": "fabric"}
        for field, cat in refs.items():
            oid = defaults.get(field)
            opt = options[cat].get(oid)
            if not opt:
                rep.error(f"defaults/{field}", f"'{oid}' gibt es nicht")
            elif opt.get("unlock", "start") != "start":
                rep.error(f"defaults/{field}", f"'{oid}' ist nicht 'start'")
        for field in ("skin_color", "eye_color_l", "eye_color_r", "hair_color", "hair_tip_color", "quartz"):
            if not COLOR_RE.match(str(defaults.get(field, ""))):
                rep.error(f"defaults/{field}", "Farbe fehlt oder ist kein #RRGGBB")
        outfit = defaults.get("outfit", {})
        for slot in slots:
            oid = outfit.get(slot)
            opt = options["outfit"].get(oid)
            if not opt or opt.get("slot") != slot:
                rep.error(f"defaults/outfit/{slot}", f"'{oid}' fehlt oder passt nicht in den Slot")
            elif opt.get("unlock", "start") != "start":
                rep.error(f"defaults/outfit/{slot}", f"'{oid}' ist nicht 'start'")
        hmin, hmax = limits.get("height_min", 0.95), limits.get("height_max", 1.05)
        if not hmin <= defaults.get("height", 1.0) <= hmax:
            rep.error("defaults/height", "außerhalb der Grenzen")

    return {"options": options, "sliders": sliders, "slots": slots, "shops": shops}


def summary(info):
    opts = info["options"]

    def count(cat, unlock):
        return sum(1 for o in opts[cat].values() if str(o.get("unlock", "start")).startswith(unlock))

    print("\nÜbersicht            Editor   Läden   Freischaltung")
    names = {"body": "Körperbau", "face": "Grundgesichter", "hair": "Frisuren", "beard": "Bärte", "brows": "Brauen",
             "marks": "Male/Tattoos", "voice": "Stimmen", "outfit": "Kleidung", "pattern": "Muster", "fabric": "Stoffe",
             "metal": "Ring-Metalle"}
    for cat, label in names.items():
        print(f"  {label:<18} {count(cat, 'start'):>6}  {count(cat, 'shop'):>6}  {count(cat, 'flag:'):>6}")
    print(f"  {'Regler':<18} {len(info['sliders']):>6}")
    print(f"  {'Kleidungs-Slots':<18} {len(info['slots']):>6}")
    print(f"  {'Läden':<18} {len(info['shops']):>6}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--strict", action="store_true", help="Warnungen zählen als Fehler")
    args = parser.parse_args()

    rep = Report()
    info = validate(rep)
    for line in rep.errors + rep.warnings:
        print(line)
    if info:
        summary(info)
    failed = bool(rep.errors) or (args.strict and bool(rep.warnings))
    print(f"\n{len(rep.errors)} Fehler, {len(rep.warnings)} Warnungen")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())

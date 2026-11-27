#!/usr/bin/env python3
"""
Prüfskript für spielbare Weisen, Status-Effekte und Gegnertypen (Nocturna / Ewigfels, Phase 3).

Liest Content/Data/Spells/*.json (ohne Katalog/) und Content/Data/Creatures/**/*.json und prüft
dieselben Regeln wie der C++-Lader (EwigfelsSpellTypes.cpp, EwigfelsCreatureTypes.cpp):
  - Pflichtfelder, doppelte IDs, Name in "de" und "en"
  - erlaubte Werte: delivery.type, effects[].type, hands, behavior, Grundtöne im Akkord
  - Verweise: Status in Effekten/Boni, counter_spell, escalate_to/cancels, Gegner-Zauber
  - catalog_id zeigt auf eine Kernweise im Katalog, Akkord und Name passen dazu
  - Farben als #RRGGBB, Asset-Pfade beginnen mit /Game/
  - Plausibilität: Parierfenster, Abklingzeiten, Kosten, Reichweiten
Zeigt am Ende eine kleine Balance-Tabelle (Schaden pro Sekunde, Atem pro Schaden).

Aufruf (aus dem Projektordner):
  python Scripts/Tools/validate_combat.py           # Bericht, Exit-Code 1 bei Fehlern
  python Scripts/Tools/validate_combat.py --strict  # Warnungen zählen als Fehler
Nur Python-Standardbibliothek.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TONES = {"Glut", "Firn", "Strom", "Quell", "Grund", "Wachs", "Hall", "Urton"}
DELIVERIES = {"projectile", "beam", "cone", "shield", "area", "self"}
EFFECTS = {"damage", "status", "knockback", "pull", "restore_atem", "heal", "stagger_attacker"}
BEHAVIORS = {"melee", "ranged", "dummy"}
SPELL_FIELDS = {"id", "name", "description", "kind", "catalog_id", "school", "chord", "unlock", "cost", "cooldown",
                "cast_time", "recovery", "hands", "delivery", "effects", "on_parry", "counter_spell", "vfx", "anim", "sfx"}
# Felder aus dem Katalog, die der Lader als Extra mitnimmt (kein Tippfehler-Hinweis)
CATALOG_EXTRA = {"learn", "combo", "koop", "heft", "grundton", "wirkart", "wirkung", "act", "legacy_name", "delivery_hint"}
COLOR_RE = re.compile(r"^#[0-9A-Fa-f]{6}$")


class Report:
    def __init__(self):
        self.errors: list[str] = []
        self.warnings: list[str] = []

    def error(self, where, msg):
        self.errors.append(f"FEHLER  {where}: {msg}")

    def warn(self, where, msg):
        self.warnings.append(f"WARNUNG {where}: {msg}")


def load_json(path: Path, rep: Report):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as e:
        rep.error(path.relative_to(ROOT).as_posix(), f"kein gültiges JSON: {e}")
        return None


def check_loc(rep, where, value, field="name"):
    if isinstance(value, str):
        rep.warn(where, f"{field} nur als Text, Übersetzung 'en' fehlt")
        return
    if not isinstance(value, dict):
        rep.error(where, f"{field} fehlt")
        return
    for lang in ("de", "en"):
        if not str(value.get(lang, "")).strip():
            (rep.error if lang == "de" else rep.warn)(where, f"{field}: Übersetzung '{lang}' fehlt")


def check_color(rep, where, value):
    if value is not None and not (isinstance(value, str) and COLOR_RE.match(value)):
        rep.error(where, f"'{value}' ist kein #RRGGBB")


def check_path(rep, where, value):
    if value is not None and not (isinstance(value, str) and value.startswith("/Game/")):
        rep.error(where, f"Asset-Pfad '{value}' muss mit /Game/ beginnen")


def num(rep, where, obj, key, lo=None, hi=None, default=None):
    v = obj.get(key, default)
    if v is None:
        return None
    if isinstance(v, bool) or not isinstance(v, (int, float)):
        rep.error(where, f"{key} muss eine Zahl sein")
        return None
    if (lo is not None and v < lo) or (hi is not None and v > hi):
        rep.warn(where, f"{key}={v} liegt außerhalb {lo}–{hi}")
    return v


def load_catalog(rep) -> dict:
    p = ROOT / "Content" / "Data" / "Spells" / "Katalog" / "Kernweisen.json"
    if not p.exists():
        return {}
    data = load_json(p, rep) or {}
    return {s.get("id"): s for s in data.get("spells", []) if isinstance(s, dict)}


def load_section(dir_: Path, key: str, rep: Report, skip_katalog=False):
    """Alle Einträge unter `key` aus allen JSON-Dateien. Ergebnis: Liste (id, where, obj)."""
    items = []
    if not dir_.exists():
        return items
    for path in sorted(dir_.rglob("*.json")):
        rel = path.relative_to(ROOT).as_posix()
        if skip_katalog and "/Katalog/" in "/" + path.relative_to(dir_).as_posix():
            continue
        data = load_json(path, rep)
        if not isinstance(data, dict) or key not in data:
            continue
        arr = data[key]
        if not isinstance(arr, list):
            rep.error(rel, f"'{key}' muss eine Liste sein")
            continue
        for i, obj in enumerate(arr):
            where = f"{rel} {key}[{i}]"
            if not isinstance(obj, dict):
                rep.error(where, "muss ein Objekt sein")
                continue
            if not obj.get("id"):
                rep.error(where, "id fehlt")
                continue
            items.append((obj["id"], f"{rel} {obj['id']}", obj))
    return items


def unique(rep, items, what):
    seen = {}
    for oid, where, obj in items:
        if oid in seen:
            rep.error(where, f"{what}-ID doppelt (schon in {seen[oid]})")
        else:
            seen[oid] = where
    return {oid: obj for oid, _, obj in items}


def check_effects(rep, where, effects, statuses, field):
    if effects is None:
        return
    if not isinstance(effects, list):
        rep.error(where, f"{field} muss eine Liste sein")
        return
    for j, e in enumerate(effects):
        w = f"{where} {field}[{j}]"
        t = e.get("type") if isinstance(e, dict) else None
        if t not in EFFECTS:
            rep.error(w, f"unbekannter Effekt-Typ '{t}' (erlaubt: {', '.join(sorted(EFFECTS))})")
            continue
        if t in ("damage", "knockback", "pull", "restore_atem", "heal"):
            if num(rep, w, e, "amount", 0, 5000) in (None, 0):
                rep.warn(w, f"{t} ohne amount wirkt nicht")
        if t == "damage" and e.get("element") and e["element"] not in TONES:
            rep.error(w, f"unbekanntes Element '{e['element']}'")
        if t == "status":
            if e.get("status") not in statuses:
                rep.error(w, f"Status '{e.get('status')}' gibt es nicht")
        bonus = e.get("bonus")
        if bonus is not None:
            if bonus.get("if_status") not in statuses:
                rep.error(w, f"bonus.if_status '{bonus.get('if_status')}' gibt es nicht")
            num(rep, w, bonus, "mult", 0.1, 10)


def check_spell(rep, sid, where, s, statuses, spells, catalog):
    for key in s:
        if key not in SPELL_FIELDS and key not in CATALOG_EXTRA and not key.startswith("_"):
            rep.warn(where, f"unbekanntes Feld '{key}' (Tippfehler? wird als Extra durchgereicht)")
    check_loc(rep, where, s.get("name"))
    chord = s.get("chord", [])
    if s.get("kind") == "gegner" and not chord:
        pass  # Gegnerweisen brauchen keinen Akkord
    elif not isinstance(chord, list) or len(chord) != 3:
        rep.error(where, "chord braucht genau 3 Grundtöne")
    else:
        for t in chord:
            if t not in TONES:
                rep.error(where, f"unbekannter Grundton '{t}'")
    if s.get("hands", "one") not in ("one", "two"):
        rep.error(where, "hands muss 'one' oder 'two' sein")
    cost = num(rep, where, s, "cost", 0, 100, 0)
    cd = num(rep, where, s, "cooldown", 0, 120, 0)
    num(rep, where, s, "cast_time", 0, 3, 0)
    num(rep, where, s, "recovery", 0, 3, 0.2)

    d = s.get("delivery")
    if not isinstance(d, dict):
        rep.error(where, "delivery fehlt")
        return
    dt = d.get("type")
    if dt not in DELIVERIES:
        rep.error(where, f"unbekannter delivery.type '{dt}' (erlaubt: {', '.join(sorted(DELIVERIES))})")
    if dt == "projectile":
        num(rep, where, d, "speed", 200, 20000)
        num(rep, where, d, "range", 100, 10000)
        num(rep, where, d, "homing", 0, 1)
        num(rep, where, d, "chain", 1, 8)
    if dt == "beam":
        num(rep, where, d, "range", 100, 6000)
        num(rep, where, d, "radius", 1, 120)
        num(rep, where, d, "chain", 1, 8)
        num(rep, where, d, "pierce", 0, 10)
    if dt == "cone":
        num(rep, where, d, "angle", 1, 360)
        num(rep, where, d, "range", 50, 5000)
    if dt == "shield":
        pw = num(rep, where, d, "parry_window", 0, 1, 0.2)
        if pw is not None and not 0.08 <= pw <= 0.4:
            rep.warn(where, f"parry_window {pw}s fühlt sich unfair an (gut: 0.12–0.3)")
        num(rep, where, d, "block_damage_mult", 0, 1)
    if dt != "shield" and not s.get("effects"):
        rep.warn(where, "keine effects: Zauber bewirkt nichts")
    if dt != "shield" and (s.get("on_parry") or s.get("counter_spell")):
        rep.warn(where, "on_parry/counter_spell wirken nur bei delivery.type 'shield'")

    check_effects(rep, where, s.get("effects"), statuses, "effects")
    check_effects(rep, where, s.get("on_parry"), statuses, "on_parry")
    cs = s.get("counter_spell")
    if cs and cs not in spells:
        rep.error(where, f"counter_spell '{cs}' gibt es nicht")

    vfx = s.get("vfx") or {}
    check_color(rep, where + " vfx.color", vfx.get("color"))
    for k in ("niagara", "impact_niagara"):
        check_path(rep, f"{where} vfx.{k}", vfx.get(k))
    check_path(rep, where + " anim.montage", (s.get("anim") or {}).get("montage"))

    # Abgleich mit dem Weisen-Katalog
    cid = s.get("catalog_id")
    if cid and catalog:
        k = catalog.get(cid)
        if not k:
            rep.error(where, f"catalog_id '{cid}' gibt es im Katalog nicht")
        elif s.get("unlock") != "hidden":
            if k.get("chord") != chord:
                rep.error(where, f"Akkord {chord} passt nicht zum Katalog {cid} {k.get('chord')}")
            kn = (k.get("name") or {}).get("de")
            sn = (s.get("name") or {}).get("de") if isinstance(s.get("name"), dict) else s.get("name")
            if kn and sn and kn != sn:
                rep.warn(where, f"Name '{sn}' weicht vom Katalog {cid} '{kn}' ab")
            ke = (k.get("name") or {}).get("en")
            se = (s.get("name") or {}).get("en") if isinstance(s.get("name"), dict) else None
            if ke and se and ke != se:
                rep.warn(where, f"englischer Name '{se}' weicht vom Katalog {cid} '{ke}' ab")
            if k.get("hands") and k["hands"] != s.get("hands", "one"):
                rep.warn(where, f"hands '{s.get('hands', 'one')}' weicht vom Katalog ({k['hands']}) ab")

    # Kosten-Plausibilität: starke Flächen-/Kegelzauber sollten etwas kosten
    if dt in ("cone", "area") and not cost and not cd:
        rep.warn(where, "Flächenzauber ohne Kosten und Abklingzeit: kann gespammt werden")


def check_status(rep, sid, where, st, statuses):
    check_loc(rep, where, st.get("name"))
    num(rep, where, st, "duration", 0.05, 60)
    num(rep, where, st, "damage_per_second", 0, 100)
    num(rep, where, st, "move_speed_mult", 0, 2)
    num(rep, where, st, "action_speed_mult", 0.05, 2)
    check_color(rep, where + " color", st.get("color"))
    for c in st.get("cancels", []) or []:
        if c not in statuses:
            rep.error(where, f"cancels: Status '{c}' gibt es nicht")
    esc = st.get("escalate_to")
    if esc:
        if esc not in statuses:
            rep.error(where, f"escalate_to '{esc}' gibt es nicht")
        if not st.get("escalate_at"):
            rep.warn(where, "escalate_to ohne escalate_at: eskaliert nie")


def check_creature(rep, cid, where, c, spells):
    check_loc(rep, where, c.get("name"))
    b = c.get("behavior")
    if b not in BEHAVIORS:
        rep.error(where, f"unbekanntes behavior '{b}' (erlaubt: {', '.join(sorted(BEHAVIORS))})")
    num(rep, where, c, "health", 1, 100000)
    walk = num(rep, where, c, "walk_speed", 0, 2000)
    run = num(rep, where, c, "run_speed", 0, 2000)
    if walk and run and run < walk:
        rep.warn(where, "run_speed ist kleiner als walk_speed")
    ar = num(rep, where, c, "attack_range", 0, 10000)
    pr = num(rep, where, c, "preferred_range", 0, 10000)
    fr = num(rep, where, c, "flee_range", 0, 10000)
    if b == "ranged":
        if pr and ar and pr > ar:
            rep.error(where, f"preferred_range {pr} > attack_range {ar}: kommt nie zum Angriff")
        if fr and pr and fr >= pr:
            rep.error(where, f"flee_range {fr} >= preferred_range {pr}: flieht dauernd")
    if b in ("melee", "ranged") and not c.get("spells"):
        rep.error(where, "Kämpfer ohne spells kann nicht angreifen")
    for sp in c.get("spells", []) or []:
        if sp not in spells:
            rep.error(where, f"Zauber '{sp}' gibt es nicht")
    for tone, m in (c.get("element_mult") or {}).items():
        if tone not in TONES:
            rep.error(where, f"element_mult: unbekannter Grundton '{tone}'")
        elif not isinstance(m, (int, float)) or m < 0:
            rep.error(where, f"element_mult.{tone} muss eine Zahl ≥ 0 sein")
    look = c.get("look") or {}
    for k in ("body", "accent", "eyes"):
        check_color(rep, f"{where} look.{k}", look.get(k))
    check_path(rep, where + " look.mesh", look.get("mesh"))


def balance_table(spells) -> str:
    rows = ["Balance (Spieler-Weisen, grob, ohne Status/Boni):",
            f"  {'Weise':<18} {'Schaden':>8} {'Zyklus s':>9} {'DPS':>6} {'Atem':>5}"]
    for sid, s in sorted(spells.items()):
        if s.get("unlock") == "hidden" or s.get("kind") == "gegner":
            continue
        dmg = sum(e.get("amount", 0) for e in s.get("effects", []) or [] if e.get("type") == "damage")
        cycle = max(s.get("cast_time", 0) + s.get("recovery", 0.2), s.get("cooldown", 0)) or 0.01
        rows.append(f"  {sid:<18} {dmg:>8.0f} {cycle:>9.2f} {dmg / cycle:>6.1f} {s.get('cost', 0):>5}")
    return "\n".join(rows)


def main() -> int:
    global ROOT
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--strict", action="store_true")
    ap.add_argument("--quiet", action="store_true")
    ap.add_argument("--root", type=Path, help="anderer Projektordner (für Tests)")
    args = ap.parse_args()
    if args.root:
        ROOT = args.root.resolve()
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")

    rep = Report()
    spell_dir = ROOT / "Content" / "Data" / "Spells"
    spell_items = load_section(spell_dir, "spells", rep, skip_katalog=True)
    status_items = load_section(spell_dir, "statuses", rep, skip_katalog=True)
    creature_items = load_section(ROOT / "Content" / "Data" / "Creatures", "creatures", rep)
    spells = unique(rep, spell_items, "Zauber")
    statuses = unique(rep, status_items, "Status")
    creatures = unique(rep, creature_items, "Gegner")
    catalog = load_catalog(rep)

    for sid, where, st in status_items:
        check_status(rep, sid, where, st, statuses)
    for sid, where, s in spell_items:
        check_spell(rep, sid, where, s, statuses, spells, catalog)
    for cid, where, c in creature_items:
        check_creature(rep, cid, where, c, spells)

    if not args.quiet:
        for line in rep.errors + rep.warnings:
            print(line)
        print()
    print(f"{len(spells)} Weisen, {len(statuses)} Status, {len(creatures)} Gegnertypen geprüft: "
          f"{len(rep.errors)} Fehler, {len(rep.warnings)} Warnungen")
    if spells and not args.quiet:
        print(balance_table(spells))
    return 1 if rep.errors or (args.strict and rep.warnings) else 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""
Prüfskript für den Weisen-Katalog (Nocturna / Ewigfels).

Liest Content/Data/Spells/Katalog/{Kernweisen,Alltagsweisen,Koopweisen,Lehrer}.json und prüft:
  - doppelte IDs und Namen, Pflichtfelder, Name in "de" und "en"
  - erlaubte Werte (Schule, Grundton, Wirkart, Wirkung, Lernweg)
  - learn.who zeigt auf einen Eintrag in Lehrer.json, und der ist im Akt der Weise schon erreichbar
  - Kernweisen: jeder Akkord (als Menge) nur einmal, Urweisen nur aus Urton
  - Baukasten: jedes Feld Grundton × Grund-Wirkart (7 × 7) hat eine Grundform zum Selbst-Entdecken
und zeigt die Mengen pro Heft, Akt und Lernweg sowie den Animations-Bedarf.

Aufruf (aus dem Projektordner):
  python Scripts/Tools/validate_spells.py               # Bericht, Exit-Code 1 bei Fehlern
  python Scripts/Tools/validate_spells.py --write-doc   # schreibt zusätzlich docs/WEISEN_KATALOG.md
Nur Python-Standardbibliothek.
"""
from __future__ import annotations

import argparse
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
KAT = ROOT / "Content" / "Data" / "Spells" / "Katalog"
DOC = ROOT / "docs" / "WEISEN_KATALOG.md"

SCHOOLS = {"schlag": "Schlag", "schutz": "Schutz", "elementar": "Elementar", "wandel": "Wandel", "ur": "Ur (verboten)"}
TONES = ["Glut", "Firn", "Strom", "Quell", "Grund", "Wachs", "Hall"]
BASE_WIRKARTEN = ["schnippen", "tippen", "streichen", "kreisen", "pusten", "summen", "pfeifen"]
SPECIAL_WIRKARTEN = ["klatschen", "stampfen", "singen", "spielen", "tanzen"]
WIRKUNGEN = {"partikel", "licht", "ton", "material", "ui", "objekt", "physik", "ki", "welt"}
HOW = {"start", "story", "unterricht", "meister", "mitschueler", "liedblatt", "entdecken", "kreatur", "kael", "fest"}
HEFTE = {
    "grundformen": "Grundformen (selbst entdecken)", "unterricht": "Unterrichtsweisen", "streiche": "Streichweisen",
    "haus": "Haus-, Küchen- und Turmweisen", "stil": "Stilweisen", "fest": "Festweisen", "tiere": "Tier- und Kreaturenlieder",
    "natur": "Garten- und Naturweisen", "reise": "Reise- und Wanderweisen", "musik": "Musikweisen", "meister": "Meisterweisen",
    "bastel": "Bastelweisen", "echo": "Echoweisen", "spiel": "Spiel- und Sportweisen", "kael": "Kael-Fragmente",
}
HOW_LABEL = {"start": "Start", "story": "Story", "unterricht": "Unterricht", "meister": "Meister in der Welt",
             "mitschueler": "Mitschüler", "liedblatt": "Liedblatt", "entdecken": "selbst entdecken",
             "kreatur": "Kreatur", "kael": "Kael/Nym", "fest": "Fest"}


class Report:
    def __init__(self):
        self.errors: list[str] = []
        self.warnings: list[str] = []

    def error(self, where, msg):
        self.errors.append(f"FEHLER  {where}: {msg}")

    def warn(self, where, msg):
        self.warnings.append(f"WARNUNG {where}: {msg}")


def load(name, key, rep):
    p = KAT / name
    if not p.exists():
        rep.error(name, "Datei fehlt")
        return []
    try:
        return json.loads(p.read_text(encoding="utf-8"))[key]
    except (json.JSONDecodeError, KeyError) as e:
        rep.error(name, f"nicht lesbar: {e}")
        return []


def check_common(rep, s, teachers, ids, names):
    sid = s.get("id", "?")
    where = f"{s.get('kind', '?')} {sid}"
    for f in ("id", "kind", "name", "unlock", "description", "learn"):
        if f not in s:
            rep.error(where, f"Pflichtfeld '{f}' fehlt")
    if sid in ids:
        rep.error(where, "ID doppelt")
    ids.add(sid)
    name = s.get("name", {})
    for lang in ("de", "en"):
        if not name.get(lang):
            rep.error(where, f"Name ohne '{lang}'")
    if name.get("de") in names:
        rep.error(where, f"Name '{name.get('de')}' doppelt")
    names.add(name.get("de"))
    if not s.get("description", {}).get("de"):
        rep.error(where, "Beschreibung ohne 'de'")
    if not isinstance(s.get("unlock"), int) or not 1 <= s.get("unlock", 0) <= 5:
        rep.error(where, "unlock muss Akt 1–5 sein")
    learn = s.get("learn", {})
    if learn.get("how") not in HOW:
        rep.error(where, f"unbekannter Lernweg '{learn.get('how')}'")
    t = teachers.get(learn.get("who"))
    if not t:
        rep.error(where, f"Lehrer/Quelle '{learn.get('who')}' fehlt in Lehrer.json")
    elif t["act"] > s.get("unlock", 9):
        rep.error(where, f"'{learn.get('who')}' ist erst ab Akt {t['act']} erreichbar, Weise aber ab Akt {s.get('unlock')}")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--write-doc", action="store_true", help="docs/WEISEN_KATALOG.md neu schreiben")
    args = ap.parse_args()
    rep = Report()

    teachers_list = load("Lehrer.json", "teachers", rep)
    teachers = {t["id"]: t for t in teachers_list}
    if len(teachers) != len(teachers_list):
        rep.error("Lehrer.json", "doppelte Lehrer-ID")
    kern = load("Kernweisen.json", "spells", rep)
    alltag = load("Alltagsweisen.json", "spells", rep)
    koop = load("Koopweisen.json", "spells", rep)

    ids: set[str] = set()
    names: set[str] = set()
    chords = defaultdict(list)
    for s in kern:
        check_common(rep, s, teachers, ids, names)
        w = f"kern {s.get('id')}"
        if s.get("school") not in SCHOOLS:
            rep.error(w, f"unbekannte Schule '{s.get('school')}'")
        ch = s.get("chord", [])
        if len(ch) != 3:
            rep.error(w, "Akkord braucht 3 Grundtöne")
        if s.get("school") == "ur":
            if set(ch) != {"Urton"}:
                rep.error(w, "Urweisen bestehen nur aus Urton")
        else:
            bad = [c for c in ch if c not in TONES]
            if bad:
                rep.error(w, f"unbekannter Grundton {bad}")
            chords[tuple(sorted(ch))].append(s.get("id"))
        if not s.get("anim", {}).get("gesture"):
            rep.error(w, "Geste (anim.gesture) fehlt")
    for ch, who in chords.items():
        if len(who) > 1:
            rep.error("Akkorde", f"{'-'.join(ch)} doppelt: {', '.join(who)}")

    grid = defaultdict(list)
    for s in alltag:
        check_common(rep, s, teachers, ids, names)
        w = f"alltag {s.get('id')}"
        if s.get("heft") not in HEFTE:
            rep.error(w, f"unbekanntes Heft '{s.get('heft')}'")
        if s.get("grundton") not in TONES:
            rep.error(w, f"unbekannter Grundton '{s.get('grundton')}'")
        if s.get("wirkart") not in BASE_WIRKARTEN + SPECIAL_WIRKARTEN:
            rep.error(w, f"unbekannte Wirkart '{s.get('wirkart')}'")
        if s.get("wirkung") not in WIRKUNGEN:
            rep.error(w, f"unbekannte Wirkung '{s.get('wirkung')}'")
        if s.get("learn", {}).get("how") == "entdecken":
            grid[(s.get("grundton"), s.get("wirkart"))].append(s.get("id"))
    for t in TONES:
        for wa in BASE_WIRKARTEN:
            n = len(grid.get((t, wa), []))
            if n == 0:
                rep.error("Baukasten", f"keine Grundform für {t} × {wa}")
            elif n > 1:
                rep.error("Baukasten", f"{t} × {wa} hat {n} Grundformen (genau 1 erwartet)")

    for s in koop:
        check_common(rep, s, teachers, ids, names)
        mp = s.get("koop", {}).get("min_players")
        if mp not in (2, 3, 4):
            rep.error(f"koop {s.get('id')}", "koop.min_players muss 2–4 sein")

    # Bericht
    total = len(kern) + len(alltag) + len(koop)
    by_act = Counter(s["unlock"] for s in kern + alltag + koop)
    by_how = Counter(s["learn"]["how"] for s in kern + alltag + koop)
    by_heft = Counter(s["heft"] for s in alltag)
    wirkarten = Counter(s["wirkart"] for s in alltag)
    print(f"Weisen gesamt: {total}  (Kern {len(kern)}, Alltag {len(alltag)}, Koop {len(koop)})")
    print("Pro Akt (ab wann lernbar): " + ", ".join(f"Akt {a}: {by_act[a]}" for a in sorted(by_act)))
    print("Lernwege: " + ", ".join(f"{HOW_LABEL[h]} {n}" for h, n in by_how.most_common()))
    print("Hefte: " + ", ".join(f"{HEFTE[h]} {n}" for h, n in by_heft.most_common()))
    special = sorted({w for w in wirkarten if w in SPECIAL_WIRKARTEN})
    print(f"Animations-Bedarf: {len(kern)} Kern-Gesten + {len(koop)} Koop-Gesten + "
          f"{len(BASE_WIRKARTEN)} Grund-Wirkarten + {len(special)} Sonder-Wirkarten ({', '.join(special)}) "
          f"für {len(alltag)} Alltagsweisen")
    for m in rep.warnings + rep.errors:
        print(m)
    print(f"{len(rep.errors)} Fehler, {len(rep.warnings)} Warnungen")

    if args.write_doc and not rep.errors:
        write_doc(kern, alltag, koop, teachers, by_act, by_how, by_heft)
        print(f"geschrieben: {DOC.relative_to(ROOT)}")
    return 1 if rep.errors else 0


def write_doc(kern, alltag, koop, teachers, by_act, by_how, by_heft):
    tn = lambda w: teachers[w]["name"]
    L = ["# WEISEN-KATALOG – Nocturna", "",
         "> Automatisch erzeugt aus `Content/Data/Spells/Katalog/*.json` mit `python Scripts/Tools/validate_spells.py --write-doc`. "
         "Nicht von Hand ändern, sondern die JSON-Dateien. Erklärung des Systems: [WEISEN.md](WEISEN.md).", "",
         f"**{len(kern) + len(alltag) + len(koop)} Weisen:** {len(kern)} Kernweisen · {len(alltag)} Alltagsweisen · {len(koop)} Koop-Weisen", "",
         "| Akt | " + " | ".join(str(a) for a in sorted(by_act)) + " |",
         "|---|" + "---|" * len(by_act),
         "| neu lernbar | " + " | ".join(str(by_act[a]) for a in sorted(by_act)) + " |", "",
         "Lernwege: " + " · ".join(f"{HOW_LABEL[h]} {n}" for h, n in by_how.most_common()), "",
         "## 1. Kernweisen", ""]
    for sch, label in SCHOOLS.items():
        rows = [s for s in kern if s["school"] == sch]
        L += [f"### {label} ({len(rows)})", "", "| ID | Weise | Akkord | Akt | Effekt | Combo | Lernen bei |", "|---|---|---|---|---|---|---|"]
        for s in rows:
            L.append(f"| {s['id']} | **{s['name']['de']}** ({s['name']['en']}) | {'-'.join(s['chord'])} | {s['unlock']} | "
                     f"{s['description']['de']} | {s['combo']['de']} | {HOW_LABEL[s['learn']['how']]}: {tn(s['learn']['who'])} |")
        L.append("")
    L += ["## 2. Koop-Weisen", "", "| ID | Weise | Spieler | Akt | Effekt | Bonus | Lernen bei |", "|---|---|---|---|---|---|---|"]
    for s in koop:
        L.append(f"| {s['id']} | **{s['name']['de']}** ({s['name']['en']}) | {s['koop']['min_players']}+ | {s['unlock']} | "
                 f"{s['description']['de']} | {s['koop']['bonus']['de']} | {HOW_LABEL[s['learn']['how']]}: {tn(s['learn']['who'])} |")
    L += ["", "## 3. Baukasten: Grundformen (Grundton × Wirkart)", "",
          "Jedes Feld kann man im Probier-Modus selbst entdecken.", "",
          "| Grundton | " + " | ".join(w.capitalize() for w in BASE_WIRKARTEN) + " |", "|---|" + "---|" * len(BASE_WIRKARTEN)]
    g = {(s["grundton"], s["wirkart"]): s["name"]["de"] for s in alltag if s["learn"]["how"] == "entdecken"}
    for t in TONES:
        L.append(f"| **{t}** | " + " | ".join(g.get((t, w), "–") for w in BASE_WIRKARTEN) + " |")
    L += ["", "## 4. Alltagsweisen nach Heft", ""]
    for h, label in HEFTE.items():
        rows = [s for s in alltag if s["heft"] == h]
        if not rows or h == "grundformen":
            continue
        L += [f"### {label} ({len(rows)})", "", "| ID | Weise | Ton · Wirkart | Akt | Effekt | Lernen bei |", "|---|---|---|---|---|---|"]
        for s in rows:
            L.append(f"| {s['id']} | **{s['name']['de']}** | {s['grundton']} · {s['wirkart']} | {s['unlock']} | "
                     f"{s['description']['de']} | {tn(s['learn']['who'])} |")
        L.append("")
    L += ["## 5. Wer lehrt was", "", "| Lehrer/Quelle | Ort | ab Akt | Weisen | Bedingung |", "|---|---|---|---|---|"]
    cnt = Counter(s["learn"]["who"] for s in kern + alltag + koop)
    for t in sorted(teachers.values(), key=lambda t: (t["act"], -cnt[t["id"]])):
        if cnt[t["id"]]:
            L.append(f"| **{t['name']}** | {t['place']} | {t['act']} | {cnt[t['id']]} | {t['condition']} |")
    DOC.write_text("\n".join(L) + "\n", encoding="utf-8")


if __name__ == "__main__":
    sys.exit(main())

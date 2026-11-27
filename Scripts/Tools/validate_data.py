#!/usr/bin/env python3
"""
Prüfskript für Quest- und Dialogdaten (Nocturna / Ewigfels).

Prüft alle Quests (Content/Data/Quests/**/*.json) und Dialoge (Content/Data/Dialogues/**/*.sud):
  - fehlende oder doppelte IDs, Pflichtfelder
  - tote Verzweigungen (next zeigt ins Leere, goto auf fehlendes Label, Etappe ohne Ausweg)
  - unerreichbare Etappen und Quests (Voraussetzung existiert nicht oder Kreis)
  - Bedingungen/Effekte: Syntax wie im C++ (EwigfelsWorldStateTypes.cpp) und Schlüssel aus WorldState.json
  - fehlende Übersetzungen (jeder Quest-Text braucht "de" und "en")
und rechnet die geschätzte Gesamtspielzeit aus.

Aufruf (aus dem Projektordner):
  python Scripts/Tools/validate_data.py            # Bericht, Exit-Code 1 bei Fehlern
  python Scripts/Tools/validate_data.py --strict   # Warnungen zählen auch als Fehler
Nur Python-Standardbibliothek, kein pip nötig.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
QUEST_DIR = ROOT / "Content" / "Data" / "Quests"
DIALOGUE_DIR = ROOT / "Content" / "Data" / "Dialogues"
REGISTRY = ROOT / "Content" / "Data" / "World" / "WorldState.json"
STORY_BIBLE = ROOT / "docs" / "STORY_BIBLE.md"



def set_root(root: Path) -> None:
    global ROOT, QUEST_DIR, DIALOGUE_DIR, REGISTRY, STORY_BIBLE
    ROOT = root
    QUEST_DIR = ROOT / "Content" / "Data" / "Quests"
    DIALOGUE_DIR = ROOT / "Content" / "Data" / "Dialogues"
    REGISTRY = ROOT / "Content" / "Data" / "World" / "WorldState.json"
    STORY_BIBLE = ROOT / "docs" / "STORY_BIBLE.md"


LANGS = ("de", "en")
QUEST_TYPES = {"main", "series", "faction", "side"}
TIERS = {"feind": -100, "misstrauisch": -59, "neutral": -19, "geschaetzt": 20, "geschätzt": 20, "verbuendet": 60, "verbündet": 60}
QUEST_STATES = {"notstarted": 0, "active": 1, "completed": 2, "failed": 3}
FREE_TALK_PREFIXES = ("D_Schule_", "D_Dorf_", "D_Welt_")  # freie Gespräche ohne Quest
TARGET_HOURS = {"main": (45, 50), "series+faction+side": (35, 40)}


# ---------------------------------------------------------------------------
# Bericht

@dataclass
class Report:
    errors: list[str] = field(default_factory=list)
    warnings: list[str] = field(default_factory=list)

    def error(self, where: str, msg: str) -> None:
        self.errors.append(f"{where}: {msg}")

    def warn(self, where: str, msg: str) -> None:
        self.warnings.append(f"{where}: {msg}")


# ---------------------------------------------------------------------------
# Bedingungen und Effekte (gleiche Regeln wie EwigfelsWorldState::ParseCondition/ParseEffect)

KEY_RE = re.compile(r"[A-Za-z0-9_]+")
COND_OPS = (">=", "<=", "==", "!=", ">", "<", "=")


def split_domain_key(text: str):
    if ":" not in text:
        return None, None, None, "Bereich fehlt (flag:, rel:, rep: oder quest:)"
    prefix, body = text.split(":", 1)
    prefix = prefix.strip().lower()
    if prefix not in ("flag", "rel", "rep", "quest"):
        return None, None, None, f"unbekannter Bereich '{prefix}'"
    body = body.lstrip()
    m = KEY_RE.match(body)
    if not m:
        return None, None, None, "Schlüssel fehlt"
    return prefix, m.group(0), body[m.end():].strip(), None


def parse_int(s: str):
    s = s.strip()
    return int(s) if re.fullmatch(r"[+-]?\d+", s) else None


def parse_condition(text: str):
    """Gibt (domain, key, error) zurück."""
    t = text.strip()
    negated = t.startswith("!")
    if negated:
        t = t[1:]
    domain, key, rest, err = split_domain_key(t)
    if err:
        return None, None, err
    if not rest:
        if domain in ("rel", "rep"):
            return None, None, "Vergleich fehlt (z. B. >=30)"
        return domain, key, None
    if negated:
        return None, None, "'!' nur ohne Vergleich erlaubt"
    op = next((o for o in COND_OPS if rest.startswith(o)), None)
    if not op:
        return None, None, f"unbekannter Vergleich '{rest}'"
    value = rest[len(op):].strip()
    if parse_int(value) is not None:
        return domain, key, None
    if domain == "rep" and value.lower() in TIERS:
        return domain, key, None
    if domain == "quest" and value.lower() in QUEST_STATES:
        return domain, key, None
    return None, None, f"Wert '{value}' ist ungültig"


def parse_effect(text: str):
    domain, key, rest, err = split_domain_key(text.strip())
    if err:
        return None, None, err
    if domain == "quest":
        return None, None, "Quest-Status setzt nur das Questsystem"
    if not rest or rest[0] not in "=+-" or parse_int(rest[1:]) is None:
        return None, None, "Änderung fehlt oder ungültig (=1, +10 oder -5)"
    return domain, key, None


# ---------------------------------------------------------------------------
# Daten laden

def load_registry(report: Report) -> dict:
    try:
        data = json.loads(REGISTRY.read_text(encoding="utf-8"))
    except Exception as exc:  # noqa: BLE001
        report.error(str(REGISTRY.relative_to(ROOT)), f"nicht lesbar: {exc}")
        return {"flags": {}, "factions": {}, "relationships": {}, "objective_kinds": [], "regions": [], "npcs": []}
    return data


def planned_mission_ids() -> set[str]:
    """Missions-IDs aus der Story-Bibel (dürfen schon referenziert werden, auch wenn noch keine JSON existiert)."""
    if not STORY_BIBLE.exists():
        return set()
    return set(re.findall(r"^\|\s*(M\d{3})\s*\|", STORY_BIBLE.read_text(encoding="utf-8"), re.MULTILINE))


def check_key(report: Report, where: str, domain: str, key: str, registry: dict, quest_ids: set[str], planned: set[str]):
    if domain == "flag" and key not in registry.get("flags", {}):
        report.error(where, f"Flag '{key}' steht nicht in WorldState.json")
    elif domain == "rel" and key not in registry.get("relationships", {}):
        report.error(where, f"Beziehung '{key}' steht nicht in WorldState.json")
    elif domain == "rep" and key not in registry.get("factions", {}):
        report.error(where, f"Fraktion '{key}' steht nicht in WorldState.json")
    elif domain == "quest" and key not in quest_ids:
        if key in planned:
            report.warn(where, f"Quest '{key}' ist geplant, aber noch nicht geschrieben")
        else:
            report.error(where, f"Quest '{key}' gibt es nicht")


def check_loc(report: Report, where: str, value, required: bool = True):
    if value is None:
        if required:
            report.error(where, "Text fehlt")
        return
    if isinstance(value, str):
        report.error(where, "Text ohne Übersetzung (bitte {\"de\": …, \"en\": …})")
        return
    if not isinstance(value, dict):
        report.error(where, "Text muss {\"de\": …, \"en\": …} sein")
        return
    for lang in LANGS:
        if not str(value.get(lang, "")).strip():
            report.error(where, f"Übersetzung '{lang}' fehlt")


# ---------------------------------------------------------------------------
# Quests

def load_quests(report: Report) -> dict[str, tuple[Path, dict]]:
    quests: dict[str, tuple[Path, dict]] = {}
    for path in sorted(QUEST_DIR.rglob("*.json")):
        rel = path.relative_to(ROOT).as_posix()
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
        except json.JSONDecodeError as exc:
            report.error(rel, f"kein gültiges JSON: {exc}")
            continue
        qid = data.get("id")
        if not qid:
            report.error(rel, "id fehlt")
            continue
        if qid != path.stem:
            report.warn(rel, f"Dateiname sollte {qid}.json sein")
        if qid in quests:
            report.error(rel, f"Quest-ID '{qid}' gibt es doppelt (auch in {quests[qid][0].relative_to(ROOT).as_posix()})")
            continue
        quests[qid] = (path, data)
    return quests


def check_quest(report: Report, qid: str, path: Path, q: dict, registry: dict, quest_ids: set[str],
                planned: set[str], dialogue_names: set[str]):
    rel = path.relative_to(ROOT).as_posix()
    kinds = set(registry.get("objective_kinds", []))

    if q.get("type") not in QUEST_TYPES:
        report.error(rel, f"type '{q.get('type')}' unbekannt ({', '.join(sorted(QUEST_TYPES))})")
    if not isinstance(q.get("duration_min"), int) or q["duration_min"] <= 0:
        report.error(rel, "duration_min (Minuten, > 0) fehlt")
    check_loc(report, f"{rel}: title", q.get("title"))
    check_loc(report, f"{rel}: summary", q.get("summary"))
    if q.get("region") and q["region"] not in registry.get("regions", []):
        report.warn(rel, f"Region '{q['region']}' steht nicht in WorldState.json")
    if q.get("giver") and q["giver"] not in registry.get("npcs", []):
        report.warn(rel, f"Auftraggeber '{q['giver']}' steht nicht in WorldState.json (npcs)")

    def conditions(items, where):
        for text in items or []:
            domain, key, err = parse_condition(text)
            if err:
                report.error(where, f"'{text}': {err}")
            else:
                check_key(report, where, domain, key, registry, quest_ids, planned)

    def effects(items, where):
        for text in items or []:
            domain, key, err = parse_effect(text)
            if err:
                report.error(where, f"'{text}': {err}")
            else:
                check_key(report, where, domain, key, registry, quest_ids, planned)

    conditions(q.get("requires"), f"{rel}: requires")
    for d in q.get("dialogues", []) or []:
        if d not in dialogue_names:
            report.error(f"{rel}: dialogues", f"Dialog '{d}' (.sud) gibt es nicht")

    stages = q.get("stages") or []
    if not stages:
        report.error(rel, "keine Etappen (stages)")
        return
    stage_ids = [s.get("id") for s in stages]
    for sid in {s for s in stage_ids if stage_ids.count(s) > 1}:
        report.error(rel, f"Etappen-ID '{sid}' doppelt")
    stage_set = set(filter(None, stage_ids))
    start = q.get("start_stage") or stage_ids[0]
    if start not in stage_set:
        report.error(rel, f"start_stage '{start}' gibt es nicht")

    graph: dict[str, list[str]] = defaultdict(list)
    can_end = set()
    for i, stage in enumerate(stages):
        sid = stage.get("id") or f"#{i}"
        where = f"{rel}: {sid}"
        if not stage.get("id"):
            report.error(where, "Etappe ohne id")
        check_loc(report, f"{where}.text", stage.get("text"))
        obj_ids = []
        for j, obj in enumerate(stage.get("objectives") or []):
            ow = f"{where}.objectives[{obj.get('id') or j}]"
            if not obj.get("id"):
                report.error(ow, "Ziel ohne id")
            obj_ids.append(obj.get("id"))
            check_loc(report, f"{ow}.text", obj.get("text"))
            kind = obj.get("kind")
            if kind not in kinds:
                report.error(ow, f"kind '{kind}' unbekannt ({', '.join(sorted(kinds))})")
            if not obj.get("target"):
                report.error(ow, "target fehlt")
            if kind == "condition" and obj.get("target"):
                conditions([obj["target"]], ow)
            if kind == "dialogue" and obj.get("target") and obj["target"] not in dialogue_names:
                report.error(ow, f"Dialog '{obj['target']}' (.sud) gibt es nicht")
            if "count" in obj and (not isinstance(obj["count"], int) or obj["count"] < 1):
                report.error(ow, "count muss eine Zahl ≥ 1 sein")
        for oid in {o for o in obj_ids if o and obj_ids.count(o) > 1}:
            report.error(where, f"Ziel-ID '{oid}' doppelt")
        if stage.get("objectives") and all(o.get("optional") for o in stage["objectives"]):
            report.warn(where, "nur optionale Ziele: Etappe ist sofort fertig")
        effects(stage.get("on_complete"), f"{where}.on_complete")

        nexts = stage.get("next") or []
        if not nexts:
            can_end.add(sid)  # ohne next: Quest endet erfolgreich
        has_fallback = False
        for k, n in enumerate(nexts):
            nw = f"{where}.next[{k}]"
            conditions(n.get("if"), nw)
            has_stage, has_end = bool(n.get("stage")), bool(n.get("end"))
            if has_stage == has_end:
                report.error(nw, "braucht genau eins von 'stage' oder 'end'")
            if has_stage:
                if n["stage"] not in stage_set:
                    report.error(nw, f"tote Verzweigung: Etappe '{n['stage']}' gibt es nicht")
                graph[sid].append(n["stage"])
            if has_end:
                if n["end"] not in ("completed", "failed"):
                    report.error(nw, "end nur 'completed' oder 'failed'")
                can_end.add(sid)
            if not n.get("if"):
                has_fallback = True
                if k < len(nexts) - 1:
                    report.warn(nw, "Regel ohne Bedingung vor weiteren Regeln: die folgenden werden nie benutzt")
        if nexts and not has_fallback:
            report.warn(where, "alle 'next'-Regeln haben Bedingungen: passt keine, bleibt die Quest stehen. Absicht?")

    # Erreichbarkeit und Ausweg
    seen, todo = set(), [start]
    while todo:
        s = todo.pop()
        if s in seen or s not in stage_set:
            continue
        seen.add(s)
        todo.extend(graph[s])
    for sid in stage_set - seen:
        report.warn(rel, f"Etappe '{sid}' ist vom Start aus nicht erreichbar")
    # Von welchen Etappen aus kommt man zu einem Ende?
    reaches_end = set(can_end)
    changed = True
    while changed:
        changed = False
        for s, targets in graph.items():
            if s not in reaches_end and any(t in reaches_end for t in targets):
                reaches_end.add(s)
                changed = True
    for sid in seen - reaches_end:
        report.error(rel, f"tote Verzweigung: von Etappe '{sid}' führt kein Weg zum Ende der Quest")

    rewards = q.get("rewards") or {}
    effects(rewards.get("effects"), f"{rel}: rewards.effects")


def check_quest_reachability(report: Report, quests: dict[str, tuple[Path, dict]], planned: set[str]):
    """Quest ist unerreichbar, wenn eine quest:-Voraussetzung fehlt (und nicht geplant ist) oder im Kreis hängt."""
    deps: dict[str, set[str]] = {}
    for qid, (_, q) in quests.items():
        d = set()
        for text in q.get("requires") or []:
            domain, key, err = parse_condition(text)
            if not err and domain == "quest" and not text.strip().startswith("!"):
                d.add(key)
        deps[qid] = d

    state: dict[str, int] = {}  # 0 = in Arbeit, 1 = erreichbar, 2 = unerreichbar

    def visit(qid: str, stack: list[str]) -> bool:
        if qid not in quests:
            return qid in planned  # geplante Quests gelten vorerst als erreichbar
        if state.get(qid) == 0:
            report.error(f"Quest {qid}", f"Voraussetzungen im Kreis: {' -> '.join(stack + [qid])}")
            state[qid] = 2
            return False
        if qid in state:
            return state[qid] == 1
        state[qid] = 0
        ok = all(visit(d, stack + [qid]) for d in deps[qid])
        state[qid] = 1 if ok else 2
        return ok

    for qid in sorted(quests):
        if not visit(qid, []):
            report.error(f"Quest {qid}", "unerreichbar (Voraussetzung fehlt oder hängt im Kreis)")


# ---------------------------------------------------------------------------
# Dialoge (.sud)

SUD_VAR_RE = re.compile(r"\{([\w.]+)\}")
SUD_SET_RE = re.compile(r"^\[set\s+([\w.]+)\s*=?\s*(.*)\]$")
SUD_EVENT_RE = re.compile(r"^\[event\s+([\w.]+)\s*(.*)\]$")
SUD_GOTO_RE = re.compile(r"^\[(?:goto|gosub)\s+(\w+)\]$")
SUD_LABEL_RE = re.compile(r"^:(\w+)$")
SUD_SPEAKER_RE = re.compile(r"^(\w+):\s")
KNOWN_EVENTS = {"state", "quest.start", "quest.report", "quest.objective", "quest.stage", "quest.fail", "emotion", "camera"}


def sud_args(text: str) -> list[str]:
    args = []
    for raw in [a.strip() for a in text.split(",")] if text.strip() else []:
        m = re.fullmatch(r"`([^`]*)`|\"((?:[^\"\\]|\\.)*)\"", raw)
        args.append(m.group(1) if m and m.group(1) is not None else (m.group(2) if m else raw))
    return args


def check_dialogue(report: Report, path: Path, registry: dict, quests: dict, planned: set[str]):
    rel = path.relative_to(ROOT).as_posix()
    quest_ids = set(quests)
    labels, gotos, speakers, named = set(), [], set(), set()
    in_header = False
    for no, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw.strip()
        where = f"{rel}:{no}"
        if not line or (line.startswith("#") and not line.startswith(("#=", "#+"))):
            continue
        if line == "===":
            in_header = not in_header
            continue

        for var in SUD_VAR_RE.findall(line):
            if "." in var and var.split(".", 1)[0] in ("flag", "rel", "rep", "quest"):
                check_key(report, where, var.split(".", 1)[0], var.split(".", 1)[1], registry, quest_ids, planned)

        if m := SUD_SET_RE.match(line):
            name = m.group(1)
            if name.startswith("SpeakerName."):
                named.add(name.split(".", 1)[1])
            elif name.split(".", 1)[0] in ("flag", "rel", "rep"):
                check_key(report, where, name.split(".", 1)[0], name.split(".", 1)[1], registry, quest_ids, planned)
            elif name.startswith("quest."):
                report.error(where, "quest.* ist nur lesbar, Quests über [event quest.*] steuern")
            continue
        if in_header:
            report.error(where, "im Kopf (===) sind nur [set …]-Zeilen erlaubt")
            continue
        if m := SUD_EVENT_RE.match(line):
            event, args = m.group(1), sud_args(m.group(2))
            if event not in KNOWN_EVENTS:
                report.warn(where, f"Event '{event}' kennt die Dialogbrücke nicht (geht nur an OnCue)")
            if event == "state":
                for a in args:
                    domain, key, err = parse_effect(a)
                    if err:
                        report.error(where, f"'{a}': {err}")
                    else:
                        check_key(report, where, domain, key, registry, quest_ids, planned)
            elif event.startswith("quest.") and event != "quest.report":
                qid = args[0] if args else ""
                if qid not in quest_ids:
                    report.error(where, f"Quest '{qid}' gibt es nicht")
                elif event in ("quest.objective", "quest.stage") and len(args) >= 2:
                    q = quests[qid][1]
                    if event == "quest.stage":
                        valid = {s.get("id") for s in q.get("stages", [])}
                    else:
                        valid = {o.get("id") for s in q.get("stages", []) for o in s.get("objectives", [])}
                    if args[1] not in valid:
                        report.error(where, f"'{args[1]}' gibt es in Quest {qid} nicht")
            elif event == "quest.report" and len(args) < 2:
                report.error(where, "quest.report braucht `kind`, `target`")
            continue
        if m := SUD_GOTO_RE.match(line):
            gotos.append((m.group(1), where))
            continue
        if m := SUD_LABEL_RE.match(line):
            labels.add(m.group(1))
            continue
        if m := SUD_SPEAKER_RE.match(line):
            speakers.add(m.group(1))

    for label, where in gotos:
        if label != "end" and label not in labels:
            report.error(where, f"tote Verzweigung: Label ':{label}' gibt es nicht")
    for label in labels - {g for g, _ in gotos}:
        report.warn(rel, f"Label ':{label}' wird nie angesprungen")
    for speaker in sorted(speakers - named):
        report.warn(rel, f"Sprecher '{speaker}' hat keinen Anzeigenamen ([set SpeakerName.{speaker} \"…\"] im Kopf)")


# ---------------------------------------------------------------------------
# Spielzeit

def playtime(quests: dict) -> str:
    minutes = defaultdict(int)
    count = defaultdict(int)
    for _, q in quests.values():
        t = q.get("type", "?")
        minutes[t] += q.get("duration_min", 0) or 0
        count[t] += 1
    names = {"main": "Hauptmissionen", "series": "Questreihen", "faction": "Fraktionen", "side": "Nebenquests"}
    lines = ["Geschätzte Spielzeit (umgesetzte Quests):"]
    for t in ("main", "series", "faction", "side"):
        lines.append(f"  {names[t]:<15} {count[t]:>3} Quests  {minutes[t] / 60:6.1f} Std.")
    total = sum(minutes.values()) / 60
    side = (minutes["series"] + minutes["faction"] + minutes["side"]) / 60
    lines.append(f"  {'Gesamt':<15} {sum(count.values()):>3} Quests  {total:6.1f} Std.   (Ziel: ~100 Std.)")
    lines.append(f"  Main {minutes['main'] / 60:.1f} von 45–50 Std. · Reihen+Fraktionen+Neben {side:.1f} von 35–40 Std.")
    return "\n".join(lines)


# ---------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--strict", action="store_true", help="Warnungen als Fehler werten")
    parser.add_argument("--quiet", action="store_true", help="nur Zusammenfassung")
    parser.add_argument("--root", type=Path, help="anderer Projektordner (für Tests)")
    args = parser.parse_args()
    if args.root:
        set_root(args.root.resolve())

    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")  # Umlaute in der Windows-Konsole

    report = Report()
    registry = load_registry(report)
    planned = planned_mission_ids()
    quests = load_quests(report)
    dialogues = sorted(DIALOGUE_DIR.rglob("*.sud"))
    dialogue_names = {p.stem for p in dialogues}

    for qid, (path, q) in sorted(quests.items()):
        check_quest(report, qid, path, q, registry, set(quests), planned, dialogue_names)
    check_quest_reachability(report, quests, planned)
    for path in dialogues:
        check_dialogue(report, path, registry, quests, planned)

    # Dialog, den keine Quest nutzt: nur Hinweis (kann auch von NPCs direkt kommen)
    used = {d for _, q in quests.values() for d in q.get("dialogues", []) or []}
    used |= {o.get("target") for _, q in quests.values() for s in q.get("stages", []) for o in s.get("objectives", []) if o.get("kind") == "dialogue"}
    for name in sorted(dialogue_names - used):
        if name.startswith(FREE_TALK_PREFIXES):
            continue  # freie Gespräche (NPC direkt ansprechen), brauchen keine Quest
        report.warn(f"Dialog {name}", "wird von keiner Quest erwähnt")

    if not args.quiet:
        for e in report.errors:
            print(f"FEHLER   {e}")
        for w in report.warnings:
            print(f"WARNUNG  {w}")
        print()
    print(f"{len(quests)} Quests, {len(dialogues)} Dialoge geprüft: {len(report.errors)} Fehler, {len(report.warnings)} Warnungen")
    print(playtime(quests))

    failed = bool(report.errors) or (args.strict and bool(report.warnings))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())

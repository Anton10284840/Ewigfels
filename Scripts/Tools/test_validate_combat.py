#!/usr/bin/env python3
"""Selbsttest für validate_combat.py: kaputte Beispieldaten müssen genau die erwarteten Fehler liefern.
Aufruf: python Scripts/Tools/test_validate_combat.py"""
import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "Scripts" / "Tools" / "validate_combat.py"


def run(root: Path) -> tuple[int, str]:
    p = subprocess.run([sys.executable, str(SCRIPT), "--root", str(root)], capture_output=True, text=True, encoding="utf-8")
    return p.returncode, p.stdout


def spell(sid="probe", **fields):
    s = {"id": sid, "name": {"de": "Probe", "en": "Probe"}, "chord": ["Glut", "Strom", "Strom"], "hands": "one",
         "delivery": {"type": "projectile", "speed": 3000, "range": 2000},
         "effects": [{"type": "damage", "amount": 10, "element": "Glut"}]}
    s.update(fields)
    return s


STATUS = [{"id": "chilled", "name": {"de": "Kalt", "en": "Cold"}, "duration": 3}]


class CombatValidatorTest(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp())
        (self.tmp / "Content/Data/Spells").mkdir(parents=True)
        (self.tmp / "Content/Data/Creatures").mkdir(parents=True)

    def tearDown(self):
        shutil.rmtree(self.tmp)

    def write(self, spells=(), statuses=STATUS, creatures=()):
        (self.tmp / "Content/Data/Spells/Test.json").write_text(
            json.dumps({"spells": list(spells), "statuses": list(statuses)}), encoding="utf-8")
        (self.tmp / "Content/Data/Creatures/Test.json").write_text(
            json.dumps({"creatures": list(creatures)}), encoding="utf-8")

    def test_echte_daten_sind_fehlerfrei(self):
        code, out = run(ROOT)
        self.assertEqual(code, 0, out)

    def test_gueltige_weise(self):
        self.write([spell()])
        code, out = run(self.tmp)
        self.assertEqual(code, 0, out)

    def test_unbekannter_status_und_effekt(self):
        self.write([spell(effects=[{"type": "status", "status": "gibtsnicht"}, {"type": "zaubern"}])])
        code, out = run(self.tmp)
        self.assertEqual(code, 1)
        self.assertIn("Status 'gibtsnicht' gibt es nicht", out)
        self.assertIn("unbekannter Effekt-Typ 'zaubern'", out)

    def test_akkord_und_doppelte_id(self):
        self.write([spell(chord=["Glut", "Quietsch"]), spell()])
        code, out = run(self.tmp)
        self.assertIn("chord braucht genau 3 Grundtöne", out)
        self.assertIn("Zauber-ID doppelt", out)

    def test_konter_fehlt(self):
        self.write([spell(delivery={"type": "shield", "parry_window": 0.2}, effects=[], counter_spell="weg")])
        code, out = run(self.tmp)
        self.assertIn("counter_spell 'weg' gibt es nicht", out)

    def test_gegner_regeln(self):
        self.write([spell()], creatures=[
            {"id": "G1", "name": {"de": "G", "en": "G"}, "behavior": "ranged", "attack_range": 1000,
             "preferred_range": 1200, "flee_range": 1300, "spells": ["probe", "fehlt"], "look": {"body": "rot"}}])
        code, out = run(self.tmp)
        self.assertEqual(code, 1)
        self.assertIn("kommt nie zum Angriff", out)
        self.assertIn("flieht dauernd", out)
        self.assertIn("Zauber 'fehlt' gibt es nicht", out)
        self.assertIn("'rot' ist kein #RRGGBB", out)

    def test_katalog_abgleich(self):
        (self.tmp / "Content/Data/Spells/Katalog").mkdir()
        (self.tmp / "Content/Data/Spells/Katalog/Kernweisen.json").write_text(json.dumps(
            {"spells": [{"id": "K01", "name": {"de": "Probe"}, "chord": ["Firn", "Firn", "Firn"]}]}), encoding="utf-8")
        self.write([spell(catalog_id="K01"), spell("b", catalog_id="K77")])
        code, out = run(self.tmp)
        self.assertIn("passt nicht zum Katalog K01", out)
        self.assertIn("catalog_id 'K77' gibt es im Katalog nicht", out)


if __name__ == "__main__":
    unittest.main()

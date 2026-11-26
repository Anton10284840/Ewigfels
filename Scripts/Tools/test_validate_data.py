#!/usr/bin/env python3
"""Selbsttest fürs Prüfskript: kaputte Beispieldaten müssen genau die erwarteten Fehler liefern.
Aufruf: python Scripts/Tools/test_validate_data.py"""
import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "Scripts" / "Tools" / "validate_data.py"


def run(root: Path) -> tuple[int, str]:
    p = subprocess.run([sys.executable, str(SCRIPT), "--root", str(root)], capture_output=True, text=True, encoding="utf-8")
    return p.returncode, p.stdout


class ValidatorTest(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp())
        (self.tmp / "Content/Data/Quests").mkdir(parents=True)
        (self.tmp / "Content/Data/Dialogues").mkdir(parents=True)
        (self.tmp / "Content/Data/World").mkdir(parents=True)
        (self.tmp / "docs").mkdir()
        shutil.copy(ROOT / "Content/Data/World/WorldState.json", self.tmp / "Content/Data/World/")

    def tearDown(self):
        shutil.rmtree(self.tmp)

    def quest(self, qid, **fields):
        q = {"id": qid, "type": "side", "duration_min": 30,
             "title": {"de": "T", "en": "T"}, "summary": {"de": "S", "en": "S"},
             "stages": [{"id": "s1", "text": {"de": "x", "en": "x"},
                         "objectives": [{"id": "o1", "kind": "talk", "target": "Edda", "text": {"de": "a", "en": "a"}}]}]}
        q.update(fields)
        (self.tmp / f"Content/Data/Quests/{qid}.json").write_text(json.dumps(q), encoding="utf-8")

    def test_echte_daten_sind_fehlerfrei(self):
        code, out = run(ROOT)
        self.assertEqual(code, 0, out)

    def test_gueltige_quest(self):
        self.quest("Q1")
        code, out = run(self.tmp)
        self.assertEqual(code, 0, out)

    def test_fehlende_uebersetzung(self):
        self.quest("Q1", title={"de": "Nur Deutsch"})
        code, out = run(self.tmp)
        self.assertEqual(code, 1)
        self.assertIn("Übersetzung 'en' fehlt", out)

    def test_tote_verzweigung(self):
        self.quest("Q1", stages=[{"id": "s1", "text": {"de": "x", "en": "x"}, "next": [{"stage": "gibtsnicht"}]}])
        code, out = run(self.tmp)
        self.assertIn("tote Verzweigung: Etappe 'gibtsnicht'", out)

    def test_unbekanntes_flag(self):
        self.quest("Q1", requires=["flag:Tippfeler"])
        code, out = run(self.tmp)
        self.assertIn("Flag 'Tippfeler' steht nicht", out)

    def test_quest_kreis(self):
        self.quest("Q1", requires=["quest:Q2"])
        self.quest("Q2", requires=["quest:Q1"])
        code, out = run(self.tmp)
        self.assertIn("Voraussetzungen im Kreis", out)
        self.assertIn("Quest Q2: unerreichbar", out)

    def test_doppelte_id(self):
        self.quest("Q1")
        (self.tmp / "Content/Data/Quests/sub").mkdir()
        shutil.copy(self.tmp / "Content/Data/Quests/Q1.json", self.tmp / "Content/Data/Quests/sub/Q1.json")
        code, out = run(self.tmp)
        self.assertIn("gibt es doppelt", out)

    def test_dialog_goto_und_event(self):
        self.quest("Q1", dialogues=["D1"])
        (self.tmp / "Content/Data/Dialogues/D1.sud").write_text(
            "===\n[set SpeakerName.Edda \"Edda\"]\n===\nEdda: Hallo {rel.Lio}\n  * Ok\n    [goto fehlt]\n"
            "[event state `rep:Kollegium+5`]\n[event quest.objective `Q1`, `o9`]\n", encoding="utf-8")
        code, out = run(self.tmp)
        self.assertIn("Label ':fehlt' gibt es nicht", out)
        self.assertIn("'o9' gibt es in Quest Q1 nicht", out)
        self.assertNotIn("rep:Kollegium+5", out)

    def test_spielzeit(self):
        self.quest("Q1", type="main", duration_min=120)
        code, out = run(self.tmp)
        self.assertIn("Hauptmissionen    1 Quests     2.0 Std.", out)


if __name__ == "__main__":
    unittest.main()

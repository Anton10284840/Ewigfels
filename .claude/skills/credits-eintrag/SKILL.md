---
name: credits-eintrag
description: Neue externe Ressource (Asset, Plugin, Sound, KI-Modell, Code) lizenzprüfen und in CREDITS.md eintragen. Nutzen, bevor irgendetwas Externes ins Projekt kommt.
---

# Externe Ressource aufnehmen

1. Lizenz an der Quelle prüfen (LICENSE-Datei bzw. Lizenzseite, nicht aus dem Gedächtnis).
   - OK: CC0, MIT, BSD, Apache 2.0, zlib, CC-BY (mit Credit), Fab Standard License, UE EULA.
   - Vorsicht: MPL-2.0 (Dateien bleiben offen), GPL/AGPL (nur als Werkzeug, nie in den Spielcode).
   - Nein: NC (nicht kommerziell), „Personal“-Lizenzen, Lizenzen mit EU-Ausschluss, unklare Herkunft.
   Bei „Nein“: ehrlich sagen und die beste kostenlose Alternative vorschlagen.
2. Bei UE-Plugins zusätzlich: unterstützte UE-Version (5.8?), letzter Commit, gepinnte Version.
   Eintrag in `Scripts/Tools/Plugins.json` und Bewertung in `docs/PLUGINS.md`. Keine Plugin-Dateien committen.
3. Zeile in der passenden Tabelle von `CREDITS.md`: Name | Quelle (URL) | Lizenz | Verwendung (Pfad).
4. KI-generierte Inhalte im Verwendungsfeld als „KI-generiert (Modell)“ markieren.

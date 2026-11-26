# ComfyUI – Workflows und Prompt-Listen

Erlaubte Modelle: nur die in `docs/KI_LIZENZEN.md` mit ✅ (hier: **FLUX.1-schnell**, Fallback **SDXL**). Kein FLUX.1-dev. Jedes neue Modell zuerst in `CREDITS.md` eintragen.

## Inhalt

| Datei | Zweck |
|---|---|
| `workflows/WF_Concept_FluxSchnell.json` | Zum **Reinziehen** in ComfyUI (manuelles Arbeiten, Prompt ändern, Queue) |
| `workflows/WF_Concept_FluxSchnell_api.json` | API-Vorlage für das Batch-Skript (4 Steps, CFG 1, euler/simple) |
| `workflows/WF_Concept_SDXL_api.json` | API-Vorlage SDXL (28 Steps, CFG 6, dpmpp_2m/karras, mit Negativ-Prompt) |
| `prompts/moodboards.json` | 11 Regionen × 8 Bilder = 88 |
| `prompts/characters.json` | 15 Hauptfiguren + Spieler-Beispiel × (Porträt, Ganzkörper, 6 Emotionen) = 128 |
| `prompts/creatures.json` | 20 Kreaturen × 2 = 40 |
| `prompts/houses.json` | 4 Türme × (Wappen, Uniform, Gemeinschaftsraum) = 12 |
| `run_comfy.py` | Batch: schickt alle Prompts an ComfyUI, speichert nach `art/concepts/`, schreibt `manifest.json` |
| `make_sheets.py` | Setzt Übersichtsblätter zusammen (Moodboard-Raster, Ausdrucks-Sheet mit 6 Emotionen) |

Namensschema Workflows: `WF_<Zweck>_<Modell>.json`.

## So erzeugst du alle Bilder (Windows)

1. ComfyUI starten: `D:\AI\ComfyUI_windows_portable\run_nvidia_gpu.bat` (Browser-Fenster darf offen bleiben). UE-Editor vorher schließen, beide brauchen den VRAM.
2. PowerShell im Repo-Ordner öffnen:
   ```powershell
   cd D:\Dev\Ewigfels
   $py = "D:\AI\ComfyUI_windows_portable\python_embeded\python.exe"
   ```
3. Erst schauen, was passiert (erzeugt nichts):
   ```powershell
   & $py Scripts\ComfyUI\run_comfy.py --list all --dry-run
   ```
4. Kurztest mit einer Region (8 Bilder, ca. 1–2 Minuten):
   ```powershell
   & $py Scripts\ComfyUI\run_comfy.py --list moodboards --only R01
   ```
   Bilder liegen danach in `art\concepts\moodboards\R01_Ewigfels\`.
5. Alles (268 Bilder, bei ca. 8–12 s pro Bild etwa **40–60 Minuten**):
   ```powershell
   & $py Scripts\ComfyUI\run_comfy.py --list all
   ```
   Abbrechen mit `Strg+C` ist jederzeit ok. Beim nächsten Start werden fertige Bilder übersprungen.
6. Figuren mit mehr Auswahl (FLUX schnell hält Gesichter nicht perfekt gleich, darum 3 Varianten pro Bild):
   ```powershell
   & $py Scripts\ComfyUI\run_comfy.py --list characters --variants 3
   ```
7. Übersichtsblätter bauen:
   ```powershell
   & $py Scripts\ComfyUI\make_sheets.py            # nimmt Variante 0
   & $py Scripts\ComfyUI\make_sheets.py --variant 2
   ```
   Ergebnis: `_SHEET.jpg` pro Region/Kreatur/Turm, `_EXPRESSIONS.jpg` pro Figur.
8. Die besten Bilder pro Region in `KEY_01.png`, `KEY_02.png` umbenennen (Maßstab für Phase 4). Danach OneDrive-Backup laufen lassen (`Scripts\Tools\Backup-ToOneDrive.ps1`).

### Nützliche Optionen

| Option | Wirkung |
|---|---|
| `--only C02_Lio,R04` | nur Jobs mit diesen ID-Anfängen |
| `--variants 3` | 3 Bilder pro Job mit verschiedenen Seeds |
| `--model sdxl` | SDXL statt FLUX (schneller, schwächer bei Gesichtern und Text-Treue) |
| `--ckpt <datei>.safetensors` | anderer Checkpoint-Dateiname (nur All-in-one-Checkpoints; GGUF braucht einen eigenen Workflow) |
| `--server http://127.0.0.1:8188` | andere ComfyUI-Adresse |

### Fehler

| Meldung | Lösung |
|---|---|
| „ComfyUI nicht erreichbar“ | `run_nvidia_gpu.bat` läuft nicht oder anderer Port |
| „lehnt den Workflow ab … ckpt_name“ | Modelldatei heißt anders → `--ckpt <Dateiname>` (Name wie im ComfyUI-Dropdown) |
| `out of memory` | `--lowvram` in `run_nvidia_gpu.bat`, siehe SETUP_WINDOWS §6.3 |
| Bilder zu ähnlich | `--variants 3` und das Beste wählen; Prompt in der JSON schärfen |

## Prompts ändern

Die Listen sind normales JSON. Ein Prompt = `shot.prompt` + `context` (Region) bzw. `{desc}` (Figur) + `style`. Englisch, weil FLUX englische Prompts am besten versteht. Neue Region oder Figur = neuer Eintrag in `groups`. Keine geschützten Namen oder Marken in Prompts.

## Lizenz der Ergebnisse

FLUX.1-schnell ist Apache 2.0, die Bilder darfst du kommerziell nutzen. Sie gelten trotzdem als **KI-generiert**: `manifest.json` pro Ordner speichert Prompt, Seed und Modell. Wenn ein Bild ins Spiel übernommen wird (z. B. als Gemälde-Textur), kommt es mit Pfad in `CREDITS.md`.

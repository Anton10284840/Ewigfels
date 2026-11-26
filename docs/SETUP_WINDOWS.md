# Phase 0: Einrichtung unter Windows 11

**Dein System:** Ryzen 7 9700X (8 Kerne), RTX 5070 (12 GB VRAM, Blackwell), 32 GB DDR5, Windows 11.
**Engine:** Unreal Engine **5.8** (aktuelle stabile Version, erschienen Juni 2026).
**Dauer:** ca. 3–5 Stunden, davon das meiste Download und Shader-Kompilierung.

> Lizenzen aller Tools: siehe [`KI_LIZENZEN.md`](KI_LIZENZEN.md). Kurz: **Hunyuan3D ist in der EU nicht erlaubt**, wir nehmen TripoSG. Cascadeur Free ist nicht kommerziell.

---

## 0. Speicherplatz planen (du hast 2 TB SSD: reicht für alles)

| Was | Größe ca. | Laufwerk |
|---|---|---|
| Unreal Engine 5.8 (ohne Debug-Symbole) | 60 GB | NVMe |
| + Editor-Debug-Symbole (für C++-Debugging) | +70 GB | NVMe, optional |
| Visual Studio 2022 + SDKs | 35 GB | C: |
| Derived Data Cache (Shader) | 20–80 GB | NVMe |
| ComfyUI + FLUX.1-schnell + SDXL | 45 GB | beliebig, NVMe schneller |
| TripoSG / Stable Fast 3D (in WSL2) | 25 GB | beliebig |
| Blender, FreeMoCap, Python | 5 GB | C: |
| Ewigfels-Projekt (wächst) | 50–300 GB | NVMe |

**Minimum zum Start: 250 GB frei auf einer NVMe-SSD. Komfortabel: 1 TB.** Auf einer HDD ist UE unbenutzbar langsam.

Ordner-Plan (Beispiel, Laufwerk anpassen):
```
D:\Epic\UE_5.8          Engine
D:\Dev\Ewigfels         Projekt (= Git-Repo)   <-- NICHT im OneDrive-Ordner!
D:\AI\ComfyUI           ComfyUI Portable
D:\Tools\freemocap-venv FreeMoCap
```

> **Warum nicht im OneDrive-Ordner?** UE schreibt ständig tausende Dateien (Intermediate, Saved, DDC). OneDrive sperrt Dateien beim Hochladen, das führt zu kaputten Assets und Build-Fehlern. Gesichert wird per Skript nach OneDrive (Abschnitt 9).

---

## 1. Windows vorbereiten

PowerShell **als Administrator** öffnen (Startmenü → „PowerShell“ → Rechtsklick → Als Administrator):

```powershell
# Lange Pfade erlauben (UE-Pfade werden lang)
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name LongPathsEnabled -Value 1 -PropertyType DWORD -Force

# winget aktuell? (sollte eine Version ausgeben)
winget --version
```

**NVIDIA-Treiber:** NVIDIA App installieren (https://www.nvidia.com/de-de/software/nvidia-app/) → Treiber → **Studio-Treiber** (stabiler für UE/Blender/KI). Danach prüfen:
```powershell
nvidia-smi
```
Muss „NVIDIA GeForce RTX 5070“ und „12227MiB“ (o. ä.) zeigen.

Optional, beschleunigt Builds deutlich, senkt aber den Virenschutz für diese Ordner. Deine Entscheidung:
```powershell
Add-MpPreference -ExclusionPath "D:\Dev\Ewigfels", "D:\Epic\UE_5.8"
```

---

## 2. Basis-Tools per Skript (Git, LFS, VS 2022, Epic Launcher, Blender, Python, 7-Zip)

Projektordner aus der Library nach `D:\Dev\Ewigfels` kopieren, dann in der Admin-PowerShell:

```powershell
cd D:\Dev\Ewigfels
Set-ExecutionPolicy -Scope Process Bypass
.\Scripts\Tools\Install-Tools.ps1
```

Das Skript installiert:
- **Git** (inkl. Git LFS und Git Credential Manager), setzt `core.longpaths` und Branch `main`
- **Visual Studio 2022 Community** mit: *Spieleentwicklung mit C++*, *Desktopentwicklung mit C++*, *.NET-Desktopentwicklung*, MSVC v143, Windows 11 SDK, *Unreal-Engine-Installer* und *IDE-Unterstützung für Unreal Engine*
- **Epic Games Launcher**, **Blender**, **Python 3.12**, **7-Zip**

Der VS-Teil dauert 20–40 Min. **Danach Windows neu starten.**

Manuell, falls winget zickt: Visual Studio Installer → *Ändern* → die drei Workloads oben anhaken → rechts unter *Spieleentwicklung mit C++* zusätzlich „Unreal Engine-Installationsprogramm“ und „IDE-Unterstützung für Unreal Engine“.

> UE 5.8 Release Notes → Abschnitt „Platform SDK Upgrades“ nennt die exakt geforderte MSVC-Version. Wenn VS meldet, dass es veraltet ist: Visual Studio Installer → *Aktualisieren*.

---

## 3. Unreal Engine 5.8

1. Epic Games Launcher starten, mit Epic-Konto anmelden (kostenlos).
2. Links **Unreal Engine** → oben **Bibliothek** → neben „ENGINE-VERSIONEN“ auf **+** → Version **5.8.x** (höchste) wählen → **Installieren**.
3. Pfad: `D:\Epic` (der Launcher legt `UE_5.8` darin an).
4. Vor dem Start auf **Optionen**:
   - ✅ Core Components, ✅ Starter Content, ✅ Templates and Feature Packs
   - ✅ **Editor symbols for debugging** – nur wenn ≥ 70 GB extra frei sind (sonst siehst du bei C++-Abstürzen keine Zeilennummern)
   - ❌ Android, iOS, Linux, Mac (spart ~20 GB)
5. Installieren (ca. 30–60 Min.).

Zusätzlich über **Fab** (im Launcher oder fab.com), alles kostenlos, „Zur Bibliothek hinzufügen“:
- **Game Animation Sample** (Motion-Matching-Basis, brauchen wir in Phase 3)
- Bei Gratis-Assets immer die **Standard License** wählen, nicht „Personal“.

---

## 4. Testprojekt kompilieren (Ziel von Phase 0)

Liegt fertig in `D:\Dev\Ewigfels` (`Ewigfels.uproject`, `Source/`, `Config/`).

**Variante A, per Skript:**
```powershell
cd D:\Dev\Ewigfels
.\Scripts\Tools\Check-Environment.ps1 -EngineDir "D:\Epic\UE_5.8"
.\Scripts\Tools\Build-Ewigfels.ps1    -EngineDir "D:\Epic\UE_5.8"
.\Scripts\Tools\Test-Ewigfels.ps1     -EngineDir "D:\Epic\UE_5.8"
```

**Variante B, per Klick:**
1. Rechtsklick auf `Ewigfels.uproject` → **Generate Visual Studio project files**. (Fehlt der Eintrag: Rechtsklick → *Weitere Optionen anzeigen*.)
2. `Ewigfels.sln` öffnen. Oben: **Development Editor** und **Win64**.
3. Projektmappen-Explorer: Rechtsklick auf **Ewigfels** → **Als Startprojekt festlegen**.
4. **Strg+Umschalt+B** (Erstellen). Erwartet: „1 erfolgreich“.
5. **F5** startet den Editor. **Der erste Start kompiliert Shader: 20–60 Min. bei 8 Kernen.** Normal.
6. Im Editor: *File → New Level → Basic* → *File → Save Current Level As* → `Content/Maps/L_TestRoom`.
7. **Play (Alt+P)**. Oben links erscheint gold: **„Ewigfels: C++-Modul geladen (Phase 0 OK)“**.
8. Test: *Tools → Session Frontend → Automation* → „Ewigfels.Smoke“ anhaken → **Start Tests** → grün.

Wenn du gefragt wirst „Ewigfels modules are missing or built with a different engine version. Rebuild?“ → **Ja**.
Wenn der Build fehlschlägt: Fehlerausgabe (Fenster „Ausgabe“) komplett kopieren und mir schicken.

**Rendering-Check:** *Edit → Project Settings → Rendering*: „Dynamic Global Illumination Method“ = Lumen, „Support Hardware Ray Tracing“ = an, „Shadow Map Method“ = Virtual Shadow Maps, Anti-Aliasing = TSR. Ist bereits in `Config/DefaultEngine.ini` gesetzt.

> Ehrliche Einordnung zur Performance: 1440p mit Hardware-Lumen, Nanite und VSM ist für eine 5070 machbar, aber nicht nativ. Wir rendern intern bei ~67–75 % und skalieren mit TSR hoch, optional später DLSS (NVIDIA-Plugin, kostenlos). Engster Punkt sind die **12 GB VRAM** (Texturen, MetaHuman-Haare). Das planen wir in Phase 4 mit Budgets ein.

---

## 4.1 Plugins und Claude-Editor-Steuerung (nach dem ersten erfolgreichen Build)

Erst wenn Schritt 4 grün ist, sonst suchen wir Fehler an zwei Stellen gleichzeitig.

```powershell
cd D:\Dev\Ewigfels
.\Scripts\Tools\Install-Plugins.ps1                       # DlgSystem + Flow (Quellcode, gepinnt)
.\Scripts\Tools\Build-Ewigfels.ps1 -EngineDir "D:\Epic\UE_5.8"   # Plugins werden mitkompiliert
```

**Unreal MCP (Claude steuert den Editor):**
1. Editor öffnen. *Edit → Plugins* → suchen „Unreal MCP“ und „All Toolsets“ → beide angehakt (stehen schon in `Ewigfels.uproject`). Falls UE einen Neustart verlangt: ja.
2. *Edit → Editor Preferences → Agentic Workflows → Model Context Protocol*: „Auto Start Server“ an, Port 8000.
3. Läuft der Server nicht: Konsole (Taste `^`) → `ModelContextProtocol.StartServer`.
4. Claude Code im Ordner `D:\Dev\Ewigfels` starten (bei offenem Editor). Die `.mcp.json` im Repo verbindet automatisch; beim ersten Mal „Erlauben“ wählen. Prüfen mit `/mcp`.

> Experimentell: Epic kann die Schnittstelle ändern. Der Server hat keine Anmeldung, also nie Port 8000 in der Firewall freigeben.

---

## 5. Blender

Ist nach Schritt 2 installiert. Einmal öffnen → *Edit → Preferences*:
- *System → Cycles Render Devices* → **OptiX** → RTX 5070 anhaken
- *Add-ons*: „Import-Export: FBX format“ und „glTF 2.0“ sind Standard und aktiv
- *Save Preferences*

---

## 6. ComfyUI + FLUX.1-schnell + SDXL (12 GB VRAM)

**Wichtig für RTX 50xx:** Blackwell braucht PyTorch mit **CUDA 12.8 oder neuer**. Die aktuelle ComfyUI-Portable-Version bringt das mit. Alte Anleitungen mit „cu121/cu124“ funktionieren auf der 5070 nicht.

### 6.1 Installation
1. https://github.com/comfyanonymous/ComfyUI/releases → neuestes Release → `ComfyUI_windows_portable_nvidia.7z` laden.
2. Mit 7-Zip nach `D:\AI\` entpacken → ergibt `D:\AI\ComfyUI_windows_portable`.
3. Starten: `run_nvidia_gpu.bat` → Browser öffnet `http://127.0.0.1:8188`.
4. Prüfen, ob die GPU erkannt wird, im Konsolenfenster: `Device: cuda:0 NVIDIA GeForce RTX 5070`.
5. Updates: `update\update_comfyui.bat`.

### 6.2 Modelle (nur kommerziell nutzbare!)
Ordner: `D:\AI\ComfyUI_windows_portable\ComfyUI\models\`

| Datei | Quelle (Hugging Face) | Ziel-Ordner | Größe | Lizenz |
|---|---|---|---|---|
| `flux1-schnell-fp8.safetensors` | `Comfy-Org/flux1-schnell` | `checkpoints\` | 17 GB | Apache 2.0 |
| `sd_xl_base_1.0.safetensors` | `stabilityai/stable-diffusion-xl-base-1.0` | `checkpoints\` | 6,9 GB | OpenRAIL++-M |
| `RealESRGAN_x4plus.pth` | GitHub `xinntao/Real-ESRGAN` Releases | `upscale_models\` | 64 MB | BSD-3 |

Download per Kommandozeile (in `cmd` im Ordner `ComfyUI_windows_portable`):
```bat
python_embeded\python.exe -m pip install -U huggingface_hub
python_embeded\Scripts\hf.exe download Comfy-Org/flux1-schnell flux1-schnell-fp8.safetensors --local-dir ComfyUI\models\checkpoints
python_embeded\Scripts\hf.exe download stabilityai/stable-diffusion-xl-base-1.0 sd_xl_base_1.0.safetensors --local-dir ComfyUI\models\checkpoints
```
(Oder einfach im Browser auf der Hugging-Face-Seite unter „Files“ herunterladen und in den Ordner legen.)

**Finger weg von** `FLUX.1-dev`, `FLUX.1-Kontext-dev` und allem mit „non-commercial“ in der Lizenz.

### 6.3 VRAM-Hinweise für 12 GB
- **FLUX.1-schnell fp8** passt nicht komplett in 12 GB. ComfyUI lagert automatisch Teile (Text-Encoder) in den RAM aus. Mit 32 GB RAM läuft es, ca. 6–12 s pro 1024×1024-Bild bei **4 Steps, CFG 1.0** (schnell braucht nicht mehr).
- Falls „out of memory“: `run_nvidia_gpu.bat` mit Editor öffnen und `--lowvram` an die Startzeile anhängen. Oder GGUF-Variante (`city96/FLUX.1-schnell-gguf`, Q6_K, ~10 GB) über den Custom Node „ComfyUI-GGUF“.
- **SDXL** läuft komplett im VRAM, ~4 s pro Bild. Besser für **kachelbare Texturen** und Decals, FLUX besser für **Concept Art und Moodboards**.
- Nicht gleichzeitig ComfyUI und den UE-Editor mit großer Szene offen haben: beide wollen den VRAM.

### 6.4 Erster Test
Im ComfyUI-Menü *Workflow → Browse Templates → Flux → Flux Schnell* → bei „Load Checkpoint“ `flux1-schnell-fp8` wählen → Prompt: `misty gothic academy on a cliff above a lake, golden hour, volumetric light, cinematic, photorealistic` → **Queue**. Ergebnis landet in `ComfyUI\output\`.
Workflows, die wir verwenden, speichern wir als JSON in `Scripts/ComfyUI/`.

---

## 7. 3D aus Bildern: TripoSG (statt Hunyuan3D)

Hunyuan3D-2 ist laut Lizenz in der EU nicht erlaubt. TRELLIS braucht 16–24 GB VRAM und hat eine nicht-kommerzielle Abhängigkeit. Deshalb **TripoSG (MIT)**.

Diese Tools sind auf Linux entwickelt, CUDA-Erweiterungen kompilieren unter Windows oft nicht. Am zuverlässigsten läuft es in **WSL2** (Ubuntu in Windows, nutzt deine GPU direkt).

### 7.1 WSL2 einrichten (Admin-PowerShell)
```powershell
wsl --install -d Ubuntu-24.04
```
Neustart, Ubuntu startet, Benutzername + Passwort festlegen. Dann **in Ubuntu**:
```bash
nvidia-smi          # muss die RTX 5070 zeigen (Treiber kommt von Windows, NICHT in Ubuntu installieren)
sudo apt update && sudo apt install -y git python3-venv python3-dev build-essential
```

### 7.2 TripoSG installieren (in Ubuntu)
```bash
mkdir -p ~/ai && cd ~/ai
git clone https://github.com/VAST-AI-Research/TripoSG.git
cd TripoSG
python3 -m venv .venv && source .venv/bin/activate
pip install torch torchvision --index-url https://download.pytorch.org/whl/cu128   # Blackwell braucht cu128+
pip install -r requirements.txt
```
Test (Modelle werden beim ersten Lauf automatisch geladen):
```bash
python -m scripts.inference_triposg --image-input assets/example_data/hjswed.png --output-path ./test.glb
```
Die `.glb` liegt dann unter `\\wsl$\Ubuntu-24.04\home\<dein-name>\ai\TripoSG\test.glb` und lässt sich in Blender importieren.
Falls sich Befehle im TripoSG-README inzwischen geändert haben, gilt das README.

**Ergebnis ist reine Geometrie.** Pipeline: Bild (FLUX) → TripoSG → Blender (Decimate/Retopo, UV) → Textur per Projektion/ArmorPaint → FBX → UE (Nanite an).

### 7.3 Zweitoption: Stable Fast 3D (texturiert, schnell, ~6 GB VRAM)
Lizenz: kostenlos bis 1 Mio. $ Jahresumsatz, für kommerzielle Nutzung bei Stability registrieren. Installation folgt bei Bedarf in Phase 1.

---

## 8. FreeMoCap (Bewegungen per Webcam)

In normaler PowerShell (kein Admin nötig):
```powershell
py -3.12 -m venv D:\Tools\freemocap-venv
D:\Tools\freemocap-venv\Scripts\Activate.ps1
pip install freemocap
freemocap
```
Es öffnet sich die FreeMoCap-Oberfläche.

**Ehrlich zur Qualität:** Mit **einer** Webcam bekommst du brauchbare, aber wackelige Daten (Tiefe wird geschätzt). Gut wird es mit **2–3 Kameras** aus verschiedenen Winkeln. Dein **Android-Handy** kann eine davon sein: Windows 11 → *Einstellungen → Bluetooth und Geräte → Mobile Geräte* → Handy über „Smartphone-Link“ koppeln → „Als verbundene Kamera verwenden“. Aufnahme-Setup (Licht, Kleidung, Kalibrierung mit ChArUco-Board) beschreibe ich in Phase 3.

**Noch offen:** Welche Webcam hast du? Dein Pixel 10 Pro kann als zweite Kamera dienen (USB-Webcam-Modus, siehe 8a).

---

## 8a. Gesichtsanimation mit dem Pixel 10 Pro (MetaHuman Animator, UE 5.8)

Geprüft am 26.11.2026 gegen die Epic-Doku zu MetaHuman 5.8. **Kurz: Echtzeit-Gesichtstracking am Handy geht mit dem Pixel nicht. Offline-Aufnahmen gehen, am sichersten mit dem Pixel als USB-Webcam.**

| Weg | Pixel 10 Pro | Warum |
|---|---|---|
| Live Link Face (Android), **Echtzeit**-Tracking | ❌ geht nicht | Epic: „Real-time facial tracking works only on devices with a Qualcomm Snapdragon 8 Gen 1 (or later) processor.“ Das Pixel 10 Pro hat einen Google **Tensor G5**. Pixel-Geräte stehen auf keiner Liste, ein Forumsnutzer meldet beim Pixel 9a „hardware acceleration not supported“. |
| Live Link Face (Android), **Takes aufnehmen** (neu in 5.8) für die Offline-Verarbeitung am PC | ⚠️ unsicher | Neu in 5.8 sind Take-Aufnahme mit Video und Ton und der Export per WLAN an Unreal. Epic garantiert das aber nur für getestete Snapdragon-Geräte. Installieren und ausprobieren kostet nichts, verlassen würde ich mich darauf nicht. |
| **Pixel als USB-Webcam** → MetaHuman Animator „Mono Video“ (offline) | ✅ **empfohlen** | MetaHuman Animator nimmt jede UVC-Kamera unter Windows. Das Pixel meldet sich im Webcam-Modus als UVC-Kamera. Die Verarbeitung läuft auf deiner RTX 5070, nicht auf dem Handy. |
| Video mit der normalen Pixel-Kamera-App filmen, später am PC importieren | ⚠️ wahrscheinlich | Die Mono-Video-Verarbeitung ist kamera-unabhängig. Den genauen Import-Weg (Capture Manager) testen wir in Phase 3 mit einer Probeaufnahme. |
| **Audio-driven** Lip-Sync (Ton → Gesicht), in 5.8 mit neuem Echtzeit-Modell | ✅ | Braucht nur ein Mikrofon oder eine WAV-Datei. Erzeugt auch Blinzeln und Emotionen aus dem Ton. Das ist unser Standardweg für die vielen NPC-Dialoge (TTS-WAV → Gesicht). |
| Körper + Gesicht aus **einer** Kamera (Plugin „MetaHuman Animator Markerless Motion Capture“, Meshcapade, Fab) | ⚠️ experimentell | Ist in 5.8 neu und als *Experimental* markiert, nur unter Windows. Preis und Lizenz des Plugins auf Fab prüfen, bevor wir es einplanen. |

**Ehrlich zur Qualität:** Ein iPhone liefert Tiefendaten (TrueDepth), ein Pixel nur ein normales Videobild. MetaHuman Animator 5.8 kann die Tiefe beim Verarbeiten zwar automatisch schätzen. Lippen, Kiefer und feine Mimik werden aus Mono-Video aber etwas ungenauer als mit Tiefendaten. Für Hauptfiguren in Nahaufnahmen planen wir in Phase 3 Nacharbeit per Control Rig ein.

### 8a.1 Pixel 10 Pro als USB-Webcam einrichten
1. Pixel entsperren und mit einem **USB-C-Datenkabel** an den PC anschließen (am besten das mitgelieferte, reine Ladekabel gehen nicht).
2. Am Handy die Benachrichtigung **„Gerät wird über USB geladen“** antippen → unter „USB verwenden für“ **„Webcam“** wählen.
3. Die Webcam-Benachrichtigung antippen → Vorschau öffnet sich → **HQ** einschalten (bessere Bildqualität). Rückkamera wählen, die ist schärfer als die Frontkamera.
4. Prüfen in Windows: *Einstellungen → Bluetooth und Geräte → Kameras* → das Pixel muss dort auftauchen.
5. In UE 5.8: *Edit → Plugins* → nach „MetaHuman“ suchen → **MetaHuman Animator** aktivieren → Editor neu starten. Die eigentliche Aufnahme und Verarbeitung (MetaHuman Performance Asset) zeige ich dir Schritt für Schritt in Phase 3.

Alternativ ohne Kabel: Windows 11 *Smartphone-Link* → „Als verbundene Kamera verwenden“. Das ist eine virtuelle Kamera, braucht laut Epic Windows 11 (hast du) und läuft per WLAN. Sie ist weniger stabil als USB.

### 8a.2 Aufnahme-Tipps (gilt für alle Mono-Video-Wege)
- Handy auf ein **Stativ in Augenhöhe**, 50–70 cm vor dem Gesicht, Gesicht füllt etwa die Hälfte des Bildes.
- **Weiches Licht von vorne** (Fenster oder Lampe hinter einem Bettlaken), kein Gegenlicht, keine harten Schatten.
- Epic empfiehlt, **Autofokus und automatische Belichtung abzuschalten**. Im Pixel-Webcam-Modus geht das nicht direkt. Darum gleichmäßiges Licht und während der Aufnahme nicht stark vor- und zurückbewegen.
- Ton: separates USB-Mikro oder Headset ist besser als das Handy-Mikro, wenn du den Ton für Audio-driven nutzen willst.

Quellen: [MetaHuman 5.8 Release Notes](https://dev.epicgames.com/documentation/metahuman/metahuman-5-8-release-notes-in-unreal-engine) · [Live Link Face App](https://dev.epicgames.com/documentation/metahuman/live-link-face-application-for-metahuman-animator-in-unreal-engine) · [Capture Device Requirements](https://dev.epicgames.com/documentation/metahuman/capture-device-requirements) · [MetaHuman 5.8 Ankündigung](https://www.metahuman.com/news/metahuman-5-8-is-now-available) · [Forum: Live Link Face für Android](https://forums.unrealengine.com/t/live-link-face-is-now-available-on-android/2540637)

---

## 9. GitHub-Repo + OneDrive-Backup einrichten

**Aufteilung:**

| Wo | Was |
|---|---|
| **GitHub (privat)** | `Source/`, `Config/`, `Scripts/`, `docs/`, `Content/Data/**/*.json`, `Ewigfels.uproject`, `CREDITS.md`, `PROGRESS.md`, ComfyUI-Workflows und Prompts |
| **Nur lokal + OneDrive-Backup** | `Content/**/*.uasset`, `*.umap`, `art/` (Bilder), `raw/` (Blender, FBX, WAV), KI-Modelle |

Das ist in `.gitignore` bereits so eingestellt. Grund: Das Gratis-LFS-Kontingent von GitHub reicht für UE-Assets nicht annähernd.

**Nachteil, den du kennen musst:** Assets haben damit keine Git-Historie. Wenn du ein Material kaputt speicherst, hilft nur der letzte OneDrive-Snapshot. Darum Backup mindestens nach jeder Session.

### 9.1 Repo anlegen (zuhause, 2 Minuten)
1. https://github.com/new
2. Owner: `koelleprime-afk`, Name: `ewigfels`, **Private**.
3. **Nichts** anhaken (kein README, kein .gitignore, keine Lizenz). → *Create repository*.
4. In PowerShell:
   ```powershell
   cd D:\Dev\Ewigfels
   git config --global user.name  "Dein Name"
   git config --global user.email "deine-mail@example.com"   # oder die GitHub-noreply-Adresse
   .\Scripts\Tools\Init-GitRepo.ps1 -RemoteUrl https://github.com/koelleprime-afk/ewigfels.git
   ```
   Beim ersten Push öffnet sich ein Browserfenster für den GitHub-Login (Git Credential Manager).
   Das Skript bricht ab, falls doch eine `.uasset`/`.umap` im Commit landen würde.
5. Sag mir Bescheid. Damit ich direkt ins Repo committen kann, muss die Claude-GitHub-App Zugriff auf `ewigfels` haben: https://github.com/apps/claude/installations/select_target

### 9.2 OneDrive-Backup
OneDrive muss eingerichtet sein (`echo $env:OneDrive` zeigt den Pfad). Dann nach jeder Arbeitssession, **Editor geschlossen**:
```powershell
.\Scripts\Tools\Backup-ToOneDrive.ps1            # Spiegel + Git-Bundle (sonntags zusätzlich Snapshot)
.\Scripts\Tools\Backup-ToOneDrive.ps1 -Snapshot  # erzwingt datiertes .7z-Archiv
```
Ergebnis in `OneDrive\Ewigfels-Backup\`: `current\` (Spiegel), `snapshots\` (die letzten 6 Archive), `ewigfels.bundle` (komplettes Git-Repo).

Optional automatisch täglich um 23:00 (Aufgabenplanung):
```powershell
$a = New-ScheduledTaskAction -Execute "powershell.exe" -Argument "-ExecutionPolicy Bypass -File D:\Dev\Ewigfels\Scripts\Tools\Backup-ToOneDrive.ps1"
$t = New-ScheduledTaskTrigger -Daily -At 23:00
Register-ScheduledTask -TaskName "Ewigfels-Backup" -Action $a -Trigger $t
```

> OneDrive-Gratis hat 5 GB. Ab Phase 3/4 wird das eng (Content wächst schnell auf 20–100 GB). Dann brauchst du entweder mehr OneDrive-Speicher (z. B. über ein vorhandenes Microsoft-365-Abo mit 1 TB) oder zusätzlich eine externe Festplatte als Ziel: `-Target E:\Ewigfels-Backup`.

---

## 10. Checkliste Phase 0

- [x] Freier Speicher geklärt (2 TB SSD)
- [ ] NVIDIA Studio-Treiber, `nvidia-smi` ok
- [ ] `Install-Tools.ps1` gelaufen, Neustart
- [ ] UE 5.8 installiert
- [ ] `Build-Ewigfels.ps1` grün, Editor zeigt „Phase 0 OK“, Smoke-Test grün
- [ ] `Install-Plugins.ps1` gelaufen, Build mit Plugins grün, `/mcp` in Claude Code zeigt „unreal-mcp“ verbunden
- [ ] Blender: OptiX aktiv
- [ ] ComfyUI: Flux-Schnell-Testbild erzeugt
- [ ] WSL2 + TripoSG: `test.glb` erzeugt und in Blender geöffnet
- [ ] FreeMoCap startet
- [ ] Pixel 10 Pro im USB-Webcam-Modus unter *Einstellungen → Kameras* sichtbar
- [ ] GitHub-Repo `ewigfels` angelegt, erster Push, Claude-App freigegeben
- [ ] Erstes OneDrive-Backup gelaufen

Wenn alles abgehakt ist, starten wir Phase 1 (Bibeln + Moodboards).

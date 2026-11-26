# Lizenz-Check: Tools & KI-Modelle (Stand 26.11.2026)

Maßstab: Darf ich das Spiel später **kommerziell veröffentlichen**, und darf ich das Tool **in Deutschland/EU** nutzen?
Vor jedem Download die Lizenzdatei im Repo bzw. auf Hugging Face selbst gegenlesen. Lizenzen ändern sich.

## Ergebnis auf einen Blick

| Tool / Modell | Lizenz | Kommerziell? | Entscheidung |
|---|---|---|---|
| Unreal Engine 5.8 | Epic EULA | Ja, 5 % Royalty ab 1 Mio. $ Brutto pro Produkt | ✅ nutzen |
| MetaHuman Creator/Animator | UE-EULA (seit 5.6) | Ja | ✅ nutzen |
| ComfyUI | GPL-3.0 | Ja (nur das Tool ist GPL, deine Bilder nicht) | ✅ nutzen |
| **FLUX.1-schnell** | Apache 2.0 | Ja | ✅ Hauptmodell |
| FLUX.1-dev / FLUX.1 Kontext-dev | FLUX.1 [dev] Non-Commercial | **Nein** | ❌ nicht verwenden |
| SDXL Base 1.0 | CreativeML Open RAIL++-M | Ja (mit Nutzungsbeschränkungen, z. B. keine Täuschung/Schaden) | ✅ nutzen |
| SDXL-Finetunes (Civitai) | je Modell verschieden | oft ja, manchmal nein | ⚠️ einzeln prüfen, in CREDITS.md eintragen |
| **Hunyuan3D-2 / 2.1 / 2mini** | Tencent Hunyuan 3D Community License | Gilt **nicht in der EU, UK, Südkorea** | ❌ **in Deutschland nicht erlaubt** |
| TRELLIS (v1) | MIT (Code + Gewichte) | Ja, **aber** Textur-Baking nutzt nvdiffrast (NVIDIA Source Code License, nur nicht-kommerziell) | ⚠️ Grauzone, außerdem ≥ 16 GB VRAM |
| TRELLIS.2 | MIT, Abhängigkeiten nvdiffrast/nvdiffrec mit eigener Lizenz | wie oben | ❌ braucht ≥ 24 GB VRAM, nur Linux getestet |
| **TripoSG** (VAST) | MIT | Ja | ✅ **Ersatz für Hunyuan3D** (Geometrie) |
| Stable Fast 3D (Stability) | Stability AI Community License | Ja bis 1 Mio. $ Jahresumsatz, Registrierung bei Stability nötig | ✅ Zweitoption (texturiert, schnell, ~6 GB VRAM) |
| Real-ESRGAN (+ x4plus-Modelle) | BSD-3-Clause | Ja | ✅ nutzen |
| Kokoro TTS | Apache 2.0 | Ja | ✅ für Englisch. **Kann kein Deutsch** |
| Piper TTS | Code GPL-3.0 (piper1-gpl), Stimmen je eigene Lizenz | Ja, je nach Stimme | ✅ für Deutsch: `de_DE-thorsten-*` (Datensatz CC0). Stimmlizenz im MODEL_CARD prüfen |
| Whisper / faster-whisper | MIT | Ja | ✅ nutzen |
| FreeMoCap | AGPL-3.0 | Ja (Aufnahmen gehören dir) | ✅ nutzen |
| Rokoko Vision (Free) | Proprietär | laut Rokoko-AGB prüfen | ⚠️ vor Nutzung AGB lesen |
| **Cascadeur Free** | Proprietär | **Nein, Free ist nur nicht-kommerziell** (Indie-Lizenz kostet) | ❌ für Release-Animationen; Alternative unten |
| Mixamo | Adobe-AGB | Ja, lizenzfrei in Spielen | ✅ nutzen |
| Game Animation Sample (Epic) | Fab/UE-EULA | Ja, in UE-Projekten | ✅ nutzen |
| Blender, Material Maker, ArmorPaint | GPL / MIT / zlib | Ja | ✅ nutzen (ArmorPaint: Quellcode frei, selbst bauen oder alte Gratis-Builds) |
| Fab-Inhalte | Fab Standard License / Personal | Standard: ja. Personal: nein | ⚠️ nur „Standard License“ nehmen. Megascans sind seit 2025 nicht mehr komplett gratis |
| Poly Haven, ambientCG | CC0 | Ja | ✅ |
| Sonniss GDC Bundles | Royalty-free | Ja | ✅ |
| Freesound | CC0 / CC-BY / CC-BY-NC | CC0 + CC-BY ja, **NC nein** | ⚠️ Filter auf CC0/CC-BY |

## Was das konkret für dich heißt

1. **Hunyuan3D fällt weg.** Die Lizenz schließt das Gebiet der EU ausdrücklich aus. Als Nutzer in Deutschland darfst du es nicht verwenden, egal ob kommerziell oder nicht.
2. **3D-aus-Bild läuft über TripoSG (MIT) + Blender.** TripoSG liefert gute Geometrie ohne Textur. Textur machen wir per Projektion (ComfyUI/SDXL-Bild → Blender „Project from View“) oder ArmorPaint. Stable Fast 3D als schnelle texturierte Alternative für kleine Props.
3. **TRELLIS nur mit Vorsicht.** Die Gewichte sind MIT, aber das Textur-Baking hängt an nvdiffrast, dessen NVIDIA-Lizenz nur nicht-kommerzielle Nutzung erlaubt. Dazu braucht TRELLIS 16 GB (v1) bzw. 24 GB (v2) VRAM, deine 5070 hat 12 GB. Empfehlung: weglassen.
4. **Cascadeur Free nicht für finale Animationen.** Ersatz: Keyframing in UE mit Control Rig (in UE 5.8 sehr brauchbar) oder Blender. Cascadeur Free darfst du zum Lernen und Vorvisualisieren nutzen, aber nichts davon ins Release übernehmen.
5. **Kokoro spricht kein Deutsch.** Deutsch: Piper (Thorsten-Stimmen). Englisch: Kokoro.
6. **KI-Bilder kennzeichnen.** Steam verlangt beim Release eine Angabe zu KI-generierten Inhalten. Rein KI-generierte Bilder sind in vielen Ländern nicht urheberrechtlich geschützt (andere dürften sie kopieren). Für Key-Art und Logo deshalb später selbst nacharbeiten.

## Quellen
- Unreal Engine 5.8 Release: https://www.unrealengine.com/news/unreal-engine-5-8-is-now-available
- Hunyuan3D-2 Lizenz: https://huggingface.co/tencent/Hunyuan3D-2/blob/main/LICENSE
- Hunyuan3D-2.1 Lizenz: https://huggingface.co/tencent/Hunyuan3D-2.1
- TRELLIS.2 README (MIT, 24 GB, Linux): https://huggingface.co/microsoft/TRELLIS.2-4B
- TRELLIS Issue „non_commercial??“ zu nvdiffrast: https://github.com/microsoft/TRELLIS/issues/298

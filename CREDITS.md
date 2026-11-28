# CREDITS

Regel: **Ohne Eintrag kommt nichts ins Projekt.** Jede externe Ressource (Asset, Sound, Musik, Modell, Code, KI-Modell, mit dem Inhalte erzeugt wurden) bekommt eine Zeile.
Lizenz-Details und Bewertung der Tools: [`docs/KI_LIZENZEN.md`](docs/KI_LIZENZEN.md).

## Software & Engine

| Name | Quelle | Lizenz | Verwendung |
|---|---|---|---|
| Unreal Engine 5.8 | https://www.unrealengine.com | Epic EULA (5 % ab 1 Mio. $) | Engine |
| MetaHuman | https://www.unrealengine.com/metahuman | UE EULA | Charaktere (ab Phase 3) |
| Blender | https://www.blender.org | GPL-3.0 | Modellieren, UV, Retopo |
| Visual Studio 2022 Community | https://visualstudio.microsoft.com | Microsoft-Lizenz (Community: Einzelentwickler ok) | C++-IDE |
| Git / Git LFS | https://git-scm.com | GPL-2.0 / MIT | Versionsverwaltung |
| Pillow | https://python-pillow.org | MIT-CMU (HPND) | `Scripts/ComfyUI/make_sheets.py` (Übersichtsblätter, nur Tool) |

## Plugins & Code (Details: [`docs/PLUGINS.md`](docs/PLUGINS.md), Versionen: `Scripts/Tools/Plugins.json`)

| Name | Quelle | Lizenz | Verwendung |
|---|---|---|---|
| Unreal MCP + All Toolsets | in UE 5.8 enthalten | UE EULA | Editor-Steuerung durch Claude (nur Editor, experimentell) |
| UE-Engine-Plugins (DataValidation, StateTree, CommonUI, Water, PoseSearch, MotionWarping, Chooser) | in UE 5.8 enthalten | UE EULA | KI, UI, Wasser, Motion Matching, Asset-Prüfung |
| UE-Engine-Plugins für das Game Animation Sample (AnimationWarping, AnimationLocomotionLibrary, AnimationLayering, BlendStack, FullBodyIK, CurveExpression, GameplayCameras, RigLogic, HairStrands, LiveLink, LiveLinkControlRig, Mover, NetworkPrediction, ChaosMover, Locomotor, MovieSceneAnimMixer, DrawDebugLibrary, SmartObjects, GameplayInteractions) | in UE 5.8 enthalten | UE EULA | Laufen, Klettern, Kamera und Geräusche der Sample-Figur |
| PCG Extended Toolkit | https://github.com/Nebukam/PCGExtendedToolkit | MIT | Prozeduraler Weltaufbau (`Plugins/ThirdParty/PCGExtendedToolkit`) |
| SUDS | https://github.com/sinbad/SUDS | MIT | Dialogsystem (`Plugins/ThirdParty/SUDS`), Ewigfels-Modul hängt davon ab |
| SPUD | https://github.com/sinbad/SPUD | MIT | optional, nicht im Einsatz (eigenes Save-System) |
| UnrealImGui | https://github.com/IDI-Systems/UnrealImGui | MIT | optional, Debug-Overlays (`Plugins/ThirdParty/ImGui`) |
| NVIDIA DLSS 4.5 Plugin | https://developer.nvidia.com/rtx/dlss | NVIDIA RTX SDK-Lizenz (Hinweispflichten vor Release prüfen) | ab Phase 4, Upscaling/Frame-Gen (`Plugins/ThirdParty/DLSS`, manuell) |
| Send to Unreal (Blender-Add-on) | https://github.com/EpicGamesExt/BlenderTools | MIT | Export Blender → UE (Tool, nicht im Spiel) |
| awesome-gamedev-agent-skills (Abhishek Barali und Mitwirkende) | https://github.com/gamedev-skills/awesome-gamedev-agent-skills (Stand c6ca6c4, 12.11.2026) | Apache 2.0 | Nur gelesen: Unreal-Tipps mit eigenen Worten in `docs/UNREAL_REGELN.md` zusammengefasst (nichts installiert, kein Code übernommen) |

## KI-Modelle (Inhalte, die damit erzeugt wurden, sind als „KI-generiert“ markiert)

| Name | Quelle | Lizenz | Verwendung |
|---|---|---|---|
| ComfyUI | https://github.com/comfyanonymous/ComfyUI | GPL-3.0 | Bildgenerierung (Tool) |
| FLUX.1-schnell | https://huggingface.co/black-forest-labs/FLUX.1-schnell | Apache 2.0 | Concept Art, Moodboards |
| SDXL Base 1.0 | https://huggingface.co/stabilityai/stable-diffusion-xl-base-1.0 | CreativeML Open RAIL++-M | Texturen, Decals |
| Real-ESRGAN x4plus | https://github.com/xinntao/Real-ESRGAN | BSD-3-Clause | Upscaling |
| TripoSG | https://github.com/VAST-AI-Research/TripoSG | MIT | 3D-Props aus Bildern |
| FreeMoCap | https://github.com/freemocap/freemocap | AGPL-3.0 | Motion Capture (Tool) |

## Assets

| Name | Quelle | Lizenz | Verwendung (Pfad im Projekt) |
|---|---|---|---|
| Game Animation Sample (Epic Games, UE 5.8) | Fab / Epic Games Launcher | UE-Content (Fab Standard-Lizenz, nur in Unreal-Projekten) | Sandbox-Figur mit Motion Matching, ca. 2.900 Dateien: `Content/Blueprints`, `Content/Characters/{UEFN_Mannequin,UE5_Mannequins,UE4_Mannequin,Echo,Paragon}`, `Content/MetaHumans/{Common,Kellan}`, `Content/Audio/Foley`, `Content/Input`, `Content/Levels/LevelPrototyping`, `Content/Misc` |

## Audio & Musik

| Name | Urheber | Quelle | Lizenz | Verwendung |
|---|---|---|---|---|
| _noch keine_ | | | | |

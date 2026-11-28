"""
Zauber-Effekte (Niagara) neu bauen. Der eigentliche Aufbau steht in C++ im Editor-Modul
(Source/EwigfelsEditor/Private/EwigfelsSpellVfx.cpp, Befehl Ewigfels.VFX.Erstellen), weil die Niagara-Schnittstelle
von UE 5.8 (UNiagaraExternalEditUtilities) nur dort vollständig nutzbar ist.

Aufruf (Editor zu, mit Oberfläche, damit auch die Stapel-Prüfung läuft):
  UnrealEditor.exe Ewigfels.uproject -ExecutePythonScript="<Pfad>/create_spell_vfx.py"
Nur bestimmte Weisen: Umgebungsvariable EWIGFELS_VFX="funkenstoss frostweise".
Ergebnis: /Game/VFX/Spells/NS_<id> und NS_<id>_Impact, Meldungen im Log unter "[Ewigfels VFX]".
"""
import os
import unreal

ids = os.environ.get("EWIGFELS_VFX", "").strip()
unreal.SystemLibrary.execute_console_command(None, ("Ewigfels.VFX.Erstellen " + ids).strip())

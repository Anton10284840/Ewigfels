"""
Phase 3: legt im Editor an (einmalig, kann gefahrlos wiederholt werden):
  - /Game/VFX/Materials/M_EwigfelsGlow   (additiv, unbeleuchtet: Color, Glow, Opacity)  – Magie, Augen, Hände
  - /Game/VFX/Materials/M_EwigfelsBeam   (additiv, weicher Rand: Color, Glow, Opacity)   – Blitze, Strahlen, Funken
  - /Game/VFX/Materials/M_EwigfelsSolid  (beleuchtet: Color, Roughness)                 – Platzhalter-Körper, Arena
  - /Game/Maps/L_Phase3_Test             (Testarena + Startpunkt) und trägt sie als Start-Map ein

Ausführen (Editor offen): Tools > Execute Python Script… > diese Datei
oder beim Start: UnrealEditor.exe Ewigfels.uproject -ExecutePythonScript="<Pfad>/create_phase3_testmap.py"
Ohne dieses Skript läuft das Spiel auch (graue Platzhalter, Arena erscheint in jedem leeren Level).
"""
import os
import re
import unreal

MAT_DIR = "/Game/VFX/Materials"
MAP_PATH = "/Game/Maps/L_Phase3_Test"

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eal = unreal.EditorAssetLibrary


def log(msg):
    unreal.log("[Ewigfels Phase3] " + msg)


def param_vector(mat, name, default, x, y):
    node = mel.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, x, y)
    node.set_editor_property("parameter_name", name)
    node.set_editor_property("default_value", default)
    return node


def param_scalar(mat, name, default, x, y):
    node = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, x, y)
    node.set_editor_property("parameter_name", name)
    node.set_editor_property("default_value", default)
    return node


def multiply(mat, a, b, x, y):
    node = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, x, y)
    mel.connect_material_expressions(a, "", node, "A")
    mel.connect_material_expressions(b, "", node, "B")
    return node


def fresh_material(name):
    path = MAT_DIR + "/" + name
    if eal.does_asset_exist(path):
        eal.delete_asset(path)
    return asset_tools.create_asset(name, MAT_DIR, unreal.Material, unreal.MaterialFactoryNew())


def make_glow():
    mat = fresh_material("M_EwigfelsGlow")
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_ADDITIVE)
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mat.set_editor_property("two_sided", True)
    color = param_vector(mat, "Color", unreal.LinearColor(1.0, 0.85, 0.55, 1.0), -900, 0)
    glow = param_scalar(mat, "Glow", 10.0, -900, 200)
    opacity = param_scalar(mat, "Opacity", 1.0, -900, 320)
    # Weicher Rand: Mitte schwächer, Kante heller (Kugeln wirken wie Licht statt wie Plastik)
    fresnel = mel.create_material_expression(mat, unreal.MaterialExpressionFresnel, -900, 450)
    fresnel.set_editor_property("exponent", 2.5)
    add = mel.create_material_expression(mat, unreal.MaterialExpressionAdd, -650, 450)
    add.set_editor_property("const_b", 0.45)
    mel.connect_material_expressions(fresnel, "", add, "A")
    m1 = multiply(mat, color, glow, -650, 100)
    m2 = multiply(mat, m1, opacity, -450, 150)
    m3 = multiply(mat, m2, add, -250, 200)
    mel.connect_material_property(m3, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mel.recompile_material(mat)
    eal.save_loaded_asset(mat)
    log("M_EwigfelsGlow angelegt")


def make_beam():
    # Blitz-/Strahlteile: Mitte hell, Rand weich ausgeblendet (umgekehrter Fresnel), additiv, ohne Licht
    mat = fresh_material("M_EwigfelsBeam")
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_ADDITIVE)
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mat.set_editor_property("two_sided", True)
    color = param_vector(mat, "Color", unreal.LinearColor(0.6, 0.8, 1.0, 1.0), -900, 0)
    glow = param_scalar(mat, "Glow", 20.0, -900, 200)
    opacity = param_scalar(mat, "Opacity", 1.0, -900, 320)
    fresnel = mel.create_material_expression(mat, unreal.MaterialExpressionFresnel, -900, 450)
    fresnel.set_editor_property("exponent", 1.5)
    inv = mel.create_material_expression(mat, unreal.MaterialExpressionOneMinus, -650, 450)
    mel.connect_material_expressions(fresnel, "", inv, "")
    soft = mel.create_material_expression(mat, unreal.MaterialExpressionPower, -450, 450)
    soft.set_editor_property("const_exponent", 2.0)
    mel.connect_material_expressions(inv, "", soft, "Base")
    m1 = multiply(mat, color, glow, -650, 100)
    m2 = multiply(mat, m1, opacity, -450, 150)
    m3 = multiply(mat, m2, soft, -250, 200)
    mel.connect_material_property(m3, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mel.recompile_material(mat)
    eal.save_loaded_asset(mat)
    log("M_EwigfelsBeam angelegt")


def make_solid():
    mat = fresh_material("M_EwigfelsSolid")
    color = param_vector(mat, "Color", unreal.LinearColor(0.3, 0.3, 0.3, 1.0), -600, 0)
    rough = param_scalar(mat, "Roughness", 0.7, -600, 200)
    mel.connect_material_property(color, "", unreal.MaterialProperty.MP_BASE_COLOR)
    mel.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    mel.recompile_material(mat)
    eal.save_loaded_asset(mat)
    log("M_EwigfelsSolid angelegt")


def make_map():
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if eal.does_asset_exist(MAP_PATH):
        les.load_level(MAP_PATH)
        log("Map existiert schon, wird ergänzt")
    elif not les.new_level(MAP_PATH):
        unreal.log_error("[Ewigfels Phase3] Map konnte nicht angelegt werden")
        return False

    arena_class = unreal.load_class(None, "/Script/Ewigfels.EwigfelsTestArena")
    existing = [a for a in eas.get_all_level_actors() if a.get_class() == arena_class]
    if not existing:
        eas.spawn_actor_from_class(arena_class, unreal.Vector(0, 0, 0))
    if not [a for a in eas.get_all_level_actors() if isinstance(a, unreal.PlayerStart)]:
        eas.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(-1400, 0, 100))
    les.save_current_level()
    log("Map gespeichert: " + MAP_PATH)
    return True


def set_startup_map():
    ini = os.path.join(unreal.Paths.project_config_dir(), "DefaultEngine.ini")
    with open(ini, "r", encoding="utf-8") as f:
        text = f.read()
    entry = MAP_PATH + "." + MAP_PATH.split("/")[-1]
    section = "[/Script/EngineSettings.GameMapsSettings]"
    for key in ("EditorStartupMap", "GameDefaultMap"):
        if re.search(r"^%s=" % key, text, re.M):
            text = re.sub(r"^%s=.*$" % key, "%s=%s" % (key, entry), text, flags=re.M)
        else:
            text = text.replace(section, section + "\n%s=%s" % (key, entry), 1)
    with open(ini, "w", encoding="utf-8") as f:
        f.write(text)
    log("Start-Map in DefaultEngine.ini eingetragen")


make_glow()
make_beam()
make_solid()
if make_map():
    set_startup_map()
log("Fertig. Jetzt oben auf Play (Alt+P) drücken.")

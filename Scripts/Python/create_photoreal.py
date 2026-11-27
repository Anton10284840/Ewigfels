"""
Fotorealistischer Look (einmalig, kann gefahrlos wiederholt werden). Legt im Editor an:
  - /Game/Environment/Materials/M_EwigfelsTriplanar   PBR-Master: Farbe, Normal, Rauheit, AO, weltbezogen (keine Kachel-Nähte,
                                                       egal wie groß Boden/Wand skaliert sind)
  - /Game/Environment/Materials/MI_Ewigfels_<Fläche>   Ground, Wall, Pillar, Rock, Paving – lädt die Arena automatisch
  - /Game/VFX/Materials/M_EwigfelsIce                 durchsichtiges Eis mit Lichtbrechung (Status "Gefroren")
  - /Game/VFX/Materials/M_EwigfelsHeat                Hitzeflimmern (Glutweise, Brennen)
Texturen: ambientCG, CC0 (CREDITS.md). Werden in Content/Environment/Textures/ambientCG/ geladen (nicht im Repo).

Ausführen (Editor offen): Tools > Execute Python Script… > diese Datei
"""
import io
import json
import os
import tempfile
import urllib.parse
import urllib.request
import zipfile

import unreal

TEX_DIR = "/Game/Environment/Textures/ambientCG"
MAT_DIR = "/Game/Environment/Materials"
VFX_DIR = "/Game/VFX/Materials"
API = "https://ambientcg.com/api/v2/full_json?type=Material&q={q}&sort=Popular&limit=8&include=downloadData"

# Fläche -> (Suchbegriffe der Reihe nach, Kachelgröße in cm). Erstes Ergebnis mit 2K-JPG gewinnt.
SURFACES = {
    "Ground": (["paving stones", "cobblestone", "ground"], 300.0),
    "Wall":   (["castle wall", "bricks stone", "rock"], 260.0),
    "Pillar": (["marble", "concrete", "plaster"], 180.0),
    "Rock":   (["rock", "cliff"], 220.0),
    "Paving": (["tiles stone", "paving stones"], 160.0),
}
MAPS = {"Color": "BaseColor", "NormalDX": "Normal", "Roughness": "Roughness", "AmbientOcclusion": "AO"}

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eal = unreal.EditorAssetLibrary


def log(msg):
    unreal.log("[Ewigfels Foto] " + msg)


def http_json(url):
    req = urllib.request.Request(url, headers={"User-Agent": "EwigfelsEditorScript/1.0"})
    with urllib.request.urlopen(req, timeout=60) as r:
        return json.loads(r.read().decode("utf-8"))


def http_bytes(url):
    req = urllib.request.Request(url, headers={"User-Agent": "EwigfelsEditorScript/1.0"})
    with urllib.request.urlopen(req, timeout=300) as r:
        return r.read()


def find_download(queries):
    for q in queries:
        data = http_json(API.format(q=urllib.parse.quote(q)))
        for asset in data.get("foundAssets", []):
            cats = (asset.get("downloadFolders", {}).get("default", {}) or {}).get("downloadFiletypeCategories", {})
            for d in (cats.get("zip", {}) or {}).get("downloads", []):
                if d.get("attribute") == "2K-JPG":
                    return asset["assetId"], d["downloadLink"]
    return None, None


def import_texture(path, dest, name, kind):
    task = unreal.AssetImportTask()
    task.filename = path
    task.destination_path = dest
    task.destination_name = name
    task.automated = True
    task.replace_existing = True
    task.save = False
    asset_tools.import_asset_tasks([task])
    tex = eal.load_asset(dest + "/" + name)
    if not tex:
        return None
    if kind == "Normal":
        tex.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
        tex.set_editor_property("srgb", False)
    elif kind in ("Roughness", "AO"):
        tex.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_MASKS)
        tex.set_editor_property("srgb", False)
    eal.save_loaded_asset(tex)
    return tex


def fetch_surface(surface, queries):
    asset_id, link = find_download(queries)
    if not link:
        unreal.log_warning("[Ewigfels Foto] keine Textur für %s gefunden" % surface)
        return asset_id, {}
    log("%s: lade %s" % (surface, asset_id))
    blob = http_bytes(link)
    out = {}
    tmp = tempfile.mkdtemp(prefix="ewigfels_")
    with zipfile.ZipFile(io.BytesIO(blob)) as z:
        for member in z.namelist():
            for suffix, kind in MAPS.items():
                if member.lower().endswith("_%s.jpg" % suffix.lower()):
                    target = os.path.join(tmp, os.path.basename(member))
                    with open(target, "wb") as f:
                        f.write(z.read(member))
                    tex = import_texture(target, TEX_DIR + "/" + asset_id, "T_%s_%s" % (asset_id, kind), kind)
                    if tex:
                        out[kind] = tex
    return asset_id, out


# ---------------------------------------------------------------------------
# Material-Bausteine

def node(mat, cls, x, y):
    return mel.create_material_expression(mat, cls, x, y)


def connect(a, a_out, b, b_in):
    mel.connect_material_expressions(a, a_out, b, b_in)


def fresh(path_dir, name, cls=unreal.Material, factory=None):
    path = path_dir + "/" + name
    if eal.does_asset_exist(path):
        eal.delete_asset(path)
    return asset_tools.create_asset(name, path_dir, cls, factory or unreal.MaterialFactoryNew())


def make_triplanar():
    """Weltbezogenes PBR: drei Projektionen (oben, vorne, seitlich), nach Flächennormale überblendet."""
    mat = fresh(MAT_DIR, "M_EwigfelsTriplanar")
    wp = node(mat, unreal.MaterialExpressionWorldPosition, -2000, 0)
    tiling = node(mat, unreal.MaterialExpressionScalarParameter, -2000, 150)
    tiling.set_editor_property("parameter_name", "TileSize")
    tiling.set_editor_property("default_value", 250.0)
    scaled = node(mat, unreal.MaterialExpressionDivide, -1800, 50)
    connect(wp, "", scaled, "A")
    connect(tiling, "", scaled, "B")
    uv = {}
    for i, (key, r, g) in enumerate((("xy", True, True), ("xz", True, False), ("yz", False, True))):
        m = node(mat, unreal.MaterialExpressionComponentMask, -1600, i * 120)
        m.set_editor_property("r", key in ("xy", "xz"))
        m.set_editor_property("g", key in ("xy", "yz"))
        m.set_editor_property("b", key in ("xz", "yz"))
        connect(scaled, "", m, "")
        uv[key] = m
    # Gewichte = |Normale|^4, normiert
    nrm = node(mat, unreal.MaterialExpressionVertexNormalWS, -1600, 500)
    absn = node(mat, unreal.MaterialExpressionAbs, -1450, 500)
    connect(nrm, "", absn, "")
    pw = node(mat, unreal.MaterialExpressionPower, -1300, 500)
    pw.set_editor_property("const_exponent", 4.0)
    connect(absn, "", pw, "Base")
    weights = {}
    for i, ch in enumerate(("b", "g", "r")):  # xy-Projektion zählt mit Z, xz mit Y, yz mit X
        m = node(mat, unreal.MaterialExpressionComponentMask, -1150, 450 + i * 80)
        m.set_editor_property("r", ch == "r")
        m.set_editor_property("g", ch == "g")
        m.set_editor_property("b", ch == "b")
        connect(pw, "", m, "")
        weights[("xy", "xz", "yz")[i]] = m
    s1 = node(mat, unreal.MaterialExpressionAdd, -1000, 450)
    connect(weights["xy"], "", s1, "A")
    connect(weights["xz"], "", s1, "B")
    s2 = node(mat, unreal.MaterialExpressionAdd, -900, 450)
    connect(s1, "", s2, "A")
    connect(weights["yz"], "", s2, "B")
    for k in list(weights):
        d = node(mat, unreal.MaterialExpressionDivide, -800, 450)
        connect(weights[k], "", d, "A")
        connect(s2, "", d, "B")
        weights[k] = d

    def triplanar(param, x, y, sampler):
        acc = None
        for i, k in enumerate(("xy", "xz", "yz")):
            t = node(mat, unreal.MaterialExpressionTextureSampleParameter2D, x, y + i * 180)
            t.set_editor_property("parameter_name", param)
            t.set_editor_property("sampler_type", sampler)
            connect(uv[k], "", t, "UVs")
            mul = node(mat, unreal.MaterialExpressionMultiply, x + 250, y + i * 180)
            connect(t, "RGB", mul, "A")
            connect(weights[k], "", mul, "B")
            if acc is None:
                acc = mul
            else:
                add = node(mat, unreal.MaterialExpressionAdd, x + 400, y + i * 180)
                connect(acc, "", add, "A")
                connect(mul, "", add, "B")
                acc = add
        return acc

    color = triplanar("BaseColor", -600, -900, unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
    tint = node(mat, unreal.MaterialExpressionVectorParameter, -150, -1000)
    tint.set_editor_property("parameter_name", "Tint")
    tint.set_editor_property("default_value", unreal.LinearColor(1, 1, 1, 1))
    tinted = node(mat, unreal.MaterialExpressionMultiply, 50, -900)
    connect(color, "", tinted, "A")
    connect(tint, "", tinted, "B")
    normal = triplanar("Normal", -600, -300, unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
    rough = triplanar("Roughness", -600, 300, unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
    rmask = node(mat, unreal.MaterialExpressionComponentMask, 50, 300)
    rmask.set_editor_property("r", True)
    connect(rough, "", rmask, "")
    ao = triplanar("AO", -600, 900, unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
    amask = node(mat, unreal.MaterialExpressionComponentMask, 50, 900)
    amask.set_editor_property("r", True)
    connect(ao, "", amask, "")
    mel.connect_material_property(tinted, "", unreal.MaterialProperty.MP_BASE_COLOR)
    mel.connect_material_property(normal, "", unreal.MaterialProperty.MP_NORMAL)
    mel.connect_material_property(rmask, "", unreal.MaterialProperty.MP_ROUGHNESS)
    mel.connect_material_property(amask, "", unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)
    mel.recompile_material(mat)
    eal.save_loaded_asset(mat)
    log("M_EwigfelsTriplanar angelegt")
    return mat


def make_instance(parent, surface, textures, tile):
    name = "MI_Ewigfels_" + surface
    path = MAT_DIR + "/" + name
    if eal.does_asset_exist(path):
        eal.delete_asset(path)
    mi = asset_tools.create_asset(name, MAT_DIR, unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
    mel.set_material_instance_parent(mi, parent)
    for kind, tex in textures.items():
        mel.set_material_instance_texture_parameter_value(mi, kind, tex)
    mel.set_material_instance_scalar_parameter_value(mi, "TileSize", tile)
    mel.update_material_instance(mi)
    eal.save_loaded_asset(mi)
    log(name + " angelegt")


def make_ice():
    """Klares Eis: durchscheinend, bricht das Licht (Refraktion), helle Kanten, leicht bläulich."""
    mat = fresh(VFX_DIR, "M_EwigfelsIce")
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    mat.set_editor_property("two_sided", False)
    mat.set_editor_property("translucency_lighting_mode", unreal.TranslucencyLightingMode.TLM_SURFACE_PER_PIXEL_LIGHTING)
    try:
        mat.set_editor_property("refraction_method", unreal.RefractionMode.RM_INDEX_OF_REFRACTION)
    except Exception:
        pass
    color = node(mat, unreal.MaterialExpressionVectorParameter, -900, 0)
    color.set_editor_property("parameter_name", "Color")
    color.set_editor_property("default_value", unreal.LinearColor(0.75, 0.9, 1.0, 1.0))
    fres = node(mat, unreal.MaterialExpressionFresnel, -900, 250)
    fres.set_editor_property("exponent", 3.0)
    opac = node(mat, unreal.MaterialExpressionLinearInterpolate, -600, 250)
    opac.set_editor_property("const_a", 0.25)
    opac.set_editor_property("const_b", 0.85)
    connect(fres, "", opac, "Alpha")
    glow = node(mat, unreal.MaterialExpressionMultiply, -600, 100)
    connect(color, "", glow, "A")
    connect(fres, "", glow, "B")
    glow_amt = node(mat, unreal.MaterialExpressionScalarParameter, -900, 400)
    glow_amt.set_editor_property("parameter_name", "Glow")
    glow_amt.set_editor_property("default_value", 1.6)
    emis = node(mat, unreal.MaterialExpressionMultiply, -400, 100)
    connect(glow, "", emis, "A")
    connect(glow_amt, "", emis, "B")
    ior = node(mat, unreal.MaterialExpressionConstant, -600, 500)
    ior.set_editor_property("r", 1.31)
    mel.connect_material_property(color, "", unreal.MaterialProperty.MP_BASE_COLOR)
    mel.connect_material_property(emis, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mel.connect_material_property(opac, "", unreal.MaterialProperty.MP_OPACITY)
    mel.connect_material_property(ior, "", unreal.MaterialProperty.MP_REFRACTION)
    rough = node(mat, unreal.MaterialExpressionConstant, -600, 650)
    rough.set_editor_property("r", 0.05)
    mel.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    spec = node(mat, unreal.MaterialExpressionConstant, -600, 750)
    spec.set_editor_property("r", 1.0)
    mel.connect_material_property(spec, "", unreal.MaterialProperty.MP_SPECULAR)
    mel.recompile_material(mat)
    eal.save_loaded_asset(mat)
    log("M_EwigfelsIce angelegt")


def make_heat():
    """Hitzeflimmern: unsichtbar, verzerrt nur das Bild dahinter (wandernde Störung, am Rand weich)."""
    mat = fresh(VFX_DIR, "M_EwigfelsHeat")
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mat.set_editor_property("two_sided", True)
    try:
        mat.set_editor_property("refraction_method", unreal.RefractionMode.RM_PIXEL_NORMAL_OFFSET)
    except Exception:
        pass
    wp = node(mat, unreal.MaterialExpressionWorldPosition, -1200, 0)
    t = node(mat, unreal.MaterialExpressionTime, -1200, 200)
    up = node(mat, unreal.MaterialExpressionMultiply, -1000, 200)
    up.set_editor_property("const_b", 60.0)
    connect(t, "", up, "A")
    off = node(mat, unreal.MaterialExpressionAppendVector, -850, 200)
    zero = node(mat, unreal.MaterialExpressionConstant2Vector, -1000, 320)
    connect(zero, "", off, "A")
    connect(up, "", off, "B")
    moved = node(mat, unreal.MaterialExpressionSubtract, -700, 100)
    connect(wp, "", moved, "A")
    connect(off, "", moved, "B")
    noise = node(mat, unreal.MaterialExpressionNoise, -500, 100)
    noise.set_editor_property("scale", 0.02)
    noise.set_editor_property("levels", 3)
    connect(moved, "", noise, "Position")
    fres = node(mat, unreal.MaterialExpressionFresnel, -500, 300)
    fres.set_editor_property("exponent", 1.5)
    inv = node(mat, unreal.MaterialExpressionOneMinus, -350, 300)
    connect(fres, "", inv, "")
    strength = node(mat, unreal.MaterialExpressionScalarParameter, -500, 450)
    strength.set_editor_property("parameter_name", "Strength")
    strength.set_editor_property("default_value", 0.06)
    a = node(mat, unreal.MaterialExpressionMultiply, -200, 200)
    connect(noise, "", a, "A")
    connect(inv, "", a, "B")
    b = node(mat, unreal.MaterialExpressionMultiply, -50, 250)
    connect(a, "", b, "A")
    connect(strength, "", b, "B")
    nrm = node(mat, unreal.MaterialExpressionAppendVector, 100, 250)
    connect(b, "", nrm, "A")
    connect(b, "", nrm, "B")
    one = node(mat, unreal.MaterialExpressionConstant, -50, 400)
    one.set_editor_property("r", 1.0)
    nrm3 = node(mat, unreal.MaterialExpressionAppendVector, 250, 300)
    connect(nrm, "", nrm3, "A")
    connect(one, "", nrm3, "B")
    zero_op = node(mat, unreal.MaterialExpressionConstant, 100, 500)
    zero_op.set_editor_property("r", 0.0)
    ior = node(mat, unreal.MaterialExpressionConstant, 100, 600)
    ior.set_editor_property("r", 1.0)
    mel.connect_material_property(nrm3, "", unreal.MaterialProperty.MP_NORMAL)
    mel.connect_material_property(zero_op, "", unreal.MaterialProperty.MP_OPACITY)
    mel.connect_material_property(ior, "", unreal.MaterialProperty.MP_REFRACTION)
    mel.recompile_material(mat)
    eal.save_loaded_asset(mat)
    log("M_EwigfelsHeat angelegt")


def main():
    parent = make_triplanar()
    for surface, (queries, tile) in SURFACES.items():
        try:
            asset_id, textures = fetch_surface(surface, queries)
        except Exception as e:  # Netz weg o. ä.: Fläche bleibt beim Platzhalter
            unreal.log_warning("[Ewigfels Foto] %s: Download fehlgeschlagen: %s" % (surface, e))
            continue
        if textures:
            make_instance(parent, surface, textures, tile)
            log("%s = ambientCG %s (CC0)" % (surface, asset_id))
    make_ice()
    make_heat()
    log("Fertig. Testarena neu laden (oder Play drücken), dann sind die echten Oberflächen da.")


main()

"""
Fingergesten aus "Paragon: Gideon" (Fab, gratis, siehe CREDITS.md) für unsere Spielfigur (GASP SandboxCharacter, UEFN-Skelett).
Einmalig im Editor ausführen (kann wiederholt werden):
  UnrealEditor.exe Ewigfels.uproject -ExecutePythonScript="<Pfad>/create_gesture_montages.py"

Macht:
  1. IK Rig für Gideon + Retargeter Gideon -> UEFN (/Game/Animations/Magic/Gesten/Retarget)
  2. Gideons Zauber-Animationen umzielen (nur die benötigten)
  3. 9 kurze Montagen AM_Geste_<geste> im Slot "UpperBody" (/Game/Animations/Magic/Gesten)
  4. SandboxCharacter_CMC_ABP: Oberkörper-Slot "UpperBody" ab spine_02 (Layered Blend per Bone), damit die Beine
     beim Zaubern weiterlaufen. DefaultSlot (Klettern/Traversal des Samples) bleibt unverändert.
Gideons Figur selbst wird im Spiel nicht benutzt.
"""
import traceback
import unreal

GIDEON_MESH = "/Game/ParagonGideon/Characters/Heroes/Gideon/Meshes/Gideon"
GIDEON_ANIMS = "/Game/ParagonGideon/Characters/Heroes/Gideon/Animations/"
TARGET_MESH = "/Game/Characters/UEFN_Mannequin/Meshes/SKM_UEFN_Mannequin"
TARGET_RIG = "/Game/Characters/UEFN_Mannequin/Rigs/IK_UEFN_Mannequin"
ABP = "/Game/Blueprints/SandboxCharacter_CMC_ABP"
OUT_DIR = "/Game/Animations/Magic/Gesten"
RTG_DIR = OUT_DIR + "/Retarget"
SLOT = "UpperBody"
BLEND_BONE = "spine_02"

# Geste -> (Gideon-Animation, Start s, Ende s). Ausgewählt nach Handbahnen (Skript-Auswertung, +Y = vorn).
GESTURES = {
    "flick":  ("Primary_Attack_A_Medium", 0.05, 0.55),  # rechte Hand stößt schnell nach vorn
    "punch":  ("Blackhole_Start", 3.20, 3.80),          # beide Hände stoßen nach vorn
    "stiff":  ("RMB_Cast", 0.00, 0.50),                 # Hand vorn gehalten, Finger gespreizt
    "clap":   ("Blackhole_Start", 0.00, 0.40),          # Arme auf, Hände treffen sich vor der Brust
    # rechte Hand steigt bis über den Kopf und bleibt oben (ohne den Wurf am Ende von "rub"), linke bleibt unten. (Burden_Start war falsch: dort gehen
    # beide Arme waagrecht zur Seite, Hände nur auf Kopfhöhe – gemessen mit AnimPose, Hand minus Kopf.)
    "lift":   ("Cast", 0.04, 0.62),
    "slam":   ("Blackhole_Start", 0.25, 0.75),          # beide Hände schmettern nach unten
    "grab":   ("Torn_Space_Outro", 0.00, 0.70),         # vorgreifen und zurückreißen
    "rub":    ("Cast", 0.10, 0.85),                     # über Kopf holen, dann werfen
    "twitch": ("PortalBlast_Start", 0.00, 0.43),        # zucken, dann zeigen
}

eal = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
SRC = unreal.RetargetSourceOrTarget.SOURCE
TGT = unreal.RetargetSourceOrTarget.TARGET


def log(msg):
    unreal.log("[Ewigfels Gesten] " + msg)


def make_or_load(path, cls, factory):
    if eal.does_asset_exist(path):
        return unreal.load_asset(path)
    folder, name = path.rsplit("/", 1)
    return tools.create_asset(name, folder, cls, factory)


def build_retargeter():
    gideon = unreal.load_asset(GIDEON_MESH)
    target_mesh = unreal.load_asset(TARGET_MESH)
    rig = make_or_load(RTG_DIR + "/IK_Gideon", unreal.IKRigDefinition, unreal.IKRigDefinitionFactory())
    rc = unreal.IKRigController.get_controller(rig)
    rc.set_skeletal_mesh(gideon)
    if not rc.get_retarget_chains():
        ok = rc.apply_auto_generated_retarget_definition()
        log("IK_Gideon Ketten automatisch: %s, %d Ketten, Wurzel %s" % (ok, len(rc.get_retarget_chains()), rc.get_retarget_root()))
    eal.save_loaded_asset(rig)

    rtg = make_or_load(RTG_DIR + "/RTG_Gideon_to_UEFN", unreal.IKRetargeter, unreal.IKRetargetFactory())
    c = unreal.IKRetargeterController.get_controller(rtg)
    c.set_ik_rig(SRC, rig)
    c.set_ik_rig(TGT, unreal.load_asset(TARGET_RIG))
    c.set_preview_mesh(SRC, gideon)
    c.set_preview_mesh(TGT, target_mesh)
    c.add_default_ops()
    c.assign_ik_rig_to_all_ops(SRC, rig)
    c.assign_ik_rig_to_all_ops(TGT, unreal.load_asset(TARGET_RIG))
    c.auto_map_chains(unreal.AutoMapChainType.FUZZY, True)
    c.auto_align_all_bones(TGT)
    eal.save_loaded_asset(rtg)
    log("RTG_Gideon_to_UEFN bereit")
    return rtg, gideon, target_mesh


def retarget(rtg, gideon, target_mesh):
    names = sorted({g[0] for g in GESTURES.values()})
    out = {}
    todo = []
    for n in names:
        dst = "%s/%s_UEFN" % (RTG_DIR, n)
        if eal.does_asset_exist(dst):
            out[n] = unreal.load_asset(dst)
        else:
            todo.append(unreal.EditorAssetLibrary.find_asset_data(GIDEON_ANIMS + n))
    if todo:
        res = unreal.IKRetargetBatchOperation.duplicate_and_retarget(
            todo, gideon, target_mesh, rtg, "", "", "", "_UEFN", RTG_DIR, False, False, True)
        log("umgezielt: %s" % [str(a.asset_name) for a in res])
    for n in names:
        dst = "%s/%s_UEFN" % (RTG_DIR, n)
        out[n] = unreal.load_asset(dst)
        if not out[n]:
            raise RuntimeError("Umzielen fehlgeschlagen: " + dst)
        eal.save_loaded_asset(out[n])
    return out


def trimmed_copy(seq, gesture, t0, t1):
    """Kopie der umgezielten Animation, auf [t0, t1] zugeschnitten (Montagen übernehmen die Länge der Quelle)."""
    path = "%s/A_Geste_%s" % (RTG_DIR, gesture)
    if eal.does_asset_exist(path):
        eal.delete_asset(path)
    copy = eal.duplicate_asset(seq.get_path_name().split(".")[0], path)
    ctrl = copy.get_editor_property("controller")
    total = unreal.AnimationLibrary.get_num_frames(copy)
    f0 = unreal.AnimationLibrary.get_frame_at_time(copy, t0)
    f1 = min(unreal.AnimationLibrary.get_frame_at_time(copy, t1), total)
    # resize_in_frames(neue Länge, T0, T1): fügt im Bereich T0–T1 Frames ein bzw. entfernt sie
    if f1 < total:
        ctrl.resize_in_frames(unreal.FrameNumber(f1), unreal.FrameNumber(f1), unreal.FrameNumber(total), True)
    if f0 > 0:
        ctrl.resize_in_frames(unreal.FrameNumber(f1 - f0), unreal.FrameNumber(0), unreal.FrameNumber(f0), True)
    log("  %s: Frames %d-%d von %d -> %.2f s" % (gesture, f0, f1, total, copy.get_play_length()))
    eal.save_loaded_asset(copy)
    return copy


def make_montages(seqs, target_mesh):
    skel = target_mesh.get_editor_property("skeleton")
    for gesture, (anim, t0, t1) in GESTURES.items():
        t1 = min(t1, seqs[anim].get_play_length())
        clip = trimmed_copy(seqs[anim], gesture, t0, t1)
        path = "%s/AM_Geste_%s" % (OUT_DIR, gesture)
        if eal.does_asset_exist(path):
            # Vorhandene Montage weiterverwenden: Löschen + Neuanlegen scheitert, solange das Paket geladen ist
            m = unreal.load_asset(path)
        else:
            f = unreal.AnimMontageFactory()
            f.set_editor_property("source_animation", clip)
            f.set_editor_property("target_skeleton", skel)
            m = tools.create_asset("AM_Geste_" + gesture, OUT_DIR, unreal.AnimMontage, f)
        length = clip.get_play_length()
        tracks = m.get_editor_property("slot_anim_tracks")
        tr = tracks[0]
        tr.set_editor_property("slot_name", SLOT)
        track = tr.get_editor_property("anim_track")
        seg = unreal.AnimSegment()
        seg.import_text('(AnimReference="%s",StartPos=0.0,AnimStartTime=0.0,AnimEndTime=%f,AnimPlayRate=1.0,LoopingCount=1)'
                        % (clip.get_path_name(), length))
        track.set_editor_property("anim_segments", [seg])
        tr.set_editor_property("anim_track", track)
        tracks[0] = tr
        m.set_editor_property("slot_anim_tracks", tracks)
        if abs(m.get_play_length() - length) > 0.02:
            # Die Länge einer vorhandenen Montage lässt sich per Python nicht neu berechnen (SequenceLength ist read-only).
            # Abhilfe: bei geschlossenem Editor Content/Animations/Magic/Gesten/AM_Geste_<geste>.uasset löschen, Skript neu starten.
            unreal.log_warning("[Ewigfels Gesten] %s: Montage-Länge %.2f statt %.2f – AM_Geste_%s.uasset löschen und Skript neu starten"
                               % (gesture, m.get_play_length(), length, gesture))
        for prop, val in (("blend_in", 0.12), ("blend_out", 0.18)):
            try:
                b = m.get_editor_property(prop)
                b.set_editor_property("blend_time", val)
                m.set_editor_property(prop, b)
            except Exception as e:
                unreal.log_warning("[Ewigfels Gesten] %s: %s" % (prop, e))
        m.set_preview_skeletal_mesh(target_mesh)
        eal.save_loaded_asset(m)
        log("AM_Geste_%s = %s %.2f-%.2f s (Länge %.2f)" % (gesture, anim, t0, t1, m.get_play_length()))


def pin(node, name, direction):
    for p in node.list_all_pins():
        if p.get_pin_direction() == direction and str(p.get_pin_name()) == name:
            return p
    raise RuntimeError("Pin %s fehlt an %s" % (name, node.get_node_title()))


def out_pose(node):
    return [p for p in node.list_all_pins() if p.get_pin_direction() == unreal.EdGraphPinDirection.EGPD_OUTPUT][0]


def setup_upper_body_slot():
    abp = unreal.load_asset(ABP)
    graph = unreal.BlueprintEditorLibrary.find_graph(abp, "AnimGraph")
    ed = unreal.BlueprintGraphEditor.get_graph_editor(graph)
    slots = [n for n in ed.list_all_nodes() if isinstance(n, unreal.AnimGraphNode_Slot)]
    for s in slots:
        if str(s.get_editor_property("node").get_editor_property("slot_name")) == SLOT:
            log("Oberkörper-Slot ist schon da")
            return
    default_slot = slots[0]
    IN, OUT = unreal.EdGraphPinDirection.EGPD_INPUT, unreal.EdGraphPinDirection.EGPD_OUTPUT
    src_in = pin(default_slot, "Source", IN)
    upstream = src_in.list_connected_pins()[0]           # "Meshraum-Additiv anwenden".Pose
    slot_out = out_pose(default_slot)
    downstream = slot_out.list_connected_pins()[0]       # "Versatz Stamm Bone".Source
    pos = default_slot.get_node_pos()

    # Pose vor dem Slot zwischenspeichern, damit beide Slots sie bekommen
    upstream.break_single_pin_link(src_in)
    save = ed.create_node_from_name("Animation|CachedPoses|NeuezwischengespeichertePosespeichern ...",
                                    unreal.Vector2D(pos.x - 300, pos.y - 250), [], None)
    save.set_editor_property("cache_name", "EwigfelsVorSlot")
    save_in = [p for p in save.list_all_pins() if p.get_pin_direction() == IN][0]
    if not upstream.try_create_connection(save_in):
        raise RuntimeError("Cache-Eingang nicht verbunden")
    use_name = [x for x in ed.list_available_nodes([]) if "CachedPoses" in x and "EwigfelsVorSlot" in x][0]
    use1 = ed.create_node_from_name(use_name, unreal.Vector2D(pos.x - 250, pos.y), [], None)
    use2 = ed.create_node_from_name(use_name, unreal.Vector2D(pos.x - 250, pos.y + 250), [], None)
    if not out_pose(use1).try_create_connection(src_in):
        raise RuntimeError("DefaultSlot-Eingang nicht verbunden")

    upper = ed.create_node_from_name("Animation|Montage|Slot'DefaultSlot'", unreal.Vector2D(pos.x, pos.y + 250), [], None)
    st = upper.get_editor_property("node")
    st.set_editor_property("slot_name", SLOT)
    upper.set_editor_property("node", st)
    if not out_pose(use2).try_create_connection(pin(upper, "Source", IN)):
        raise RuntimeError("UpperBody-Eingang nicht verbunden")

    lb = ed.create_node_from_name("Animation|Blends|GeschichteteÜberblendungproBone", unreal.Vector2D(pos.x + 250, pos.y + 100), [], None)
    lst = lb.get_editor_property("node")
    lst.import_text('(LayerSetup=((BranchFilters=((BoneName="%s",BlendDepth=0)))),BlendWeights=(1.000000),bMeshSpaceRotationBlend=True,CurveBlendOption=Override)' % BLEND_BONE)
    lb.set_editor_property("node", lst)
    slot_out.break_single_pin_link(downstream)
    if not slot_out.try_create_connection(pin(lb, "BasePose", IN)):
        raise RuntimeError("BasePose nicht verbunden")
    if not out_pose(upper).try_create_connection(pin(lb, "BlendPoses_0", IN)):
        raise RuntimeError("BlendPoses_0 nicht verbunden")
    if not out_pose(lb).try_create_connection(downstream):
        raise RuntimeError("Ausgang nicht verbunden")
    log("Layered Blend: %s" % lb.get_editor_property("node").export_text()[:200])
    unreal.BlueprintEditorLibrary.compile_blueprint(abp)
    eal.save_loaded_asset(abp)
    skel = abp.get_editor_property("target_skeleton")
    eal.save_loaded_asset(skel, only_if_is_dirty=False)
    log("AnimBP: Oberkörper-Slot '%s' ab %s eingebaut und kompiliert" % (SLOT, BLEND_BONE))


def main():
    try:
        rtg, gideon, target_mesh = build_retargeter()
        seqs = retarget(rtg, gideon, target_mesh)
        make_montages(seqs, target_mesh)
        setup_upper_body_slot()
        log("Fertig.")
    except Exception:
        unreal.log_error("[Ewigfels Gesten] FEHLER\n" + traceback.format_exc())


main()

import bpy
from bpy import data as D
from bpy import context as C
from mathutils import *
from math import *
import xml.etree.ElementTree as ET


def main(path):
    import_and_tweak_collada(path + ".dae")
    anixmlPath = path + ".anixml"
    tree = ET.parse(anixmlPath)
    rt = tree.getroot()
    read_data(rt)
    read_metadata(rt)


def import_and_tweak_collada(daePath):
    bpy.ops.wm.collada_import(filepath=daePath)
    delete_lods = False
    if (delete_lods):
        bpy.ops.object.select_pattern(pattern="*lod[123456789]*", case_sensitive=True, extend=False)
        bpy.ops.object.delete(use_global=True)
    else:
        for obj in C.scene.objects:
            if 'lod0' not in obj.name and 'lod' in obj.name:
                obj.hide_select = True
                obj.hide_viewport = True
                obj.hide_render = True

    hide_collision = True
    if (hide_collision):
        for obj in C.scene.objects:
            if 'collision' in obj.name:
                obj.hide_select = True
                obj.hide_viewport = True
                obj.hide_render = True


def read_data(root):
    print("Starting animations")
    for part in root[0]:  # we wrote data first
        part_name = part.attrib['name']
        for anim in part:
            anim_name = anim.attrib['subname']
            action_name = part_name + anim_name.split("_", 1)[0]
            scene = D.scenes['Scene']
            obj = D.objects[part_name]
            if  obj.animation_data is None:
                obj.animation_data_create()
            for path in anim:
                path_name = path.tag
                for axis in path:
                    read_frame(axis, obj, path_name)


def read_frame(axis_data, obj, path_name):
    groupnames = {"location": "Location", "rotation_euler": "Rotation", "scale": "Scaling"}
    axis_name = axis_data.tag
    axes = ["X", "Y", "Z"]
    axis_idx = axes.index(axis_name)
    # TODO offset instead?

    starting_data={"location": obj.location, "rotation_euler": obj.rotation_euler, "scale": obj.scale}

    for f in axis_data:
        frame = int(f.attrib["id"])
        obj.keyframe_insert(data_path=path_name,
                            index=axis_idx,
                            frame=float(frame),
                            group=groupnames[path_name])
        fc = get_fcurve(obj,path_name,axis_idx)
        kf = get_keyframe(fc,frame)

        if (path_name == "rotation_euler"):
            kf.co[1] = starting_data[path_name][axis_idx]-float(f.attrib["value"])
        else:
            kf.co[1]=starting_data[path_name][axis_idx]+float(f.attrib["value"])
        interp = f.attrib["interpolation"]
        if (interp == "STEP"):
            kf.interpolation="CONSTANT"
        else:
            kf.interpolation=interp

        handle_l = f.find("handle_left")
        handle_r = f.find("handle_right")
        kf.handle_left=(float(handle_l.attrib["X"]),float(handle_l.attrib["Y"]))
        kf.handle_right=(float(handle_r.attrib["X"]),float(handle_r.attrib["Y"]))

def get_fcurve(obj,path,idx):
    for fc in obj.animation_data.action.fcurves:
        if fc.data_path==path and fc.array_index == idx:
            return fc

    return None
def get_keyframe(fc,frame):
    for kf in fc.keyframe_points:
        if (abs(frame-kf.co[0])<0.1):
            return kf
    return None

def read_metadata(root):
    print("Starting metadata")
    for conn in root[1]:  # then we wrote metadta
        tgt_name = conn.attrib["name"]
        for anim in conn:
            anim_name = anim.attrib["subname"]
            scene = D.scenes['Scene']
            timeline_markers = scene.timeline_markers

            start = int(anim.attrib["start"])
            end = int(anim.attrib["end"])
            state_name = anim_name.split("_", 1)[1]
            # TODO reverse lookup by time and concatenate?
            if timeline_markers.get(state_name):
                continue
            elif start == end:
                timeline_markers.new(state_name, frame=start)
                continue
            if not timeline_markers.get(state_name + "Start"):
                # TODO if is there check if same/warn
                timeline_markers.new(state_name + "Start", frame=start)
            if not timeline_markers.get(state_name + "End"):
                # TODO if is there check if same/warn
                timeline_markers.new(state_name + "End", frame=end)

    print("Done with metadata")


if True:
    for obj in D.objects:
        obj.hide_select = False
        obj.hide_viewport = False
        obj.hide_render = False

    for scene in D.scenes:
        bpy.context.window.scene = scene
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete()

bpy.context.window.scene = D.scenes['Scene']
main('/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.out')

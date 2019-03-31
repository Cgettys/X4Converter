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
    delete_lods = True
    if (delete_lods):
        override = C.copy()
        bpy.ops.object.select_pattern(override, pattern="*lod[!0]*", case_sensitive=True, extend=False)
        bpy.ops.object.delete(override)
    hide_collision = True
    if (hide_collision):
        override = C.copy()
        bpy.ops.object.select_pattern(override, pattern="*collision*", case_sensitive=True, extend=False)
        for obj in C.selected_objects(override):
            obj.hide_select = True
            obj.hide_viewport = True
            obj.hide_render = True



def get_scene_by_anim_name(anim_name, create_if_not_exist=False):
    scene_name = anim_name.split("_", 1)[0]
    if scene_name not in D.scenes and create_if_not_exist:
        print("Created Scene")
        scene = D.scenes.new(scene_name)
        scene.use_gravity = False
        return scene
    else:
        return D.scenes[scene_name]


def read_data(root):
    print("Starting animations")
    for part in root[0]:  # we wrote data first
        part_name = part.attrib['name']
        for anim in part:
            anim_name = anim.attrib['subname']
            scene = get_scene_by_anim_name(anim_name, True)
#           scene.
# tgt = anim.find("channel").attrib["target"]
#   tgt_name,tgt_channel = tgt.split("/")
#  channel_group,channel_axis=tgt_channel.split(".")
#    print("Target Name: {} ChannelType: {} Axis: {}".format(tgt_name,channel_group,channel_axis))
#   if tgt_name not in D.actions:
#      print("\tCreated action")
#      D.actions.new(tgt_name)
#  action = D.actions[tgt_name]
# for src in anim.findall("source"):


def read_metadata(root):
    print("Starting metadata")
    for conn in root[1]:  # then we wrote metadta
        tgt_name = conn.attrib["name"]
        for anim in conn:
            anim_name = anim.attrib["subname"]
            scene = get_scene_by_anim_name(anim_name, True)
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
            else:
                print("wut")
    print("Done with metadata")


if True:
    override = C.copy()
    bpy.ops.object.select_all(override)
    bpy.ops.object.delete(override)
main('/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.out')

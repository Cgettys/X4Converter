import re
import os
import bpy
from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator
import subprocess
from mathutils import *
from math import *
import xml.etree.ElementTree as ET
bl_info = {
    "name": "X4 Foundations Addon",
    "category": "Import-Export",
    "blender": (2, 80, 0)}

def call_converter(action, datdir,target):
    # As per the converter's command line args:
    # executable <action> <datdir> <target>
    script_path = os.path.realpath(__file__)
    dir_path = os.path.dirname(script_path)
    converter_path = dir_path

    if (os.path.exists(dir_path + "X4ConverterApp")):
        converter_path = converter_path+"X4ConverterApp"
    elif (os.path.exists(dir_path + "X4ConverterApp.exe")):
        converter_path = converter_path+"X4ConverterApp.exe"
    else:
        error = "Error, could not find converter - are you sure it's in the same directory as the script?"
        print(error)
        raise Exception(error)
    subprocess.check_call([converter_path,action,datdir,target])

class ImportAsset(Operator, ImportHelper):
    bl_idname = "import_scene.x4import"
    bl_label = "X4 Foundations Import"
    bl_options = {'REGISTER' }
    filename_ext = ".xml"

    filter_glob: StringProperty(
        default="*.xml",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )


    unpacked_path: StringProperty(
        name="Unpacked Path",
        description="Path to unpacked catalog files",
        subtype='DIR_PATH'
    )

    run_converter: BoolProperty(
        name="Run Converter",
        description="Run Converter on Target File instead of assuming it's already been done",
        default=True,
    )

    delete_lods: BoolProperty(
        name="Delete Lods",
        description="Delete Lower Lods",
        default=False,
    )

    hide_collision: BoolProperty(
        name="Hide Collision Mesh",
        description="Hide Collision Mesh from scene",
        default=True,
    )
    import_animations: BoolProperty(
        name="Import Animations (Experimental)",
        description="Import Animation Data - wip",
        default=True,
    )
    def execute(self, context):
        if (self.run_converter):
            call_converter("importxmf",self.unpacked_path,self.filepath)
        base_path = re.sub('\.xml$', '', self.filepath)
        self.import_and_tweak_collada(base_path)


        if (self.import_animations):
            self.read_animations(base_path)

        #read_metadata(rt)
        return {'FINISHED'}


    def import_and_tweak_collada(self,base_path):
        dae_path = base_path+ ".dae"
        bpy.ops.wm.collada_import(filepath=dae_path)
        #    for obj in C.scene.objects:
        #        obj.up_axis = 'Y'
        #        obj.track_axis = 'POS_Z'
        if (self.delete_lods):
            bpy.ops.object.select_pattern(pattern="*lod[123456789]*", case_sensitive=True, extend=False)
            bpy.ops.object.delete(use_global=True)
        else:
            for obj in C.scene.objects:
                if 'lod0' not in obj.name and 'lod' in obj.name:
                    obj.hide_select = True
                    obj.hide_viewport = True
                    obj.hide_render = True

        if (self.hide_collision):
            for obj in C.scene.objects:
                if 'collision' in obj.name:
                    obj.hide_select = True
                    obj.hide_viewport = True
                    obj.hide_render = True


    def read_animations(self, base_path):
        anixml_path = base_path + ".anixml"
        tree = ET.parse(anixml_path)
        root = tree.getroot()
        print("Starting animations")
        for part in root[0]:  # we wrote data first
            part_name = part.attrib['name']
            obj = D.objects[part_name]
            for cat in part:
                handle_category(obj,cat,part_name)

        for part in root[1]:
            print(part.attrib['name'])
            for e in part:
                if (e.tag=='pivot_position_offset'):
                    obj = D.objects[part.attrib['name']]
                    for c in obj.children:
                        c.delta_location = (-float(e.attrib['x']),-float(e.attrib['y']),-float(e.attrib['z']))
        ship_macro=path.rsplit('/',1)[1]
        ship_macro=ship_macro.replace(".out","")
        # also the root part
        root_part=D.objects[ship_macro]

    def handle_category(obj,cat,part_name):
        scene = D.scenes['Scene']
        offset_frames = 0
        for anim in cat:
            offset_frames = offset_frames + handle_category_anim(obj,cat,part_name,anim,offset_frames)


    def handle_category_anim(obj,cat,part_name,anim,offset_frames):
        anim_name = anim.attrib['subname']
        action_name = part_name + anim_name
        frame_max = 0
        if  obj.animation_data is None:
            obj.animation_data_create()
        for path in anim:
            path_name = path.tag
            for axis in path:
                possible_max=read_frames(axis, obj, path_name,offset_frames)
                frame_max=max(frame_max,possible_max)
        return frame_max

    def read_frames(axis_data, obj, path_name,offset_frames):
        # TODO test/assert assumption that all anims start at 0, data wise
        groupnames = {"location": "Location", "rotation_euler": "Rotation", "scale": "Scaling"}
        axis_name = axis_data.tag
        axes = ["X", "Y", "Z"]
        axis_idx = axes.index(axis_name)
        # TODO offset instead?

        frame_min = 0
        frame_max = 0
        starting_data={"location": obj.location, "rotation_euler": obj.rotation_euler, "scale": obj.scale}
        for f in axis_data:
            frame = int(f.attrib["id"])
            fake_frame = float(frame+offset_frames)
            obj.keyframe_insert(data_path=path_name,
                                index=axis_idx,
                                frame=fake_frame,
                                group=groupnames[path_name])
            fc = get_fcurve(obj,path_name,axis_idx)
            kf = get_keyframe(fc,fake_frame)

            # Ugh converting world handedness bullshit
            if path_name == "location" and axis_name =="X":
                kf.co[1]=starting_data[path_name][axis_idx]-float(f.attrib["value"])
            elif path_name == "rotation_euler" and axis_name == "X":
                kf.co[1]=starting_data[path_name][axis_idx]-float(f.attrib["value"])
            elif path_name == "scale":
                # scale multiplies... I'm an idiot sometimes
                kf.co[1]=starting_data[path_name][axis_idx]*float(f.attrib["value"])
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
            frame_min = min(frame_min, frame)
            frame_max = max(frame_max,frame)
        if (frame_min <0):
            print("Something has gone horribly wrong, frame_min < 0")
        return frame_max

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


# Only needed if you want to add into a dynamic menu
def menu_func_import(self, context):
    self.layout.operator(ImportAsset.bl_idname, text="X4 Import")





def register():

    print("X4 Foundations Addon registered!")
    bpy.utils.register_class(ImportAsset)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)

def unregister():
    print("X4 Foundations Addon unregistered :(")
    bpy.utils.unregister_class(ImportAsset)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)

if False:
    for obj in D.objects:
        obj.hide_select = False
        obj.hide_viewport = False
        obj.hide_render = False

    for scene in D.scenes:
        bpy.context.window.scene = scene
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete()

#bpy.context.window.scene = D.scenes['Scene']
#base_dir = '/home/cg/Desktop/X4/unpacked/'
#tgt_rel = 'assets/units/size_m/ship_par_m_corvette_01'
#tgt_rel='assets/units/size_s/ship_gen_s_fighter_01'
#tgt_rel='assets/units/size_s/ship_par_s_scout_01'
#tgt_rel='assets/units/size_s/ship_arg_s_fighter_02'
#tgt_rel='assets/units/size_xl/ship_arg_xl_carrier_01'
#tgt_rel='assets/units/size_m/ship_tel_m_frigate_01'
#main(base_dir+tgt_rel+'.out')
#print('done')
if __name__ == '__main__':
    register()
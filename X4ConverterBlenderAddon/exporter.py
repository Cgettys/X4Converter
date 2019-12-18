import re
import bpy
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator
import xml.etree.ElementTree as ET
from X4ConverterBlenderAddon.common import *

class ExportAsset(Operator, ExportHelper):
    bl_idname = "export_scene.x4export"
    bl_label = "X4 Foundations Export"
    bl_options = {'REGISTER' }
    filename_ext = ".xml"

    filter_glob: StringProperty(
        default="*.xml",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )


    run_converter: BoolProperty(
        name="Run Converter",
        description="Run Converter on Target File instead of assuming it's already been done",
        default=True,
    )

    export_animations: BoolProperty(
        name="Export Animations (Experimental)",
        description="Export Animation Data - wip",
        default=False,
    )

    def execute(self, context):
        base_path = re.sub(r'\.xml$', '', self.filepath)
        self.tweak_and_export_collada(context,base_path)


        if (self.export_animations):
            self.write_animations(base_path)

        
        # #read_metadata(rt)

        if (self.run_converter):
            call_converter("exportxmf",base_path+".dae")
        return {'FINISHED'}


    def tweak_and_export_collada(self, ctx, base_path):
        # TODO real path handling. This is a kludge to address issue 2
        if (base_path.find('/') != -1):
            ship_macro = base_path.rsplit('/', 1)[1]
        else:
            ship_macro = base_path.rsplit('\\', 1)[1]
        ship_macro = ship_macro.replace(".out", "")
        # also the root part
        root_part = bpy.data.objects[ship_macro]
        root_part.scale = (1, 1, 1)
        # For export, we need it to be at the game's scale

        dae_path = base_path + ".dae"
        bpy.ops.wm.collada_export(filepath=dae_path)



    def write_animations(self,base_path):
        anixml_path = base_path + ".anixml"
        print("not implemented")
        # tree = ET.parse(anixml_path)
        # root = tree.getroot()
        # print("Starting animations")
        # for part in root[0]:  # we wrote data first
        #     part_name = part.attrib['name']
        #     obj = bpy.data.objects[part_name]
        #     for cat in part:
        #         self.handle_category(obj,cat,part_name)

        # for part in root[1]:
        #     print(part.attrib['name'])


    def handle_category(self,obj,cat,part_name):
        for anim in cat:
            self.handle_category_anim(obj,cat,part_name,anim)


    def handle_category_anim(self,obj,cat,part_name,anim):
        anim_name = anim.attrib['subname']
        action_name = part_name + anim_name
        frame_max = 0
        if  obj.animation_data is None:
            obj.animation_data_create()
        for path in anim:
            path_name = path.tag
            for axis in path:
                self.read_frames(axis, obj, path_name)

    def read_frames(self,axis_data, obj, path_name):
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
            obj.keyframe_insert(data_path=path_name,
                                index=axis_idx,
                                frame=frame,
                                group=groupnames[path_name])
            fc = self.get_fcurve(obj,path_name,axis_idx)
            kf = self.get_keyframe(fc,frame)

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

    def get_fcurve(self,obj,path,idx):
        for fc in obj.animation_data.action.fcurves:
            if fc.data_path==path and fc.array_index == idx:
                return fc

        return None
    def get_keyframe(self,fc,frame):
        for kf in fc.keyframe_points:
            if (abs(frame-kf.co[0])<0.1):
                return kf
        return None

    def read_metadata(self,ctx,root):
        print("Starting metadata")
        for conn in root[1]:  # then we wrote metadta
            tgt_name = conn.attrib["name"]
            for anim in conn:
                anim_name = anim.attrib["subname"]
                #TODO fixme
                scene = ctx.scenes['Scene']
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

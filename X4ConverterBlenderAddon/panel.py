import re
import bpy
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty, IntProperty
from bpy.types import Panel, PropertyGroup
import xml.etree.ElementTree as ET
from X4ConverterBlenderAddon.common import *

class OBJECT_PT_X4Panel(Panel):
    bl_idname = "OBJECT_PT_X4Panel"
    bl_label = "X4 Animation Metadata"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

 
    def draw(self, context):
        layout = self.layout
        obj = context.object
        parent=find_anim_metadata_source(obj)
        if (parent is obj):
            row = layout.row()
            row.prop(obj,"X4_anim_data")
        elif (parent is None):
            row =layout.row()
            row.label(text="No animation data found for this node or its parents, and adding not yet implemented")
        else:
            text = "Inherited from "+parent.name
            row =layout.row()
            row.label(text=text)


# Assign a collection
class SubAnimationItem(PropertyGroup):
    subname: bpy.props.StringProperty(name="Subanimation Name", default="")
    start_frame: bpy.props.IntProperty(name="Start frame",default=0)
    end_frame: bpy.props.IntProperty(name="End frame",default=0)
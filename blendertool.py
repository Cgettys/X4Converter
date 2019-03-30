import bpy
from bpy import data as D
from bpy import context as C
from mathutils import *
from math import *
import xml.etree.ElementTree as ET
tree = ET.parse('/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.out.anixml')
root = tree.getroot()
scene_num = 1
scene = D.scenes[scene_num]
timeline_markers = scene.timeline_markers
print("Starting animations")
"""
<animation id="landinggear_front_01_location_X">
			<source id="landinggear_front_01_location_X-input">
				<float_array id="landinggear_front_01_location_X-input-array" count="1">0</float_array>
				<technique_common>
					<accessor source="#landinggear_front_01_location_X-input-array" count="1" stride="1">
						<param name="TIME" type="float" />
					</accessor>
				</technique_common>
			</source>
			<source id="landinggear_front_01_location_X-output">
				<float_array id="landinggear_front_01_location_X-output-array" count="1">0</float_array>
				<technique_common>
					<accessor source="#landinggear_front_01_location_X-output-array" count="2" stride="1">
						<param name="X" type="float" />
					</accessor>
				</technique_common>
			</source>
			<source id="landinggear_front_01_location_X-interpolation">
				<Name_array id="landinggear_front_01_location_X-interpolation-array" count="1">STEP</Name_array>
				<technique_common>
					<accessor source="#landinggear_front_01_location_X-interpolation-array" count="1" stride="1">
						<param name="INTERPOLATION" type="name" />
					</accessor>
				</technique_common>
			</source>
			<source id="landinggear_front_01_location_X-intangent">
				<float_array id="landinggear_front_01_location_X-intangent-array" count="2">0 0</float_array>
				<technique_common>
					<accessor source="#landinggear_front_01_location_X-intangent-array" count="2" stride="2">
						<param name="X" type="float" />
						<param name="Y" type="float" />
					</accessor>
				</technique_common>
			</source>
			<source id="landinggear_front_01_location_X-outtangent">
				<float_array id="landinggear_front_01_location_X-outtangent-array" count="2">0 0</float_array>
				<technique_common>
					<accessor source="#landinggear_front_01_location_X-outtangent-array" count="2" stride="2">
						<param name="X" type="float" />
						<param name="Y" type="float" />
					</accessor>
				</technique_common>
			</source>
"""

for anim in root[0]: # we wrote data first
    tgt = anim.find("channel").attrib["target"]
    tgt_name,tgt_channel = tgt.split("/")
    channel_group,channel_axis=tgt_channel.split(".")
    print("Target Name: {} ChannelType: {} Axis: {}".format(tgt_name,channel_group,channel_axis))

    if tgt_name not in D.actions:
        print("\tCreated action")
        D.actions.new(tgt_name)
    action = D.actions[tgt_name]

    #for src in anim.findall("source"):



print("Starting metadata")
for conn in root[1]: # then we wrote metadta
    tgt_name=conn.attrib["name"]
    for anim in conn:
        # TODO multiscene somehow
        anim_name=anim.attrib["animName"]
        start = int(anim.attrib["start"])
        end = int(anim.attrib["end"])
        if timeline_markers.get(anim_name+"Both"):
            continue
        elif start == end :
            timeline_markers.new(anim_name+"Both",frame=start)
        elif not timeline_markers.get(anim_name+"Start"):
            # TODO if is there check if same/warn
            timeline_markers.new(anim_name+"Start",frame=start)
        elif not timeline_markers.get(anim_name+"End"):
            # TODO if is there check if same/warn
            timeline_markers.new(anim_name+"End",frame=end)
        else:
            print("wut")
print("Done with metadata")


"""
for x in D.scenes[0].timeline_markers:
    D.scenes[0].timeline_markers.remove(x)
"""
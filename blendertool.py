import bpy
from bpy import data as D
from bpy import context as C
from mathutils import *
from math import *

for i in range(len(D.actions)):
    action = D.action[i]
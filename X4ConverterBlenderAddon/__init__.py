import bpy
from X4ConverterBlenderAddon.importer import *
from X4ConverterBlenderAddon.exporter import *
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

bl_info = {
    "name" : "X4FoundationsConverter",
    "author" : "Charlie Gettys",
    "description" : "",
    "blender" : (2, 80, 0),
    "location" : "",
    "warning" : "",
    "category" : "Import-Export"
}



def menu_func_import(self, context):
    self.layout.operator(ImportAsset.bl_idname, text="X4 Import")

def menu_func_export(self, context):
    self.layout.operator(ExportAsset.bl_idname, text="X4 Export")

def register():
    print("X4 Foundations Addon registered!")
    bpy.utils.register_class(ImportAsset)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)
    bpy.utils.register_class(ExportAsset)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    print("X4 Foundations Addon unregistered :(")
    bpy.utils.unregister_class(ImportAsset)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.utils.unregister_class(ExportAsset)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


import bpy
import bmesh
import json
import os

SCRIPT_PATH = bpy.context.space_data.text.filepath
FILE = os.path.abspath(
    os.path.dirname(SCRIPT_PATH) + "/../assets/geometries/pyramid_blender.json"
)
data = json.load(open(FILE))
verts = data["vertices"]
faces = data["faces"]
assert data["convention"] == "Blender"

print(FILE)
print("Vertices", verts)


def add_mesh(name, verts, faces, edges=None, col_name="Collection"):
    if edges is None:
        edges = []
    mesh = bpy.data.meshes.new(name)
    obj = bpy.data.objects.new(mesh.name, mesh)
    col = bpy.data.collections[col_name]
    col.objects.link(obj)
    bpy.context.view_layer.objects.active = obj
    mesh.from_pydata(verts, edges, faces)


add_mesh("pyramid", verts, faces)

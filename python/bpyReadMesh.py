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
colors = data.get("vertex_colors", None)
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
    if colors is not None:
        color_att = mesh.color_attributes.new("Color", "BYTE_COLOR", "POINT")
        for color, vertex in zip(colors, color_att.data):
            vertex.color_srgb = color + [255]


add_mesh("pyramid", verts, faces)

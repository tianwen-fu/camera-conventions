import bpy
import bmesh
from mathutils import Matrix, Vector, Euler
import json
import os

SCRIPT_PATH = bpy.context.space_data.text.filepath
FILE = os.path.abspath(
    os.path.dirname(SCRIPT_PATH) + "/../assets/cameras/robCam_blender.json"
)


def add_camera(name, lens, sensor_width, sensor_height, matrix_world):
    camera = bpy.data.cameras.new(name)
    cam_obj = bpy.data.objects.new(camera.name, camera)
    col = bpy.context.collection
    col.objects.link(cam_obj)
    bpy.context.view_layer.objects.active = cam_obj
    cam_obj.matrix_world = matrix_world
    camera.lens = lens
    camera.sensor_width = sensor_width
    # setting sensor_height wouldn't be useful because the rendering aspect ratio
    # is set globally across the scene by bpy.context.scene.render.resolution_x(y)
    camera.display_size = 1.0

    return cam_obj


def main():
    data = json.load(open(FILE))
    sensor_width = 36.0
    lens = data["fhat"] * sensor_width
    sensor_height = sensor_width / data["aspect_ratio"]
    add_camera("robCam", lens, sensor_width, sensor_height, Matrix(data["T"]))


if __name__ == "__main__":
    main()

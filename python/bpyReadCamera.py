import bpy
import bmesh
from mathutils import Matrix, Vector, Euler
import json
import os

SCRIPT_PATH = bpy.context.space_data.text.filepath
FILE = os.path.abspath(
    os.path.dirname(SCRIPT_PATH) + "/../assets/cameras/testCam_blender.json"
)
data = json.load(open(FILE))
sensor_width = 36.0
lens = data["fhat"] * sensor_width


def create_camera(K, Rt, K_resolution, sensor_width):
    # create camera
    bpy.ops.object.camera_add(enter_editmode=False)
    camera = bpy.context.object  # bpy.context.scene.objects['Camera']
    camera.matrix_world = Matrix(Rt)
    # focal length
    camera.data.sensor_width = sensor_width
    camera.data.lens = K[0][0] / max(K_resolution) * sensor_width
    # principle point
    camera.data.shift_x = -(K[0][2] - K_resolution[0] / 2) / K_resolution[0]
    camera.data.shift_y = (K[1][2] - K_resolution[1] / 2) / K_resolution[1]
    camera.data.display_size = 1.0

    return camera


def main():
    fx, fy, cx, cy = data["fx"], data["fy"], data["cx"], data["cy"]
    T = data["T"]
    resolution = (data["width"], data["height"])
    sensor_width = 36.0
    K = [[fx, 0, cx], [0, fy, cy], [0, 0, 1]]
    cam = create_camera(K, T, resolution, sensor_width)


if __name__ == "__main__":
    main()

from pathlib import Path
import json

import numpy as np
from camera_conventions.camera import Camera, parse_alicevision_camera
from camera_conventions.parser import parse_ini_convention


def main():
    conventions_path = Path(__file__).parent.parent / "conventions"
    aliceCamera = parse_ini_convention(conventions_path / "alicevision.ini")
    nersemble = parse_ini_convention(conventions_path / "nersemble.ini")
    blender = parse_ini_convention(conventions_path / "Blender.ini")
    opengl = parse_ini_convention(conventions_path / "OpenGL.ini")
    camera_param_path = Path("/nas/RobFacialPerformace/01/cameras")

    downsample_factor = 4
    intrinsics = None
    extrinsics = {}
    cameras_blender = {}
    cameras_opengl = {}
    for camera_param_file in camera_param_path.glob("*.txt"):
        raw_cam_data = {}
        camera = parse_alicevision_camera(camera_param_file, aliceCamera, raw_cam_data)
        camera_name = camera_param_file.stem[len("camera") :]
        camera_name = camera_name.zfill(3)
        print(camera_name)
        # f = raw_cam_data["focal_length"] / downsample_factor
        # we need a weird hack here to make everything look right
        w = raw_cam_data["resolution"][0] / downsample_factor
        h = raw_cam_data["resolution"][1] / downsample_factor
        f = camera.fhat * w
        intrinsics = np.array([[f, 0.0, w / 2], [0, f, h / 2], [0, 0, 1]])
        extrinsics[camera_name] = camera.convert(nersemble).T.tolist()
        cameras_blender[camera_name] = camera.convert(blender).to_dict()
        c_opengl = camera.convert(opengl)
        c_opengl.T = np.linalg.inv(c_opengl.T)
        cameras_opengl[camera_name] = c_opengl.to_dict()

    json.dump(
        {"intrinsics": intrinsics.tolist(), "world_2_cam": extrinsics},
        open("/nas/data/rob_npn_ner/camera_params/01/camera_params.json", "wt"),
    )

    json.dump(
        cameras_blender,
        open("/nas/data/rob_npn_ner/camera_params/01/camera_params_blender.json", "wt"),
        indent=2,
    )
    json.dump(
        cameras_opengl,
        open("/nas/data/rob_npn_ner/camera_params/01/camera_params_opengl.json", "wt"),
        indent=2,
    )


if __name__ == "__main__":
    main()

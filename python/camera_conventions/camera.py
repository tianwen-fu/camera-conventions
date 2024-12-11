import numpy as np
from dataclasses import dataclass
import json
from camera_conventions import Convention
from copy import deepcopy
from os import PathLike


@dataclass
class Camera:
    # we always assume fx=fy, and the principal point is at the center of the image
    fhat: float  # f / w
    T: np.ndarray  # 4x4 matrix
    aspect_ratio: float  # w / h
    convention: Convention

    def convert(self, convention: Convention):
        if self.convention == convention:
            return self
        T = self.T
        if not self.convention.is_world_to_camera:
            T = np.linalg.inv(T)
        # T is (R | t)
        opengl_to_self = self.convention.world_transformation_matrix
        opengl_to_convention = convention.world_transformation_matrix
        convention_to_self = opengl_to_self @ np.linalg.inv(opengl_to_convention)
        world_conv = np.eye(4)
        world_conv[:3, :3] = convention_to_self
        opengl_to_self = self.convention.camera_transformation_matrix
        opengl_to_convention = convention.camera_transformation_matrix
        self_to_convention = opengl_to_convention @ np.linalg.inv(opengl_to_self)
        cam_conv = np.eye(4)
        cam_conv[:3, :3] = self_to_convention
        T_new = cam_conv @ T @ world_conv
        if not convention.is_world_to_camera:
            T_new = np.linalg.inv(T_new)
        new_camera = deepcopy(self)
        new_camera.T = T_new
        new_camera.convention = convention
        return new_camera

    def to_json(self, filename: str):
        data = {
            "fhat": self.fhat,
            "T": self.T.tolist(),
            "aspect_ratio": self.aspect_ratio,
            "convention": self.convention.name,
        }
        with open(filename, "wt") as f:
            json.dump(data, f, indent=2)

    def from_json(file, convention: Convention):
        data = json.load(file)
        assert data["convention"] == convention.name
        return Camera(
            data["fhat"], np.array(data["T"]), data["aspect_ratio"], convention
        )


def parse_alicevision_camera(
    file_path: PathLike, alice_convention: Convention
) -> Camera:
    assert alice_convention.name == "AliceVision"
    f = None
    w, h = None, None
    T = None
    with open(file_path, "rt") as file:
        while (line := file.readline().strip()) != "":
            if line == "#focal length":
                f = float(file.readline().strip().split()[0])
            if line == "#resolution":
                w, h = map(int, file.readline().strip().split())
            if line == "MATRIX :":
                T = np.array(list(map(float, file.read().split()))).reshape(4, 4)
    T[:3, 3] /= 100  # convert from cm to m
    fhat = f / max(w, h)  # TODO: why max here?
    return Camera(fhat, T, w / h, alice_convention)

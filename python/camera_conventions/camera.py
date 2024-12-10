import numpy as np
from dataclasses import dataclass
from typing import ClassVar
import json
from . import Convention, convention_convertion_matrix
from copy import deepcopy


@dataclass
class Camera:
    # we always assume fx=fy, and the principal point is at the center of the image
    fhat: float  # f / w
    T: np.ndarray  # 4x4 matrix
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
        conversion = np.eye(4)
        conversion[:3, :3] = convention_to_self
        T_new = T @ conversion
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
            "convention": self.convention.name,
        }
        with open(filename, "wt") as f:
            json.dump(data, f, indent=2)

    def from_json(file, convention: Convention):
        data = json.load(file)
        assert data["convention"] == convention.name
        return Camera(data["fhat"], np.array(data["T"]), convention)

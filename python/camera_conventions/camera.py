import numpy as np
from dataclasses import dataclass
from typing import ClassVar
import json
from . import Convention, convention_convertion_matrix
from copy import deepcopy


@dataclass
class Camera:
    width: float
    height: float
    fx: float
    fy: float
    cx: float
    cy: float
    T: np.ndarray  # 4x4 matrix
    convention: Convention

    FLOAT_FORMAT: ClassVar[str] = "{:.10f}"

    def convert(self, convention: Convention):
        if self.convention == convention:
            return self
        matrix = convention_convertion_matrix(convention, self.convention)  # 3x3 matrix
        C = -self.T[:3, :3].T @ self.T[:3, 3]
        C_new = (C[np.newaxis, :] @ matrix.T).squeeze(axis=0)
        R = self.T[:3, :3]
        R_new = matrix @ R @ np.linalg.inv(matrix)
        T_new = np.eye(4)
        T_new[:3, :3] = R_new
        T_new[:3, 3] = -R_new @ C_new
        new_camera = deepcopy(self)
        new_camera.T = T_new
        new_camera.convention = convention
        return new_camera

    def to_txt(self, filename: str):
        assert self.convention == "OpenGL"
        with open(filename, "wt") as f:
            print(
                f"{self.FLOAT_FORMAT.format(self.width)} {self.FLOAT_FORMAT.format(self.height)}",
                file=f,
            )
            print(
                f"{self.FLOAT_FORMAT.format(self.fx)} {self.FLOAT_FORMAT.format(self.fy)}",
                file=f,
            )
            print(
                f"{self.FLOAT_FORMAT.format(self.cx)} {self.FLOAT_FORMAT.format(self.cy)}",
                file=f,
            )
            for row in self.T:
                print(" ".join(self.FLOAT_FORMAT.format(elem) for elem in row), file=f)

    def to_json(self, filename: str):
        data = {
            "width": self.width,
            "height": self.height,
            "fx": self.fx,
            "fy": self.fy,
            "cx": self.cx,
            "cy": self.cy,
            "T": self.T.tolist(),
            "convention": self.convention,
        }
        with open(filename, "wt") as f:
            json.dump(data, f, indent=2)
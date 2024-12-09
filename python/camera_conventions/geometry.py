from typing import Literal, Optional
from dataclasses import dataclass
import json
from os import PathLike

import numpy as np

from . import convention_convertion_matrix, Convention


@dataclass
class Geometry:
    convention: Convention
    verts: np.ndarray
    faces: np.ndarray
    vertex_colors: Optional[np.ndarray] = None

    def __post_init__(self):
        assert self.verts.ndim == 2
        assert self.verts.shape[1] == 3
        assert self.faces.ndim == 2
        assert self.faces.shape[1] == 3
        if self.vertex_colors is not None:
            assert self.vertex_colors.ndim == 2
            assert self.vertex_colors.shape[0] == self.verts.shape[0]
            assert self.vertex_colors.shape[1] == 3
            assert self.vertex_colors.dtype == np.uint8

    def dump(self, file: PathLike):
        data = {
            "convention": self.convention,
            "vertices": self.verts.tolist(),
            "faces": self.faces.tolist(),
        }
        if self.vertex_colors is not None:
            data["vertex_colors"] = self.vertex_colors.tolist()
        json.dump(data, open(file, "w"), indent=2)

    def convert(self, convention: Convention):
        if self.convention == convention:
            return self
        matrix = convention_convertion_matrix(convention, self.convention)
        return Geometry(
            convention=convention,
            verts=self.verts @ matrix.T,
            faces=self.faces,
            vertex_colors=self.vertex_colors,
        )

    @staticmethod
    def load(file):
        data = json.load(file)
        if "vertex_colors" in data:
            vertex_colors = np.array(data["vertex_colors"])
            assert vertex_colors.dtype == np.int32
            vertex_colors = vertex_colors.astype(np.uint8)
        else:
            vertex_colors = None
        return Geometry(
            convention=data["convention"],
            verts=np.array(data["vertices"]),
            faces=np.array(data["faces"]),
            # none if not present
            vertex_colors=vertex_colors,
        )

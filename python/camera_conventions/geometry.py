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

    def __post_init__(self):
        assert self.verts.ndim == 2
        assert self.verts.shape[1] == 3
        assert self.faces.ndim == 2
        assert self.faces.shape[1] == 3

    def dump(self, file: PathLike):
        data = {
            "convention": self.convention,
            "vertices": self.verts.tolist(),
            "faces": self.faces.tolist(),
        }
        json.dump(data, open(file, "w"), indent=2)

    def convert(self, convention: Convention):
        if self.convention == convention:
            return self
        matrix = convention_convertion_matrix(convention, self.convention)
        return Geometry(
            convention=convention,
            verts=self.verts @ matrix.T,
            faces=self.faces,
        )

    @classmethod
    def load(cls, file):
        data = json.load(file)
        return cls(
            convention=data["convention"],
            verts=np.array(data["vertices"]),
            faces=np.array(data["faces"]),
        )

import numpy as np
from dataclasses import dataclass
from typing import ClassVar
from . import Convention


@dataclass
class Camera:
    width: float
    height: float
    fx: float
    fy: float
    cx: float
    cy: float
    T: np.ndarray
    convention: Convention

    FLOAT_FORMAT: ClassVar[str] = "{:.10f}"

    def to_txt_file(self, filename: str):
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

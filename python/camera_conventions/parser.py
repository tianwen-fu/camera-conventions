import configparser
from camera_conventions import Convention
from os import PathLike
import numpy as np

__all__ = ["parse_ini_convention"]


class _ParsedCamera(Convention):
    def __init__(self, transform_matrix, w2c, name):
        self._transform_matrix = transform_matrix
        self._w2c = w2c
        self._name = name

    @property
    def world_transformation_matrix(self):
        return self._transform_matrix

    @property
    def is_world_to_camera(self):
        return self._w2c

    @property
    def name(self):
        return self._name


UNIT_VECTORS = dict(
    x=np.array([1.0, 0, 0]),
    y=np.array([0, 1.0, 0]),
    z=np.array([0, 0, 1.0]),
)


def parse_ini_convention(file_path: PathLike) -> Convention:
    config = configparser.ConfigParser()
    config.read(file_path)
    M = np.array(
        [
            UNIT_VECTORS[config["world"]["right"]],
            UNIT_VECTORS[config["world"]["up"]],
            UNIT_VECTORS[config["world"]["forward"]],
        ]
    ).T  # unit vectors are column vectors
    name = config["common"]["name"]
    # TODO: Implement the camera section
    if config["camera"]["transform"] == "cam2world":
        return _ParsedCamera(M, False, name)
    elif config["camera"]["transform"] == "world2cam":
        return _ParsedCamera(M, True, name)
    else:
        raise ValueError("Unknown transform")

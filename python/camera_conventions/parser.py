import configparser
from camera_conventions import Convention
from os import PathLike
from pathlib import Path
import numpy as np

__all__ = ["parse_ini_convention"]


class _ParsedCamera(Convention):
    def __init__(self, m_world, m_cam, w2c, name):
        self._m_world = m_world
        self._m_cam = m_cam
        self._w2c = w2c
        self._name = name

    @property
    def world_transformation_matrix(self):
        return self._m_world

    @property
    def camera_transformation_matrix(self):
        return self._m_cam

    @property
    def is_world_to_camera(self):
        return self._w2c

    @property
    def name(self):
        return self._name


UNIT_VECTORS = {
    "x": np.array([1.0, 0, 0]),
    "y": np.array([0, 1.0, 0]),
    "z": np.array([0, 0, 1.0]),
    "-x": np.array([-1.0, 0, 0]),
    "-y": np.array([0, -1.0, 0]),
    "-z": np.array([0, 0, -1.0]),
}


def parse_ini_convention(file_path: PathLike) -> Convention:
    config = configparser.ConfigParser()
    if not Path(file_path).exists():
        raise FileNotFoundError(f"File {file_path} not found")
    config.read(file_path)
    M_world = np.array(
        [
            UNIT_VECTORS[config["world"]["right"]],
            UNIT_VECTORS[config["world"]["up"]],
            UNIT_VECTORS[config["world"]["forward"]],
        ]
    ).T  # unit vectors are column vectors
    M_cam = np.array(
        [
            UNIT_VECTORS[config["camera"]["right"]],
            UNIT_VECTORS[config["camera"]["up"]],
            -UNIT_VECTORS[config["camera"]["lookat"]],
        ]
    ).T
    name = config["common"]["name"]
    if config["camera"]["transform"] == "cam2world":
        return _ParsedCamera(M_world, M_cam, False, name)
    elif config["camera"]["transform"] == "world2cam":
        return _ParsedCamera(M_world, M_cam, True, name)
    else:
        raise ValueError("Unknown transform")

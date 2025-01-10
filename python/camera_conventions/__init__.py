import numpy as np
from typing import Literal
from abc import ABC, abstractmethod

__all__ = ["Convention"]


class Convention(ABC):
    @property
    @abstractmethod
    def world_transformation_matrix(self) -> np.ndarray:
        """
        Gets the world transformation that transforms a point in the OpenGL world coordinates to the world coordinates
        """
        pass

    @property
    @abstractmethod
    def camera_transformation_matrix(self) -> np.ndarray:
        """
        Gets the camera transformation that transforms a point in the OpenGL camera coordinates to the camera coordinates
        """
        pass

    @property
    @abstractmethod
    def is_world_to_camera(self) -> bool:
        """
        Returns True if the transformation transforms world coordinates to camera coordinates
        """
        pass

    @property
    @abstractmethod
    def name(self) -> str:
        """
        Returns the name of the convention
        """
        pass

    def __eq__(self, other):
        return self.name == other.name

    def __repr__(self):
        return self.name

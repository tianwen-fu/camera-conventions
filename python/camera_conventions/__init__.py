import numpy as np
from typing import Literal

Convention = Literal["OpenGL", "Blender"]


def convention_convertion_matrix(tgt_conv: Convention, src_conv: Convention):  # 3x3
    if src_conv != "OpenGL" and tgt_conv == "OpenGL":
        return np.linalg.inv(convention_convertion_matrix(src_conv, "OpenGL"))
    elif src_conv != "OpenGL" and tgt_conv != "OpenGL":
        return convention_convertion_matrix(
            tgt_conv, "OpenGL"
        ) @ convention_convertion_matrix("OpenGL", src_conv)
    assert src_conv == "OpenGL"
    if tgt_conv == src_conv:
        result = np.eye(3)
    elif tgt_conv == "Blender":
        # x->y, y->z, z->x
        result = np.array([[0, 0, 1], [1, 0, 0], [0, 1, 0]])
    else:
        raise ValueError(f"Unknown convention {tgt_conv}")
    result = result.astype(np.float32)
    assert np.isclose(np.linalg.det(result), 1)
    return result

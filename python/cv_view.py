from argparse import ArgumentParser
import json

import numpy as np
import cv2

WIDTH = 600


def parse_args():
    parser = ArgumentParser()
    parser.add_argument("camera_params", help="Path to camera parameters file")
    parser.add_argument("geometry", help="Path to geometry file")
    return parser.parse_args()


def main():
    args = parse_args()
    geometry = json.load(open(args.geometry, "rt"))
    camera_params = json.load(open(args.camera_params, "rt"))
    assert camera_params["convention"] == "NerSemble"
    assert geometry["convention"] == "NerSemble"

    print(
        "Note: OpenCV does not have a z-buffer, so the triangles may be rendered in any order."
    )

    vertices = np.asarray(geometry["vertices"])
    vertex_colors = np.asarray(geometry["vertex_colors"])

    height = int(WIDTH / camera_params["aspect_ratio"])
    image = np.zeros((height, WIDTH, 3), dtype=np.uint8)
    f = camera_params["fhat"] * WIDTH
    cx = WIDTH / 2
    cy = height / 2
    intrinsics = np.array([[f, 0, cx], [0, f, cy], [0, 0, 1]])

    T = np.asarray(camera_params["T"])
    rvec = cv2.Rodrigues(T[:3, :3])[0]
    tvec = T[:3, 3]
    vertices_2d = cv2.projectPoints(vertices, rvec, tvec, intrinsics, None)[0]
    vertices_2d = vertices_2d.squeeze(axis=1)
    vertices_2d = vertices_2d.astype(np.int32).reshape(-1, 3, 2)
    vertex_colors = vertex_colors.astype(np.uint8).reshape(-1, 3, 3)
    for triangle, color in zip(vertices_2d, vertex_colors):
        cv2.fillPoly(image, [triangle], tuple(color[0].tolist()))

    cv2.imshow("image", image)
    cv2.waitKey(0)


if __name__ == "__main__":
    main()

#pragma once

#include <string>

struct CameraParams {
    float width, height;
    float fx, fy, cx, cy;
    float T[4][4];

    static CameraParams fromFile(const std::string_view &filename);
};
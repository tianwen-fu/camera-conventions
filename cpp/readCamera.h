#pragma once

#include <string>

struct CameraParams {
    float width, height;
    float fx, fy, cx, cy;
    float T[4][4];

    static CameraParams fromFile(const std::string_view &filename, float width, float height);

    /// compute the OpenGL perspective matrix
    /// @param dest in column-major order
    /// @param znear near plane
    /// @param zfar far plane
    void toPerspectiveMatrix(float *dest, float znear, float zfar) const;
};
#include "readCamera.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

CameraParams CameraParams::fromFile(const std::string_view &filename, float width, float height) {
    std::ifstream file(filename.data());
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " +
                                 std::string(filename));
    }
    const auto data = json::parse(file);
    if (data["convention"].get<std::string>() != "OpenGL") {
        throw std::runtime_error("Only OpenGL convention is supported");
    }

    CameraParams params{};
    params.width  = width;
    params.height = height;
    params.cx = width / 2;
    params.cy = height / 2;
    float fhat = data["fhat"];
    params.fx = fhat * width;
    params.fy = params.fx;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            data["T"][i][j].get_to(params.T[i][j]);

    return params;
}

void CameraParams::toPerspectiveMatrix(float *dest, const float znear, const float zfar) const {
    float ymax = height * znear / fy / 2;
    float xmax = width * znear / fx  / 2;
    float ymin = -ymax;
    float xmin = -xmax;
    // frustum
    memset(dest, 0, sizeof(float) * 16); // 4x4
    dest[0] = 2 * znear / (xmax - xmin);
    dest[5] = 2 * znear / (ymax - ymin);
    dest[8] = (xmax + xmin) / (xmax - xmin);
    dest[9] = (ymax + ymin) / (ymax - ymin);
    dest[10] = -(zfar + znear) / (zfar - znear);
    dest[11] = -1;
    dest[14] = -2 * zfar * znear / (zfar - znear);
}
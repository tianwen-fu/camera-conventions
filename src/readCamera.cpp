#include "readCamera.h"

#include <fstream>

CameraParams CameraParams::fromFile(const std::string_view &filename) {
    std::ifstream file(filename.data());
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(filename));
    }

    CameraParams params{};
    file >> params.width >> params.height;
    file >> params.fx >> params.fy >> params.cx >> params.cy;
    for (auto &row : params.T) {
        for (auto &elem : row) {
            file >> elem;
        }
    }

    return params;
}
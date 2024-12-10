#include "readCamera.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

CameraParams CameraParams::fromFile(const std::string_view &filename) {
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
    data["width"].get_to(params.width);
    data["height"].get_to(params.height);
    data["fx"].get_to(params.fx);
    data["fy"].get_to(params.fy);
    data["cx"].get_to(params.cx);
    data["cy"].get_to(params.cy);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            data["T"][i][j].get_to(params.T[i][j]);

    return params;
}
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "readGeometry.h"

using json = nlohmann::json;

void readGeometry(const std::string_view &filename,
    std::vector<std::array<float, 3>> &vertices,
    std::vector<std::array<float, 3>> &colors) {
    std::ifstream file(filename.data());
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " +
                                 std::string(filename));
    }
    const auto data = json::parse(file);
    if (data["convention"].get<std::string>() != "OpenGL") {
        throw std::runtime_error("Only OpenGL convention is supported");
    }

    for (const auto &vertex : data["vertices"]) {
        std::array<float, 3> v{};
        vertex.get_to(v);
        // v[2] += 0.5f;
        vertices.push_back(v);
    }

    for(const auto &color : data["vertex_colors"]) {
        std::array<float, 3> c{};
        color.get_to(c);
        for(auto &i : c)
            i /= 255.0f; // convert to [0, 1]
        colors.push_back(c);
    }
}
#pragma once
#include <string_view>
#include <vector>
#include <array>

void readGeometry(const std::string_view &filename, std::vector<std::array<float, 3>> &vertices, std::vector<std::array<float, 3>> &colors);
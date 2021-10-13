#pragma once

#include <iostream>

struct Renderable {
    std::string meshID;
    uint32_t textureIndex = 0;
    std::size_t index = 0;
};
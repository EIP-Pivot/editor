#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <pivot/ecs/Core/Component/description.hxx>
#include <pivot/ecs/Core/Data/value.hxx>

#include <ImGuizmo.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

using namespace pivot::ecs::component;
using namespace pivot::ecs::data;

Value createValue(BasicType type)
{
    switch (type) {
        case BasicType::String: return Value{""};
        case BasicType::Number: return Value{0.0};
        case BasicType::Integer: return Value{0};
        case BasicType::Boolean: return Value{false};
        case BasicType::Vec3: return Value{glm::vec3(0.0f)};
    }
    throw std::runtime_error("Ilegal BasicType value.");
}

Value createValue(RecordType types)
{
    Value value{Record{}};
    for (auto &[name, type]: types)
        std::visit(
            [&value, &name](auto &&arg) mutable {
                std::get<Record>(value).insert({name, createValue(arg)});
            },
            type);
    return value;
}
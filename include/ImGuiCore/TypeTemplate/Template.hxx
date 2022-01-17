#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <pivot/ecs/Core/Component/description.hxx>

#include <imgui.h>
#include <ImGuizmo.h>
#include <misc/cpp/imgui_stdlib.h>

using namespace pivot::ecs::component;

template <typename T>
void draw(T &value, std::string name);

#ifndef PIVOT_PROPERTY_DRAW_IMPLEMENTATION
#define PIVOT_PROPERTY_DRAW_IMPLEMENTATION

template <>
void draw(glm::vec3 &value, std::string name)
{
    ImGui::InputFloat3(name.c_str(), glm::value_ptr(value));
}

template <>
void draw(std::string &value, std::string name)
{
    ImGui::InputText(name.c_str(), &value);
}

template <>
void draw(int &value, std::string name)
{
    // ImGui::InputText("Name", &value);
}

#endif
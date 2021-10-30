#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <ImGuizmo.h>

#include <pivot/graphics/VulkanApplication.hxx>


class ImGuiManager
{
public:
    void newFrame(VulkanApplication &);
    void render();
};

#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <ImGuizmo.h>

#include <pivot/graphics/VulkanApplication.hxx>
#include <pivot/graphics/common.hxx>

class ImGuiManager
{
public:
    ImGuiManager(pivot::graphics::VulkanApplication &);
    void init();
    void newFrame();
    void render();

private:
    std::reference_wrapper<pivot::graphics::VulkanApplication> app;
    std::vector<ImTextureID> text;
    ImVec2 viewportSize;
};

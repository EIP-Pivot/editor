#include "ImGuiCore/ImGuiManager.hxx"
#include <pivot/ecs/Core/SceneManager.hxx>

#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include <Logger.hpp>

#include <unistd.h>
#include <sys/wait.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <iostream>


extern SceneManager gSceneManager;


//Loading Indicator
namespace ImGui {

    void LoadingIndicatorCircle(const char* label, const float size) {

        const float speed = 6;
        const int circle_count = 10;
        const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
        
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) {
            return;
        }

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);

        const ImVec2 pos = window->DC.CursorPos;
        const float circle_radius = size / 10.0f;
        const ImRect bb(pos, ImVec2(pos.x + size * 2.0f, pos.y + size * 2.0f));
        const ImGuiStyle& style = g.Style;
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return;

        // Render
        const float t = g.Time;
        const auto degree_offset = 2.0f * IM_PI / circle_count;

        for (int i = 0; i < circle_count; ++i) {
            const auto x = size * std::sin(degree_offset * i);
            const auto y = size * std::cos(degree_offset * i);
            const auto growth = std::max(0.0f, std::sin(t * speed - i * degree_offset));
            window->DrawList->AddCircleFilled(ImVec2(pos.x + size + x, pos.y + size - y), circle_radius + growth * circle_radius, GetColorU32(col));
        }
    }

    void LoadingIndicatorSpinner(const char* label, float radius, int thickness) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;
        
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        
        const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
        
        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius )*2, (radius + style.FramePadding.y)*2);
        
        
        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return;
        
        // Render
        window->DrawList->PathClear();

        int num_segments = 20;
        const float t = g.Time;
        int start = abs(ImSin(t*1.8f)*(num_segments-5));
        const float a_min = IM_PI*2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI*2.0f * ((float)num_segments-3) / (float)num_segments;
        const ImVec2 centre = ImVec2(pos.x+radius, pos.y+radius+style.FramePadding.y);

        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + 2 + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a+t*8) * radius, centre.y + ImSin(a+t*8) * radius));
        }
        window->DrawList->PathStroke(col, false, thickness);
    }
}

void ImGuiManager::newFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);

    if (ImGui::Button("Load Script")){
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileKey", "Choose File", ".pivotscript", ".");
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseFileKey")){
        if (ImGuiFileDialog::Instance()->IsOk()){
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            logger->debug("Script loader") << filePathName; LOGGER_ENDL
            //ImGui::LoadingIndicatorCircle("##Circle", 25);
            //ImGui::LoadingIndicatorSpinner("##spinner", 25, 6);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void ImGuiManager::render() { ImGui::Render(); }

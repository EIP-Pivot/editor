#include "ImGuiCore/SystemsEditor.hxx"

void SystemsEditor::create()
{
    ImGui::Begin("Systems");
    displaySystem<PhysicsSystem>("Physics system");
    if (ImGui::Button("Add System")) { ImGui::OpenPopup("AddSystem"); }
    createPopUp();
    ImGui::End();
}

void SystemsEditor::createPopUp()
{
    if (ImGui::BeginPopup("AddSystem")) {
        if (!gSceneManager.getCurrentLevel().hasSystem<PhysicsSystem>()) {
            if (ImGui::MenuItem("Physics system")) { addSystem<PhysicsSystem>(); }
        }
        ImGui::EndPopup();
    }
}
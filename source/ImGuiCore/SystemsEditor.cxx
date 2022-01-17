#include "ImGuiCore/SystemsEditor.hxx"

void SystemsEditor::create()
{
    ImGui::Begin("Systems");
    displaySystem();
    if (ImGui::Button("Add System")) { ImGui::OpenPopup("AddSystem"); }
    createPopUp();
    ImGui::End();
}

void SystemsEditor::createPopUp()
{
    auto &sm = gSceneManager.getCurrentLevel().getSystemManager();
    if (ImGui::BeginPopup("AddSystem")) {
        for (const auto &[name, description]: GlobalIndex::getSingleton()) {
            auto used = sm.getSystemUsed();
            if (std::find(used.begin(), used.end(), name) == used.end())
                if (ImGui::MenuItem("Physics system")) sm.useSystem(name);
        }
        ImGui::EndPopup();
    }
}

void SystemsEditor::displaySystem()
{
    auto &sm = gSceneManager.getCurrentLevel().getSystemManager();
    for (const auto &system: sm.getSystemUsed()) {
        if (ImGui::TreeNode(system.c_str())) ImGui::TreePop();
    }
}

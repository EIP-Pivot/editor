#include "ImGuiCore/ComponentEditor.hxx"
#include "ImGuiCore/TypeTemplate/Template.hxx"
#include "ImGuiCore/TypeTemplate/createValue.hxx"
#include <magic_enum.hpp>
#include <misc/cpp/imgui_stdlib.h>

void ComponentEditor::create(Entity entity)
{
    currentEntity = entity;
    ImGui::Begin("Component editor");
    createPopUp();
    displayComponent();
    if (ImGui::Button("Add Component")) { ImGui::OpenPopup("AddComponent"); }
    ImGui::End();
}

void ComponentEditor::create()
{
    ImGui::Begin("Component editor");
    ImGui::Text("No entity selected.");
    ImGui::End();
}

void ComponentEditor::setVectorObject(LevelId scene) { sceneObject[scene] = ObjectVector(); }

std::unordered_map<LevelId, ObjectVector> &ComponentEditor::getVectorObject() { return sceneObject; }

ObjectVector ComponentEditor::getObject() { return sceneObject[gSceneManager.getCurrentLevelId()]; }

void ComponentEditor::createPopUp()
{
    auto &cm = gSceneManager.getCurrentLevel().getComponentManager();
    if (ImGui::BeginPopup("AddComponent")) {
        for (const auto &[name, description]: GlobalIndex::getSingleton()) {
            if (cm.GetComponent(currentEntity, cm.GetComponentId(name).value()) == std::nullopt) {
                if (ImGui::MenuItem(name.c_str())) { addComponent(description); }
            }
        }
        ImGui::EndPopup();
    }
}

void ComponentEditor::displayComponent()
{
    auto &cm = gSceneManager.getCurrentLevel().getComponentManager();
    for (auto [description, component]: cm.GetAllComponents(currentEntity)) {
        if (ImGui::TreeNode(description.name.c_str())) {
            ImGui::TreePop();
            ImGui::Indent();
            std::visit([](auto &&arg) { 
                draw(arg, "oui");
                }, component);
            ImGui::Unindent();
        }
    }
}

void ComponentEditor::addComponent(const Description &description)
{
    auto &cm = gSceneManager.getCurrentLevel().getComponentManager();
    auto id = cm.GetComponentId(description.name).value();
    Value newComponent;
    std::visit([&newComponent](auto &&arg) mutable { newComponent = createValue(arg); }, description.type);
    cm.AddComponent(currentEntity, newComponent, id);
}

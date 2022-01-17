#include "ImGuiCore/ComponentEditor.hxx"
#include "ImGuiCore/TypeTemplate/Template.hxx"
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
    for (const auto &[description, component]: cm.GetAllComponents(currentEntity)) {
        if (ImGui::TreeNode(description.name.c_str())) {
            ImGui::TreePop();
            ImGui::Indent();
            for (const auto &property: description.properties) {
                std::visit([property](auto &&arg) { draw(arg, property.name); },
                           description.getProperty(component, property.name));
            }
            ImGui::Unindent();
        }
    }
}

void ComponentEditor::addComponent(const Description &description)
{
    auto &cm = gSceneManager.getCurrentLevel().getComponentManager();
    auto id = cm.GetComponentId(description.name).value();
    std::map<std::string, Description::Property::ValueType> properties;
    for (const auto &property: description.properties) {
        switch (property.type) {
            case Description::Property::Type::String: properties.insert({property.name, std::string("")}); break;
            case Description::Property::Type::Vec3: properties.insert({property.name, glm::vec3(0.0f)}); break;
            case Description::Property::Type::Number: properties.insert({property.name, 0}); break;
        }
    }
    cm.AddComponent(currentEntity, description.create(properties), id);
}

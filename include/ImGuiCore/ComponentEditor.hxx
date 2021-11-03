#pragma once

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

#include <pivot/ecs/Core/types.hxx>

#include <pivot/ecs/Components/RigidBody.hxx>
#include <pivot/ecs/Components/Gravity.hxx>
#include <pivot/graphics/types/RenderObject.hxx>
#include <pivot/graphics/types/UniformBufferObject.hxx>

#include <iostream>

#include <pivot/ecs/Core/SceneManager.hxx>

extern SceneManager gSceneManager;

using ObjectVector = std::vector<std::reference_wrapper<const pivot::graphics::RenderObject>>;

class ComponentEditor
{
public:
    void create(Entity entity);
    void create();

    void setVectorObject(LevelId scene);
    std::unordered_map<LevelId, ObjectVector> &getVectorObject();
    ObjectVector getObject();

    template <typename T>
    void addComponent(Entity entity, T component)
    {
        if (!gSceneManager.getCurrentLevel().isRegister<T>()) gSceneManager.getCurrentLevel().RegisterComponent<T>();
        gSceneManager.getCurrentLevel().AddComponent<T>(entity, component);
    }
private:
    template <typename T>
    void addComponent(T component)
    {
        if (!gSceneManager.getCurrentLevel().isRegister<T>())
            gSceneManager.getCurrentLevel().RegisterComponent<T>();
        gSceneManager.getCurrentLevel().AddComponent<T>(currentEntity, component);
    }
    template <typename T>
    void displayComponent(std::string name)
    {
        if (gSceneManager.getCurrentLevel().hasComponent<T>(currentEntity)) {
            if (ImGui::TreeNode(name.c_str())) {
                ImGui::TreePop();
                createComponent(gSceneManager.getCurrentLevel().GetComponent<T>(currentEntity));
            }
        }
    }

    void createPopUp();
    void createComponent(RigidBody &rigidBody);
    void createComponent(Gravity &gravity);
    void createComponent(pivot::graphics::RenderObject &renderObject);

private :
    Entity currentEntity;
    glm::vec3 matrixTranslation{0.f};
    glm::vec3 matrixRotation{0.f};
    glm::vec3 matrixScale{1.f};
    std::unordered_map<LevelId, ObjectVector> sceneObject;
    std::array<std::string, 8> textures = {"rouge", "vert", "bleu", "cyan", "orange", "jaune", "blanc", "violet"};
    std::array<std::string, 2> models = {"cube", "plane"};
};

template <>
void ComponentEditor::addComponent(Entity entity, pivot::graphics::RenderObject renderObject);
template <>
void ComponentEditor::addComponent(pivot::graphics::RenderObject renderObject);
#pragma once

#include <imgui.h>
#include <ImGuizmo.h>

#include "Systems/PhysicsSystem.hxx"

#include <pivot/ecs/Core/SceneManager.hxx>

extern SceneManager gSceneManager;

class SystemsEditor
{
public:
    void create();
    void addSystem();

    template <typename T>
    void addSystem()
    {
        auto system = gSceneManager.getCurrentLevel().RegisterSystem<T>();
        gSceneManager.getCurrentLevel().SetSystemSignature<T>(system->getSignature());
        system->Init();
    }
private:
    template <typename T>
    void displaySystem(std::string name)
    {
        if (gSceneManager.getCurrentLevel().hasSystem<T>()) {
            if (ImGui::TreeNode(name.c_str())) {
                ImGui::TreePop();
            }
        }
    }

    void createPopUp();
};

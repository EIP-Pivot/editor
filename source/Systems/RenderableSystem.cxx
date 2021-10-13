#include "Systems/RenderableSystem.hxx"

#include "pivot/ecs/Components/Transform.hxx"
#include "Components/Renderable.hxx"
#include <pivot/ecs/Core/SceneManager.hxx>

#include "iostream"

extern SceneManager gSceneManager;

void RenderableSystem::Init() {}

void RenderableSystem::Update(float dt)
{
    for (auto const &entity: mEntities) {
        auto &transform = gSceneManager.getCurrentLevel().GetComponent<pivot::ecs::component::Transform>(entity);
        auto &renderable = gSceneManager.getCurrentLevel().GetComponent<Renderable>(entity);
        gSceneManager.getCurrentLevel().obj[renderable.index].objectInformation.transform.setPosition(transform.position);
    }
}
#include "Systems/PhysicsSystem.hxx"
#include "pivot/ecs/Components/Gravity.hxx"
#include "pivot/ecs/Components/RigidBody.hxx"
#include <pivot/ecs/Core/SceneManager.hxx>
#include <pivot/graphics/types/RenderObject.hxx>

extern SceneManager gSceneManager;

void PhysicsSystem::Init() {}

void PhysicsSystem::Update(float dt)
{
    for (auto const &entity: mEntities) {
        auto &rigidBody = gSceneManager.getCurrentLevel().GetComponent<RigidBody>(entity);
        auto &renderObject = gSceneManager.getCurrentLevel().GetComponent<RenderObject>(entity);

        // Forces
        auto const &gravity = gSceneManager.getCurrentLevel().GetComponent<Gravity>(entity);

        renderObject.objectInformation.transform.addPosition(rigidBody.velocity * dt);

        rigidBody.velocity += gravity.force * dt;
    }
}
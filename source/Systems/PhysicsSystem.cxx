#include "Systems/PhysicsSystem.hxx"
#include "pivot/ecs/Components/Gravity.hxx"
#include "pivot/ecs/Components/RigidBody.hxx"
#include <pivot/graphics/types/RenderObject.hxx>

#include <pivot/ecs/Core/SceneManager.hxx>

extern SceneManager gSceneManager;

void PhysicsSystem::Update(float dt)
{
    for (auto const &entity: mEntities) {
        auto &rigidBody = gSceneManager.getCurrentLevel().GetComponent<RigidBody>(entity);
        auto &renderObject = gSceneManager.getCurrentLevel().GetComponent<pivot::graphics::RenderObject>(entity);

        // Forces
        auto const &gravity = gSceneManager.getCurrentLevel().GetComponent<Gravity>(entity);

        renderObject.objectInformation.transform.addPosition(rigidBody.velocity * dt);

        rigidBody.velocity += gravity.force * dt;
    }
}

Signature PhysicsSystem::getSignature()
{
    if (!gSceneManager.getCurrentLevel().isRegister<pivot::graphics::RenderObject>())
        gSceneManager.getCurrentLevel().RegisterComponent<pivot::graphics::RenderObject>();
    if (!gSceneManager.getCurrentLevel().isRegister<Gravity>())
        gSceneManager.getCurrentLevel().RegisterComponent<Gravity>();
    if (!gSceneManager.getCurrentLevel().isRegister<RigidBody>())
        gSceneManager.getCurrentLevel().RegisterComponent<RigidBody>();
    Signature signature;
    signature.set(gSceneManager.getCurrentLevel().GetComponentType<Gravity>());
    signature.set(gSceneManager.getCurrentLevel().GetComponentType<RigidBody>());
    signature.set(gSceneManager.getCurrentLevel().GetComponentType<pivot::graphics::RenderObject>());
    return signature;
}

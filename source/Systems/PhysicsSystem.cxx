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
        auto &renderObject = gSceneManager.getCurrentLevel().GetComponent<RenderObject>(entity);

        // Forces
        auto const &gravity = gSceneManager.getCurrentLevel().GetComponent<Gravity>(entity);

        renderObject.objectInformation.transform.addPosition(rigidBody.velocity * dt);

        rigidBody.velocity += gravity.force * dt;
    }
}

Signature PhysicsSystem::getSignature()
{
    if (!gSceneManager.getCurrentLevel().isRegister<RenderObject>())
        gSceneManager.getCurrentLevel().RegisterComponent<RenderObject>();
    if (!gSceneManager.getCurrentLevel().isRegister<Gravity>())
        gSceneManager.getCurrentLevel().RegisterComponent<Gravity>();
    if (!gSceneManager.getCurrentLevel().isRegister<RigidBody>())
        gSceneManager.getCurrentLevel().RegisterComponent<RigidBody>();
    Signature signature;
    signature.set(gSceneManager.getCurrentLevel().GetComponentType<Gravity>());
    signature.set(gSceneManager.getCurrentLevel().GetComponentType<RigidBody>());
    signature.set(gSceneManager.getCurrentLevel().GetComponentType<RenderObject>());
    return signature;
}

void physicsSystem(std::vector<std::vector<std::any>> entities)
{
    for (const auto &entity: entities) {
        auto gravity = std::any_cast<std::reference_wrapper<Gravity>>(entity[0]);
        auto rigidBody = std::any_cast<std::reference_wrapper<RigidBody>>(entity[1]);
        rigidBody.get().velocity += gravity.get().force * 0.16f;
    }
    // for (const auto &component: components) std::cout << "\t" << component.type().name() << "\n";
    // auto gravity = std::any_cast<std::reference_wrapper<Gravity>>(components[0]);
    // auto rigidBody = std::any_cast<std::reference_wrapper<RigidBody>>(components[1]);
}

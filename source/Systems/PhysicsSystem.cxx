#include "Systems/PhysicsSystem.hxx"

using namespace pivot::ecs;

void physicsSystem(const systems::Description &systemDescription, systems::Description::systemArgs &entities, const event::Event &event)
{
    auto dt = (float)std::get<double>(event.payload);
    for (auto combination: entities) {
        auto gravity = combination[0].get();
        auto rigidBody = combination[1].get();
        auto najo = combination[2].get();
        auto thomas = combination[3].get();

        auto &force = std::get<glm::vec3>(std::get<data::Record>(gravity).at("force"));

        auto &velocity = std::get<glm::vec3>(std::get<data::Record>(rigidBody).at("velocity"));
        velocity += force * dt;
        combination[1].set(rigidBody);

        auto &note = std::get<double>(std::get<data::Record>(najo).at("note"));
        note += 1 * dt;
        combination[2].set(najo);

        auto &qi = std::get<double>(std::get<data::Record>(thomas).at("QI"));
        qi += note * dt;
        combination[3].set(thomas);
    }
}

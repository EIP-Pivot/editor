#pragma once

#include <pivot/ecs/Core/Systems/description.hxx>

using namespace pivot::ecs;

void physicsSystem(const systems::Description &, systems::Description::systemArgs &, const event::Event &);
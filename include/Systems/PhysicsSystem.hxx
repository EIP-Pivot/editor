#pragma once

#include "pivot/ecs/Core/System.hxx"

#include <vector>
#include <any>

class PhysicsSystem : public System
{
public:
    void Update(float dt);

    Signature getSignature();
};

void physicsSystem(std::vector<std::any> components);
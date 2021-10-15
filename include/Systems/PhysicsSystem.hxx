#pragma once

#include "pivot/ecs/Core/System.hxx"

class PhysicsSystem : public System
{
public:
    void Update(float dt);

    Signature getSignature();
};
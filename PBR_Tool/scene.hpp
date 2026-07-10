#pragma once
#include "event.hpp"

namespace pt
{
    class Scene
    {
    public:
        virtual void onStart() {}
        virtual void onEvent(Event& event) {}
        virtual void onUpdate(float deltaTime) {}
        virtual void onRender() {}
    };
}
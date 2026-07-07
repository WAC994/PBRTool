#pragma once

namespace pt
{
    class Scene
    {
    public:
        virtual void init() {}
        virtual void update(float deltaTime) {}
        virtual void render() {}
    };
}
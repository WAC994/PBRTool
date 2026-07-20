#pragma once
#include <glm/glm.hpp>

namespace pt
{
    struct LightData
    {
        glm::vec3 color = glm::vec3(1.0f); 
        glm::vec3 position = glm::vec3(0.0f); 
    };
    
    class Light
    {
    public:
        Light();
        Light(const glm::vec3& color, const glm::vec3& position);
        ~Light();
        LightData& data();
    private:
        uint32_t m_index;
    };

    class LightManager
    {
    public:
        static void init();
        static void update();
    };
}
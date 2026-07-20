#include "lights.hpp"
#include "utilities.hpp"
#include <array>
#include <iostream>

namespace pt
{
    class LightManagerImpl
    {
    public:
        std::array<LightData, MAX_LIGHTS> lights;
        uint32_t lightsCount = 0;
        LightData emptyLight;
    private:
        uint32_t m_buffer;
        bool m_lightBufferCreated = false;
    public:
        void init()
        {
            if (!m_lightBufferCreated)
            {
                glCreateBuffers(1, &m_buffer);
                glNamedBufferStorage(m_buffer, (sizeof(LightData) * MAX_LIGHTS) + sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
                emptyLight = { glm::vec3(0.0f), glm::vec3(0.0f) };
                m_lightBufferCreated = true;
            }
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIGHTS_SSBO_BINDING_POINT, m_buffer);
        }

        void update()
        {
            glNamedBufferSubData(m_buffer, 0, sizeof(uint32_t), &lightsCount);
            glNamedBufferSubData(m_buffer, sizeof(uint32_t), sizeof(LightData) * lightsCount, lights.data());
        }

        bool status()
        {
            return m_buffer;
        }
    };

    static LightManagerImpl instance;

    Light::Light()
    {
        if (!instance.status())
        {
            std::cout << "Light manager not initialize\nd";
            return;
        }

        if (instance.lightsCount >= MAX_LIGHTS)
        {
            std::cout << "Maximum lights reached\n";
            return;
        }

        m_index = instance.lightsCount++;
        instance.lights[m_index] = {};
    }

    Light::Light(const glm::vec3& color, const glm::vec3& position)
    {
        if (!instance.status())
        {
            std::cout << "Light manager not initialize\nd";
            return;
        }

        if (instance.lightsCount >= MAX_LIGHTS)
        {
            std::cout << "Maximum lights reached\n";
            m_index = MAX_LIGHTS;
            return;
        }

        m_index = instance.lightsCount++;
        instance.lights[m_index] = { color, position };
    }

    Light::~Light()
    {
        if (m_index == MAX_LIGHTS)
        {
            return;
        }

        for (size_t i = m_index; i < instance.lightsCount - 1; i++)
        {
            instance.lights[i] = instance.lights[i + 1];
        }
        instance.lightsCount--;
    }

    LightData& Light::data()
    {
        if (m_index == MAX_LIGHTS)
        {
            return instance.emptyLight;
        }
        return instance.lights[m_index];
    }

    void LightManager::init()
    {
        instance.init();
    }

    void LightManager::update()
    {
        instance.update();
    }
}
#pragma once
#include <cstdint>
#include <functional>
#include <string_view>

enum class ET // Event Type
{
    N,  // None
    K,  // key
    MB, // Mouse Button
    MP, // Mouse Position
    MS, // Mouse Scroll
    FR, // Framebuffer Resize
    WC, // Window Close
};

#define EVENT_TYPE_MACRO(type)                                                                                         \
    static ET getStaticType()                                                                                          \
    {                                                                                                                  \
        return type;                                                                                                   \
    }                                                                                                                  \
    ET getType() override                                                                                              \
    {                                                                                                                  \
        return getStaticType();                                                                                        \
    }                                                                                                                  \
    std::string_view logEvent() override                                                                               \
    {                                                                                                                  \
        return #type;                                                                                                  \
    }

struct Event
{
    virtual ET getType() = 0;
    virtual std::string_view logEvent() = 0;
};

struct KeyEvent : public Event
{
    int32_t key, action;
    bool GetKey(int32_t key, int32_t action) const
    {
        return (key == this->key && action == this->action);
    }
    EVENT_TYPE_MACRO(ET::K)
};

struct MouseButtonEvent : public Event
{
    int button, action;
    bool GetKey(int button, int action) const
    {
        return (button == this->button && action == this->action);
    }
    EVENT_TYPE_MACRO(ET::MB)
};

struct MousePositionEvent : public Event
{
    double x, y, dx, dy;
    EVENT_TYPE_MACRO(ET::MP)
};

struct MouseScrollEvent : public Event
{
    double x, y;
    EVENT_TYPE_MACRO(ET::MS)
};

struct FramebufferResizeEvent : public Event
{
    int width, height;
    EVENT_TYPE_MACRO(ET::FR)
};

struct WindowCloseEvent : public Event
{
    bool close;
    EVENT_TYPE_MACRO(ET::WC)
};

class EventManager
{
public:
    EventManager(Event& event) : m_eventReference{ event }
    {
    }

    template <typename ETName> using EventFunctionTemplate = std::function<void(ETName&)>;

    template <typename ETName> void dispatch(EventFunctionTemplate<ETName> eventFunction)
    {
        if (ETName::getStaticType() == m_eventReference.getType())
            eventFunction(*(ETName*)&m_eventReference);
    }
private:
    Event& m_eventReference;
};

#include <pbr_tool_core.hpp>
#include "demo.hpp"

int main()
{
    App& app = App::getInstance();
    app.setScene<ScreenColor>();
    app.run();
}
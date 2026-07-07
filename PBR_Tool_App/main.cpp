#include <pbr_tool_core.hpp>
#include "demo.hpp"

int main()
{
    pt::App& app = pt::App::getInstance();
    app.setScene<DemoScene>();
    app.run();
}
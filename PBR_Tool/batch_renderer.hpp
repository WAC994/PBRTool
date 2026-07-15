#pragma once
#include "model.hpp"
#include "utilities.hpp"

namespace pt
{
    class BatchRenderer
    {
    public:
        static void init();
        static void batch(Model& model);
        static void draw();
    };
}
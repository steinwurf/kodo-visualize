#pragma once

#include <cstdint>

namespace kodo_visualize
{
    struct pixel_format
    {
        uint32_t r_mask;
        uint32_t g_mask;
        uint32_t b_mask;
        uint32_t a_mask;
    };
}

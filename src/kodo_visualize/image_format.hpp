#pragma once

#include <cstdint>
#include <vector>
#include <SDL.h>

namespace kodo_visualize
{
    class image_format
    {

    public:

        image_format()
        { }

        image_format(uint8_t bits_per_pixel, uint32_t r_mask,
            uint32_t g_mask, uint32_t b_mask, uint32_t a_mask):
            m_bits_per_pixel(bits_per_pixel),
            m_r_mask(r_mask),
            m_g_mask(g_mask),
            m_b_mask(b_mask),
            m_a_mask(a_mask)
        { }

    public:

        uint8_t m_bits_per_pixel;
        uint32_t m_r_mask;
        uint32_t m_g_mask;
        uint32_t m_b_mask;
        uint32_t m_a_mask;

        std::vector<SDL_Color> m_palette;
    };
}

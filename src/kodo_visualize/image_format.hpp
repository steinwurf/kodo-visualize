// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <vector>
#include <SDL.h>

namespace kodo_visualize
{
    class image_format
    {

    public:

        image_format():
            m_bits_per_pixel(0),
            m_r_mask(0),
            m_g_mask(0),
            m_b_mask(0),
            m_a_mask(0)
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

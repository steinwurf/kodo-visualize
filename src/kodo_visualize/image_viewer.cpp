// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cstdint>
#include <mutex>

#include <SDL.h>

#include "image_format.hpp"

#include "image_viewer.hpp"

namespace kodo_visualize
{

    image_viewer::image_viewer(image_format format, int32_t width,
        int32_t height, int32_t x, int32_t y):
        m_width(width),
        m_height(height),
        m_x(x),
        m_y(y)
    {
        m_surface = SDL_CreateRGBSurface(
            0,
            m_width,
            m_height,
            format.m_bits_per_pixel,
            format.m_r_mask,
            format.m_g_mask,
            format.m_b_mask,
            format.m_a_mask);

        // We need a palette if the number fo bits for each pixel is 8 or less
        if (format.m_bits_per_pixel <= 8)
        {
            SDL_SetPaletteColors(
                m_surface->format->palette,
                format.m_palette.data(),
                0,
                format.m_palette.size());
        }

        m_size =  m_surface->h * m_surface->pitch;
    }

    void image_viewer::render(SDL_Renderer* renderer)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(
            renderer, m_surface);
        SDL_Rect rect = { m_x, m_y, m_width, m_height };
        SDL_RenderCopy(renderer, texture, 0, &rect);
        SDL_DestroyTexture(texture);
    }

    image_viewer::~image_viewer()
    {
        SDL_FreeSurface(m_surface);
    }
}

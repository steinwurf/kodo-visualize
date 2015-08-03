// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <mutex>

#include <SDL.h>

#include "i_renderable.hpp"
#include "image_format.hpp"

namespace kodo_visualize
{
    class image_viewer : public i_renderable
    {

    public:

        image_viewer(image_format format, int32_t width, int32_t height,
            int32_t x, int32_t y);

        void render(SDL_Renderer* renderer);
        ~image_viewer();

        template<class Decoder>
        void display_decoding(Decoder& decoder)
        {
            std::lock_guard<std::mutex> lock(m_lock);

            SDL_LockSurface(m_surface);
            decoder.copy_from_symbols((uint8_t*)m_surface->pixels, m_size);
            SDL_UnlockSurface(m_surface);
        }

    private:

        int32_t m_width;
        int32_t m_height;
        int32_t m_size;
        int32_t m_x;
        int32_t m_y;

        SDL_Surface* m_surface;

        std::mutex m_lock;
    };
}

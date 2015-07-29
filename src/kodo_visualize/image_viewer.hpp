#pragma once

#include <cstdint>
#include <mutex>

#include <sak/storage.hpp>
#include <SDL.h>

#include "i_renderable.hpp"
#include "image_format.hpp"

namespace kodo_visualize
{
    class image_viewer : public i_renderable
    {

    public:

        image_viewer(image_format format, uint32_t width, uint32_t height,
            uint32_t x, uint32_t y);

        void render(SDL_Renderer* renderer);
        ~image_viewer();

        template<class Decoder>
        void display_decoding(Decoder decoder)
        {
            std::lock_guard<std::mutex> lock(m_lock);

            SDL_LockSurface(m_surface);
            decoder->copy_symbols(sak::storage(m_surface->pixels, m_size));
            SDL_UnlockSurface(m_surface);
        }

    private:

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_size;
        uint32_t m_x;
        uint32_t m_y;

        SDL_Surface* m_surface;

        std::mutex m_lock;
    };
}
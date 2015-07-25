#include <cstdint>
#include <mutex>

#include <SDL2/SDL.h>

#include "pixel_format.hpp"

#include "image_viewer.hpp"

namespace kodo_visualize
{

    image_viewer::image_viewer(pixel_format format, uint32_t width,
        uint32_t height, uint32_t x, uint32_t y):
        m_width(width),
        m_height(height),
        m_x(x),
        m_y(y)
    {
        m_surface = SDL_CreateRGBSurface(
            0,
            m_width,
            m_height,
            24,
            format.r_mask,
            format.g_mask,
            format.b_mask,
            format.a_mask);
        m_size =  m_surface->h * m_surface->pitch;
    }

    void image_viewer::render(SDL_Renderer* renderer)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        SDL_Texture* texture =
            SDL_CreateTextureFromSurface(renderer, m_surface);
        SDL_Rect rect = {
            (int)m_x,
            (int)m_y,
            (int)m_width,
            (int)m_height
        };
        SDL_RenderCopy(renderer, texture, 0, &rect);
    }

    image_viewer::~image_viewer()
    {
        SDL_FreeSurface(m_surface);
    }
}

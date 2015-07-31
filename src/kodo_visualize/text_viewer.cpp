// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cassert>
#include <cstdint>
#include <string>
#include <sstream>

#include <SDL.h>
#include <SDL_ttf.h>

#include "i_renderable.hpp"

#include "text_viewer.hpp"

namespace kodo_visualize
{
    text_viewer::text_viewer(int32_t x, int32_t y):
        m_x(x),
        m_y(y),
        m_font(NULL)
    {
        m_color = { 255, 255, 255, 255 };
        if (!TTF_WasInit())
        {
            assert(TTF_Init() == 0);
        }
    }

    void text_viewer::set_text(const std::string& text)
    {
        m_text = text;
    }

    void text_viewer::set_color(uint8_t red, uint8_t green, uint8_t blue)
    {
        m_color = { red, green, blue, 255 };
    }

    void text_viewer::set_font(const std::string& path_to_font, uint32_t size)
    {
        if (m_font)
            TTF_CloseFont(m_font);
        m_font = TTF_OpenFont(path_to_font.c_str(), size);
        assert(m_font);
    }

    void text_viewer::render(SDL_Renderer* renderer)
    {
        if (m_text.empty())
            return;

        // make sure we have a font.
        assert(m_font);

        int32_t y_offset = 0;
        std::istringstream text_stream(m_text);
        std::string line;
        while (std::getline(text_stream, line))
        {
            SDL_Surface* text_surface = TTF_RenderText_Solid(
                m_font, line.c_str(), m_color);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(
                renderer, text_surface);
            SDL_FreeSurface(text_surface);
            int32_t width;
            int32_t height;
            SDL_QueryTexture(texture, NULL, NULL, &width, &height);
            SDL_Rect rect = {
                m_x,
                m_y + y_offset,
                width,
                height

            };
            SDL_RenderCopy(renderer, texture, 0, &rect);
            y_offset += height;
        }
    }

    text_viewer::~text_viewer()
    {
        if (m_font)
            TTF_CloseFont(m_font);

        if (TTF_WasInit())
            TTF_Quit();
    }
}

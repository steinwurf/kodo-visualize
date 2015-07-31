// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <algorithm>
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
        m_width(0),
        m_height(0),
        m_dirty(true),
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
        m_dirty = true;
        m_text = text;
    }

    void text_viewer::set_color(uint8_t red, uint8_t green, uint8_t blue)
    {
        m_color = { red, green, blue, 255 };
    }

    void text_viewer::set_font(const std::string& path_to_font, uint32_t size)
    {
        m_dirty = true;
        if (m_font)
            TTF_CloseFont(m_font);
        m_font = TTF_OpenFont(path_to_font.c_str(), size);
        assert(m_font);
    }

    void text_viewer::render(SDL_Renderer* renderer)
    {
        auto surfaces = generate_surfaces();

        int32_t y_offset = 0;
        for (auto& surface : surfaces)
        {
            SDL_Rect rect = {
                m_x,
                m_y + y_offset,
                surface->w,
                surface->h

            };
            y_offset += surface->h;

            SDL_Texture* texture = SDL_CreateTextureFromSurface(
                renderer, surface);

            SDL_RenderCopy(renderer, texture, 0, &rect);
            SDL_FreeSurface(surface);
        }
    }

    int32_t text_viewer::width()
    {
        calculate_dimentions();
        return m_width;
    }

    int32_t text_viewer::height()
    {
        calculate_dimentions();
        return m_height;
    }

    std::vector<SDL_Surface*> text_viewer::generate_surfaces()
    {
        std::vector<SDL_Surface*> textures;

        if (m_text.empty())
            return textures;

        // make sure we have a font.
        assert(m_font);

        std::istringstream text_stream(m_text);
        std::string line;
        while (std::getline(text_stream, line))
        {
            SDL_Surface* text_surface = TTF_RenderText_Solid(
                m_font, line.c_str(), m_color);
            textures.push_back(text_surface);
        }

        return textures;
    }

    void text_viewer::calculate_dimentions()
    {
        if (!m_dirty)
            return;

        auto surfaces = generate_surfaces();

        for (auto& surface : surfaces)
        {
            m_width = std::max(m_width, surface->w);
            m_height += surface->h;
            SDL_FreeSurface(surface);
        }

        m_dirty = false;
    }

    text_viewer::~text_viewer()
    {
        if (m_font)
            TTF_CloseFont(m_font);

        if (TTF_WasInit())
            TTF_Quit();
    }
}

#pragma once

#include <cassert>
#include <cstdint>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "i_renderable.hpp"

namespace kodo_visualize
{
    class text_viewer : public i_renderable
    {

    public:

        text_viewer(int32_t x, int32_t y);

        void set_text(const std::string& text);

        void set_color(uint8_t red, uint8_t green, uint8_t blue);

        void set_font(const std::string& font, uint32_t size);

        void render(SDL_Renderer* renderer);

        ~text_viewer();

    private:

        int32_t m_x;
        int32_t m_y;
        SDL_Color m_color;
        TTF_Font* m_font;

        std::string m_text;

    };
}

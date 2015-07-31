// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cstdint>
#include <mutex>
#include <vector>
#include <functional>

#include <SDL.h>

#include "i_renderable.hpp"

#include "state_viewer.hpp"

namespace kodo_visualize
{
    state_viewer::state_viewer(int32_t size, int32_t x, int32_t y):
        i_renderable(),
        m_size(size),
        m_x(x),
        m_y(y)
    { }

    void state_viewer::render(SDL_Renderer* renderer)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_code_state.size() == 0)
            return;

        int32_t rect_size = m_size / m_code_state.size();
        int32_t symbol_y = m_y + rect_size / 2;

        for (auto& symbol : m_code_state)
        {
            int32_t symbol_x = m_x + rect_size / 2;
            for (auto& data : symbol)
            {
                if (data != 0)
                {
                    uint8_t color = 255 - (data % 255);
                    SDL_Rect rect = {symbol_x, symbol_y, rect_size, rect_size};
                    SDL_SetRenderDrawColor(renderer, color, color, color, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }
                symbol_x += rect_size;
            }
            symbol_y += rect_size;
        }
    }

    void state_viewer::set_code_state(const code_state_type& code_state)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_code_state = code_state;
    }
}

#include <cstdint>
#include <mutex>
#include <vector>
#include <functional>

#include <SDL.h>

#include "i_renderable.hpp"

#include "state_viewer.hpp"

namespace kodo_visualize
{
    state_viewer::state_viewer(uint32_t size, uint32_t x, uint32_t y):
        i_renderable(),
        m_size(size),
        m_canvas_x(x),
        m_canvas_y(y)
    { }

    void state_viewer::render(SDL_Renderer* renderer)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_code_state.size() == 0)
            return;

        /// @todo don't call the square size diameter. don't use radius?!
        uint32_t diameter = m_size / m_code_state.size();
        uint32_t radius = diameter / 2;
        uint32_t y = m_canvas_y + radius;

        for (auto& symbol : m_code_state)
        {
            uint32_t x = m_canvas_x + radius;
            for (auto& data : symbol)
            {
                if (data != 0)
                {
                    uint8_t color = 255 - (data % 255);
                    SDL_Rect rect = {
                        (int)x,
                        (int)y,
                        (int)diameter,
                        (int)diameter
                    };
                    SDL_SetRenderDrawColor(renderer, color, color, color, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }

                x += diameter;
            }
            y += diameter;
        }
    }

    void state_viewer::set_code_state(const code_state_type& code_state)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_code_state = code_state;
    }
}
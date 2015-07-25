
#pragma once

#include <mutex>
#include <vector>
#include <functional>

#include <SDL2/SDL.h>

#include "i_renderable.hpp"

namespace kodo_visualize
{
    class state_viewer : public i_renderable
    {
    public:

        using code_state_type = std::vector<std::vector<uint32_t>>;

    public:

        state_viewer(uint32_t size, uint32_t x, uint32_t y);

        void render(SDL_Renderer* renderer);

        virtual void trace_callback(const std::string& zone,
            const std::string& message) = 0;

        template<class Coder>
        void set_callback(Coder coder)
        {
            coder->set_trace_callback(std::bind(
                &state_viewer::trace_callback,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
        }


    protected:

        void set_code_state(const code_state_type& code_state);

    private:

        uint32_t m_size;
        uint32_t m_canvas_x;
        uint32_t m_canvas_y;

        std::mutex m_lock;

        code_state_type m_code_state;
    };
}
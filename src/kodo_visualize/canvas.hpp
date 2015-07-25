#pragma once

#include <condition_variable>
#include <cstdint>
#include <thread>
#include <vector>

#include "i_renderable.hpp"

namespace kodo_visualize
{
    class canvas
    {
    public:
        canvas(uint32_t width, uint32_t height);

        void start();
        void stop();
        void add(i_renderable* renderable);

    private:

        void run();

    private:

        bool m_running;
        uint32_t m_width;
        uint32_t m_height;

        std::vector<i_renderable*> m_renderables;
        std::condition_variable m_started;
        std::thread m_thread;
    };
}
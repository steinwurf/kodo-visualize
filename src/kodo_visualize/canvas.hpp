// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <condition_variable>
#include <cstdint>
#include <thread>
#include <vector>

#include "i_renderable.hpp"

// Fix build on windows
#undef main

namespace kodo_visualize
{
    class canvas
    {
    public:
        canvas(uint32_t width, uint32_t height, double scale=1.0);

        void start();
        void stop();
        void add(i_renderable* renderable);

    private:

        void run();

    private:

        bool m_running;
        uint32_t m_width;
        uint32_t m_height;
        double m_scale;

        std::vector<i_renderable*> m_renderables;
        std::condition_variable m_started;
        std::thread m_thread;
    };
}

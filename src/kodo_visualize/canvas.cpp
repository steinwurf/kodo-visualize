#include <cstdint>
#include <thread>

#include <cassert>
#include <mutex>
#include <condition_variable>

#include <SDL2/SDL.h>

#include "i_renderable.hpp"

#include "canvas.hpp"

namespace kodo_visualize
{
    canvas::canvas(uint32_t width, uint32_t height):
        m_running(false),
        m_width(width + 1),
        m_height(height + 1)
    { }

    void canvas::start()
    {
        m_thread = std::thread(&canvas::run, this);
        std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);
        m_started.wait(lock);
        assert(m_running);
    }

    void canvas::stop()
    {
        m_running = false;
        if (m_thread.joinable())
            m_thread.join();
    }

    void canvas::add(i_renderable* renderable)
    {
        assert(!m_running);
        m_renderables.push_back(renderable);
    }

    void canvas::run()
    {
        m_running = true;
        SDL_Init(SDL_INIT_EVERYTHING);

        SDL_Window* window = SDL_CreateWindow(
            "Canvas",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            m_width,
            m_height,
            SDL_WINDOW_BORDERLESS);

        SDL_Renderer* renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED);

        m_started.notify_all();
        while (m_running)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(renderer);
            for (auto renderable : m_renderables)
            {
                renderable->render(renderer);
            }

            SDL_RenderPresent(renderer);
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}
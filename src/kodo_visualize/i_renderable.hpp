#pragma once

#include <SDL.h>

namespace kodo_visualize
{
    class i_renderable
    {
    public:
        virtual void render(SDL_Renderer* renderer) = 0;
    };
}

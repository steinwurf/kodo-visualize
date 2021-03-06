// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

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

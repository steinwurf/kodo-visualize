// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <string>

#include <SDL.h>

#include "i_renderable.hpp"

namespace kodo_visualize
{
    class to_file : public i_renderable
    {

    public:

        to_file(const std::string& directory);

        void render(SDL_Renderer* renderer);

        ~to_file();

    private:

        std::string m_directory;
        SDL_Surface* m_screenshot;
        uint32_t m_files;
    };
}

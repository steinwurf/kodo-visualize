#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

#include <SDL.h>

#include "to_file.hpp"

namespace kodo_visualize
{
    to_file::to_file(const std::string& directory):
        m_directory(directory),
        m_screenshot(NULL),
        m_files(0)
    { }

    void to_file::render(SDL_Renderer* renderer)
    {
        if (m_screenshot == NULL)
        {
            int32_t width;
            int32_t height;

            SDL_GetRendererOutputSize(renderer, &width, &height);

            m_screenshot = SDL_CreateRGBSurface(0, width, height, 32,
                0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
        }

        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
            m_screenshot->pixels, m_screenshot->pitch);

        std::stringstream file;
        file << m_directory
             << std::setw(9) << std::setfill('0') << m_files << ".bmp";
        SDL_SaveBMP(m_screenshot, file.str().c_str());
        m_files++;
    }

    to_file::~to_file()
    {
        SDL_FreeSurface(m_screenshot);
    }
}

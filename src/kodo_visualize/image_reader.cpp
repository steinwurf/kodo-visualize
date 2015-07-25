#include <string>
#include <cstdint>
#include <vector>
#include <cassert>

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include "pixel_format.hpp"

#include "image_reader.hpp"

namespace kodo_visualize
{
    image_reader::image_reader(const std::string& filename)
    {
        SDL_Surface* image = IMG_Load(filename.c_str());
        assert(image);

        m_format = {
            image->format->Rmask,
            image->format->Gmask,
            image->format->Bmask,
            image->format->Amask
        };

        m_width = image->w;
        m_height = image->h;
        m_pitch = image->pitch;
        m_size = m_height * m_pitch;
        m_data.resize(m_size);

        SDL_LockSurface(image);
        uint8_t* pixels = (uint8_t*)image->pixels;
        std::copy(pixels, pixels + m_size, m_data.data());
        SDL_UnlockSurface(image);
        SDL_FreeSurface(image);
    }

    pixel_format image_reader::format() const
    {
        return m_format;
    }

    uint32_t image_reader::width() const
    {
        return m_width;
    }

    uint32_t image_reader::height() const
    {
        return m_height;
    }

    uint32_t image_reader::pitch() const
    {
        return m_pitch;
    }

    uint32_t image_reader::size() const
    {
        return m_size;
    }

    uint8_t* image_reader::data()
    {
        return m_data.data();
    }
}

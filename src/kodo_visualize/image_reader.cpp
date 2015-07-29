#include <string>
#include <cstdint>
#include <vector>
#include <cassert>

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include "image_format.hpp"

#include "image_reader.hpp"

namespace kodo_visualize
{
    image_reader::image_reader(const std::string& filename)
    {
        SDL_Surface* image = IMG_Load(filename.c_str());
        assert(image);

        // We do not handle palettes
        m_format = image_format(
            image->format->BitsPerPixel,
            image->format->Rmask,
            image->format->Gmask,
            image->format->Bmask,
            image->format->Amask);

        if (image->format->palette)
        {
            auto palette = image->format->palette;
            for (int32_t i = 0; i < palette->ncolors; ++i)
            {
                auto color = palette->colors[i];
                m_format.m_palette.push_back({
                    color.r,
                    color.g,
                    color.b,
                    color.a
                });
            }
        }

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

    image_format image_reader::format() const
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

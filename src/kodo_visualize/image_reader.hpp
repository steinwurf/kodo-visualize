#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "pixel_format.hpp"

namespace kodo_visualize
{
    class image_reader
    {

    public:

        image_reader(const std::string& filename);

        pixel_format format() const;
        uint32_t width() const;
        uint32_t height() const;
        uint32_t pitch() const;
        uint32_t size() const;
        uint8_t* data();

    private:

        std::vector<uint8_t> m_data;
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_pitch;
        uint32_t m_size;

        pixel_format m_format;
    };
}

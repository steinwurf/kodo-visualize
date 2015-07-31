// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gtest/gtest.h>

#include <kodo_visualize/image_reader.hpp>

TEST(test_image_reader, init)
{
    kodo_visualize::image_reader image_reader("lena.jpg");
}

TEST(test_image_reader, check_stats)
{
    kodo_visualize::image_reader image_reader("lena.jpg");
    auto format = image_reader.format();

    EXPECT_EQ(24u, format.m_bits_per_pixel);
    EXPECT_EQ(0x000000FFu, format.m_r_mask);
    EXPECT_EQ(0x0000FF00u, format.m_g_mask);
    EXPECT_EQ(0x00FF0000u, format.m_b_mask);
    EXPECT_EQ(0x00000000u, format.m_a_mask);
    EXPECT_EQ(0U, format.m_palette.size());

    EXPECT_EQ(400U, image_reader.width());
    EXPECT_EQ(225U, image_reader.height());
    uint8_t bits_per_pixel = image_reader.format().m_bits_per_pixel;
    uint32_t calculated_pitch = image_reader.width() * ((bits_per_pixel / 8));

    // handle leftovers.
    if (bits_per_pixel % 8 != 0)
        calculated_pitch += 1;

    EXPECT_EQ(calculated_pitch, image_reader.pitch());
    uint32_t calculated_size = image_reader.height() * image_reader.pitch();
    EXPECT_EQ(calculated_size, image_reader.size());
    EXPECT_NE((uint8_t*)NULL, image_reader.data());
}

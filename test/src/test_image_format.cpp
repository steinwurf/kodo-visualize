// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_visualize/image_format.hpp>

#include <gtest/gtest.h>

TEST(test_image_format, init)
{
    kodo_visualize::image_format image_format;
}

TEST(test_image_format, constructor1)
{
    kodo_visualize::image_format image_format;

    EXPECT_EQ(0U, image_format.m_bits_per_pixel);
    EXPECT_EQ(0U, image_format.m_r_mask);
    EXPECT_EQ(0U, image_format.m_g_mask);
    EXPECT_EQ(0U, image_format.m_b_mask);
    EXPECT_EQ(0U, image_format.m_a_mask);
}

TEST(test_image_format, constructor2)
{

    uint8_t bits_per_pixel = 1;
    uint32_t r_mask = 2;
    uint32_t g_mask = 3;
    uint32_t b_mask = 4;
    uint32_t a_mask = 5;

    kodo_visualize::image_format image_format(
        bits_per_pixel,
        r_mask,
        g_mask,
        b_mask,
        a_mask
    );

    EXPECT_EQ(bits_per_pixel, image_format.m_bits_per_pixel);
    EXPECT_EQ(r_mask, image_format.m_r_mask);
    EXPECT_EQ(g_mask, image_format.m_g_mask);
    EXPECT_EQ(b_mask, image_format.m_b_mask);
    EXPECT_EQ(a_mask, image_format.m_a_mask);
}

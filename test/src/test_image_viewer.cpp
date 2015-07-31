// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_visualize/image_viewer.hpp>

#include <kodo_visualize/image_format.hpp>

#include <gtest/gtest.h>

TEST(test_image_viewer, init)
{
    kodo_visualize::image_format image_format;
    image_format.m_bits_per_pixel = 16;
    kodo_visualize::image_viewer image_viewer(image_format, 10, 10, 0, 0);
}

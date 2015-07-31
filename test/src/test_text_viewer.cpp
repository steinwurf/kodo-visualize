// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <gtest/gtest.h>

#include <kodo_visualize/text_viewer.hpp>

TEST(test_text_viewer, init)
{
    kodo_visualize::text_viewer text_viewer(0, 0);
}

TEST(test_text_viewer, setters)
{
    kodo_visualize::text_viewer text_viewer(0, 0);
    text_viewer.set_text("Hello World!");
    text_viewer.set_color(255, 0, 0);
}

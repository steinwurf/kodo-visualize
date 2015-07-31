// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_visualize/canvas.hpp>

#include <gtest/gtest.h>

TEST(test_canvas, init)
{
    kodo_visualize::canvas canvas(100, 100);
}


TEST(test_canvas, start_stop)
{
    kodo_visualize::canvas canvas(100, 100);

    // Check that we can call stop before calling start
    canvas.stop();

    // Check that we can call start
    canvas.start();

    // Check that we can call stop after calling start
    canvas.stop();
}

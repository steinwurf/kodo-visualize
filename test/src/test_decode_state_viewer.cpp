// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_visualize/decode_state_viewer.hpp>

#include <gtest/gtest.h>

TEST(test_decode_state_viewer, init)
{
    kodo_visualize::decode_state_viewer decode_state_viewer(10, 0, 0);
}

TEST(test_decode_state_viewer, callback)
{
    kodo_visualize::decode_state_viewer decode_state_viewer(10, 0, 0);

    decode_state_viewer.trace_callback("not decoder_state", "random data");
}

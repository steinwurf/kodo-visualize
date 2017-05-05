// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo_visualize/encode_state_viewer.hpp>

#include <gtest/gtest.h>

TEST(encode_state_viewer, init)
{
    kodo_visualize::encode_state_viewer encode_state_viewer(10, 0, 0);
}

TEST(encode_state_viewer, callback)
{
    kodo_visualize::encode_state_viewer encode_state_viewer(10, 0, 0);
    encode_state_viewer.trace_callback(
        "not symbol_index_before_write_uncoded_symbol", "random data");
    encode_state_viewer.trace_callback(
        "not coefficients_after_write_symbol", "random data");
}

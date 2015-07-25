#pragma once

#include <sstream>

#include "state_viewer.hpp"

namespace kodo_visualize
{
    class decode_state_viewer : public state_viewer
    {

    public:

        decode_state_viewer(uint32_t size, uint32_t x, uint32_t y);
        void trace_callback(const std::string& zone,
            const std::string& message);
    };
}

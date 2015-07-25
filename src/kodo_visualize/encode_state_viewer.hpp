#pragma once

#include <cstdint>

#include "state_viewer.hpp"

namespace kodo_visualize
{
    class encode_state_viewer : public state_viewer
    {
    public:

        using code_state_type = state_viewer::code_state_type;

    public:

        encode_state_viewer(uint32_t size, uint32_t x, uint32_t y);

        void trace_callback(const std::string& zone,
            const std::string& message);

        void set_symbols(uint32_t symbols);

    private:

        uint32_t m_symbols;
        uint32_t m_index;

        code_state_type m_encode_state;
    };
}
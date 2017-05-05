// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <cassert>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "encode_state_viewer.hpp"

namespace kodo_visualize
{
    encode_state_viewer::encode_state_viewer(uint32_t size, uint32_t x,
        uint32_t y):
        state_viewer(size, x, y),
        m_symbols(0),
        m_index(0)
    {
        set_symbols(size);
    }

    void encode_state_viewer::trace_callback(const std::string& zone,
        const std::string& message)
    {
        // Make sure that symbols has been set.
        assert(m_symbols > 0);
        std::vector<uint32_t> symbol;
        if (zone == std::string("symbol_index_after_write_uncoded_symbol"))
        {
            std::istringstream message_stream(message);
            std::string line;
            while (std::getline(message_stream, line, ' '));

            uint32_t index = std::stoi(line);
            symbol.resize(m_symbols);
            symbol[index] = 255;
        }
        else if(zone == std::string("symbol_coefficients_after_write_symbol"))
        {
            // Make sure that symbols has been set.
            assert(m_symbols > 0);
            std::istringstream message_stream(message.substr(3));
            std::string previous;
            std::string line;
            while (std::getline(message_stream, line, ' '))
            {
                if (!previous.empty())
                {
                    symbol.push_back(std::stoi(previous));
                }
                previous = line;
            }
        }
        else
        {
            return;
        }

        if (m_index < m_symbols)
        {
            m_encode_state[m_index] = symbol;
        }
        else
        {
            m_encode_state.erase(m_encode_state.begin());
            m_encode_state.push_back(symbol);
        }

        m_index += 1;

        state_viewer::set_code_state(m_encode_state);
    }

    void encode_state_viewer::set_symbols(uint32_t symbols)
    {
        m_symbols = symbols;
        m_encode_state.resize(symbols);
    }
}

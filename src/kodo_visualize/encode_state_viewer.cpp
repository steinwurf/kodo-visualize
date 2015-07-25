#include <vector>
#include <cstdint>
#include <string>
#include <sstream>

#include "encode_state_viewer.hpp"

namespace kodo_visualize
{
    encode_state_viewer::encode_state_viewer(uint32_t size, uint32_t x,
        uint32_t y):
        state_viewer(size, x, y),
        m_symbols(0),
        m_index(0)
    { }

    void encode_state_viewer::trace_callback(const std::string& zone,
        const std::string& message)
    {
        if (zone == std::string("set_symbols"))
        {
            std::vector<std::string> lines;
            std::istringstream message_stream(message);
            std::string line;
            while (std::getline(message_stream, line))
            {
                lines.push_back(line);
            }

            std::istringstream is(lines[lines.size() - 5]);
            std::getline(is, line, ' ');
            set_symbols(std::stoi(line) + 1);
            return;
        }

        std::vector<uint32_t> symbol;
        if (zone == std::string("symbol_index_before_write_uncoded_symbol"))
        {
            std::istringstream message_stream(message);
            std::string line;
            while (std::getline(message_stream, line, ' '));

            uint32_t index = std::stoi(line);
            symbol.resize(m_symbols);
            symbol[index] = 255;
        }
        else if(zone == std::string("coefficients_after_write_symbol"))
        {
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
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include "state_viewer.hpp"

#include "decode_state_viewer.hpp"

namespace kodo_visualize
{
    decode_state_viewer::decode_state_viewer(uint32_t size, uint32_t x,
        uint32_t y):
        state_viewer(size, x, y)
    { }

    void decode_state_viewer::trace_callback(const std::string& zone,
        const std::string& message)
    {
        if (zone != std::string("decoder_state"))
            return;

        state_viewer::code_state_type decode_state;

        std::istringstream message_stream(message);
        std::string line;
        while (std::getline(message_stream, line))
        {
            if (line.empty())
                continue;
            std::vector<uint32_t> symbol;

            std::istringstream line_stream(line);
            std::string data;
            uint32_t i = 0;
            while (std::getline(line_stream, data, ' '))
            {
                i++;
                if (i < 4)
                    continue;
                symbol.push_back(std::stoi(data));
            }
            decode_state.push_back(symbol);
        }
        state_viewer::set_code_state(decode_state);
    }
}

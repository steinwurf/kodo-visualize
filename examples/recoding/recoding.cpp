// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>
#include <thread>

#include <kodocpp/kodocpp.hpp>

#include <kodo_visualize/canvas.hpp>

#include <kodo_visualize/decode_state_viewer.hpp>
#include <kodo_visualize/encode_state_viewer.hpp>

int main()
{
    using namespace kodo_visualize;

    uint32_t symbols = 260;
    uint32_t symbol_size = 512;

    uint32_t size_x = 0;
    uint32_t size_y = 0;

    encode_state_viewer source_encoder_viewer(symbols, size_x, size_y);
    size_x += symbols;

    decode_state_viewer recoder_decode_state_viewer(symbols, size_x, size_y);
    size_x += symbols;

    encode_state_viewer recoder_encode_viewer(symbols, size_x, size_y);
    size_x += symbols;

    decode_state_viewer sink_decode_state_viewer(symbols, size_x, size_y);
    size_x += symbols;

    size_y += symbols;

    canvas canvas(size_x, size_y);

    canvas.add(&source_encoder_viewer);
    canvas.add(&recoder_decode_state_viewer);
    canvas.add(&recoder_encode_viewer);
    canvas.add(&sink_decode_state_viewer);

    canvas.start();

    // Initialization of encoder and decoder
    kodocpp::encoder_factory encoder_factory(
        kodo_full_vector,
        kodo_binary8,
        symbols,
        symbol_size,
        true);

    kodocpp::encoder source = encoder_factory.build();

    kodocpp::decoder_factory decoder_factory(
        kodo_full_vector,
        kodo_binary8,
        symbols,
        symbol_size,
        true);

    kodocpp::decoder recoder = decoder_factory.build();
    kodocpp::decoder sink = decoder_factory.build();


    source_encoder_viewer.set_callback(source);

    // We need to manually set the symbols for the recoders encoding
    // presentation.
    recoder_encode_viewer.set_symbols(symbols);

    auto cb = [&](const std::string& zone, const std::string& data)
    {
        recoder_encode_viewer.trace_callback(zone, data);
        recoder_decode_state_viewer.trace_callback(zone, data);
    };

    recoder.set_trace_callback(cb);

    sink_decode_state_viewer.set_callback(sink);

    // Allocate some storage for a "payload" the payload is what we would
    // eventually send over a network
    std::vector<uint8_t> data(source.block_size());
    std::generate(data.begin(), data.end(), rand);

    source.set_symbols(data.data(), data.size());

    std::vector<uint8_t> payload(source.payload_size());
    while (!sink.is_complete())
    {
        source.write_payload(payload.data());

        if (!(((uint32_t)(rand() % 100) + 1) < 50))
        {
            recoder.read_payload(payload.data());
        }

        recoder.write_payload(payload.data());

        if (!(((uint32_t)(rand() % 100) + 1) < 50))
        {
            sink.read_payload(payload.data());
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    canvas.stop();
}

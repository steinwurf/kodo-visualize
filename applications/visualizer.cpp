// Copyright Steinwurf ApS 2011.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <functional>
#include <vector>

#include <kodo/rlnc/full_vector_codes.hpp>
#include <sak/storage.hpp>

#include <kodo_visualize/canvas.hpp>

#include <kodo_visualize/state_viewer.hpp>
#include <kodo_visualize/decode_state_viewer.hpp>
#include <kodo_visualize/encode_state_viewer.hpp>
#include <kodo_visualize/image_reader.hpp>
#include <kodo_visualize/image_viewer.hpp>

int main()
{
    kodo_visualize::image_reader image("lena.jpg");

    uint32_t size_x = 0;
    uint32_t size_y = 0;

    kodo_visualize::encode_state_viewer encode_state_viewer(
        image.height(),
        size_x,
        size_y);

    size_x += image.height();

    kodo_visualize::image_viewer image_viewer(
        image.format(),
        image.width(),
        image.height(),
        size_x,
        size_y);

    size_x += image.width();

    kodo_visualize::decode_state_viewer decode_state_viewer(
        image.height(),
        size_x,
        size_y);

    size_x += image.height();
    size_y += image.height();

    kodo_visualize::canvas canvas(size_x, size_y);

    canvas.add(&encode_state_viewer);
    canvas.add(&image_viewer);
    canvas.add(&decode_state_viewer);

    canvas.start();

    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t symbols = image.height();
    uint32_t symbol_size = image.pitch();

    // Typdefs for the encoder/decoder type we wish to use
    using rlnc_encoder = kodo::rlnc::full_vector_encoder<
        fifi::binary8, meta::typelist<kodo::enable_trace>>;
    using rlnc_decoder = kodo::rlnc::full_vector_decoder<
        fifi::binary8, meta::typelist<kodo::enable_trace>>;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_encoder::factory encoder_factory(symbols, symbol_size);
    auto encoder = encoder_factory.build();

    rlnc_decoder::factory decoder_factory(symbols, symbol_size);
    auto decoder = decoder_factory.build();

    encode_state_viewer.set_callback(encoder);
    decode_state_viewer.set_callback(decoder);

    // Allocate some storage for a "payload" the payload is what we would
    // eventually send over a network
    std::vector<uint8_t> payload(encoder->payload_size());

    // Assign the data buffer to the encoder so that we may start
    // to produce encoded symbols from it
    encoder->set_symbols(sak::storage(image.data(), image.size()));

    while (!decoder->is_complete())
    {
        // Encode a packet into the payload buffer
        encoder->write_payload(payload.data());

        if (rand() % 2)
            continue;

        // Pass that packet to the decoder
        decoder->read_payload(payload.data());

        image_viewer.display_decoding(decoder);
        SDL_Delay(100);
    }
    SDL_Delay(1000);

    canvas.stop();
}

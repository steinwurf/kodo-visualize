// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <functional>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>

#include <kodo/rlnc/full_vector_codes.hpp>

#include <kodo_visualize/canvas.hpp>

#include <kodo_visualize/state_viewer.hpp>
#include <kodo_visualize/decode_state_viewer.hpp>
#include <kodo_visualize/encode_state_viewer.hpp>
#include <kodo_visualize/image_reader.hpp>
#include <kodo_visualize/image_viewer.hpp>
#include <kodo_visualize/text_viewer.hpp>

#include <sak/storage.hpp>

namespace {
    std::string generate_stats(uint32_t symbols, uint32_t symbol_size,
        uint32_t packets, uint32_t lost, uint32_t linear_dependent)
    {
        std::stringstream stats;
        stats << "Symbols:          " << symbols << std::endl
              << "Symbol Size:      " << symbol_size << std::endl
              << "Total Packets:    " << packets << std::endl
              << "Lost Packets:     " << lost << std::endl
              << "Linear Dependent: " << linear_dependent;
        return stats.str();
    }
}

int main(int argc, char* argv[])
{
    namespace fs = boost::filesystem;
    namespace po = boost::program_options;

    // Get path of executable
    auto executable_path = fs::system_complete(fs::path(argv[0])).parent_path();

    // Get commandline arguments
    std::string application_name = "Kodo Visualize App";
    po::options_description description(application_name);

    description.add_options()
        ("help,h", "Produce help message")
        ("image-file,i", po::value<std::string>()->required(),
            "Image file to encode/decode (required)")
        ("error-rate,e", po::value<uint8_t>(),
            "The likelyhood of packet loss in percent.")
        ("field,f", po::value<uint8_t>(),
            "The field to use.")
        ("record_dir,r", po::value<std::string>(),
            "Create recording of simulation");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, description), vm);
        if (vm.count("help"))
        {
            std::cout << description << std::endl;
            return 0;
        }
        po::notify(vm);
    }
    catch(po::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl
                << description << std::endl;
      return 1;
    }

    std::string image_file = vm["image-file"].as<std::string>();
    kodo_visualize::image_reader image(image_file);

    uint32_t size_x = 0;
    uint32_t size_y = 0;

    kodo_visualize::text_viewer text_viewer(size_x, size_y);

    std::string font_file = (executable_path / fs::path("font.ttf")).string();

    text_viewer.set_font(font_file, 12);

    // Set the number of symbols (i.e. the generation size in RLNC
    // terminology) and the size of a symbol in bytes
    uint32_t symbols = image.height();
    uint32_t symbol_size = image.pitch();
    uint32_t packets = 0;
    uint32_t lost = 0;
    uint32_t linear_dependent = 0;

    text_viewer.set_text(
        generate_stats(symbols, symbol_size, packets, lost, linear_dependent));

    size_x += text_viewer.width() + 10;

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
    canvas.add(&text_viewer);

    canvas.start();


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
        text_viewer.set_text(generate_stats(symbols, symbol_size, packets, lost,
            linear_dependent));
        // Encode a packet into the payload buffer
        encoder->write_payload(payload.data());
        packets += 1;

        if (rand() % 2)
        {
            lost += 1;
            continue;
        }

        uint32_t old_rank = decoder->rank();
        // Pass that packet to the decoder
        decoder->read_payload(payload.data());

        if (decoder->rank() == old_rank)
            linear_dependent += 1;

        image_viewer.display_decoding(decoder);
    }
    SDL_Delay(1000);

    canvas.stop();
}

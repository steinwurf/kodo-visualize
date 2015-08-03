// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <functional>
#include <vector>
#include <unordered_map>

#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>

#include <kodocpp/kodocpp.hpp>

#include <kodo_visualize/canvas.hpp>

#include <kodo_visualize/state_viewer.hpp>
#include <kodo_visualize/decode_state_viewer.hpp>
#include <kodo_visualize/encode_state_viewer.hpp>
#include <kodo_visualize/image_reader.hpp>
#include <kodo_visualize/image_viewer.hpp>
#include <kodo_visualize/text_viewer.hpp>

#include <sak/storage.hpp>

namespace {
    std::string generate_stats(uint32_t encoder_rank, uint32_t decoder_rank,
        uint32_t symbols, uint32_t symbol_size, uint32_t error_rate,
        uint32_t packets, uint32_t lost, uint32_t linear_dependent)
    {
        std::stringstream stats;
        stats << "Encoder Rank:     " << encoder_rank << std::endl
              << "Decoder Rank:     " << decoder_rank << std::endl
              << "Symbols:          " << symbols << std::endl
              << "Symbol Size:      " << symbol_size << std::endl
              << "Error Rate:       " << error_rate << "%"<< std::endl
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
        ("image-file", po::value<std::string>()->required(),
            "Image file to encode/decode (required)")
        ("error-rate", po::value<uint32_t>()->default_value(50U),
            "The likelyhood of packet loss in percent.")
        ("delay", po::value<uint32_t>()->default_value(0U),
            "Delay the coding process (symbols * ms).")
        ("non-systematic", "Use non-systematic encoding.")
        ("algorithm", po::value<std::string>()->default_value("full_rlnc"),
            "The algorithm to use.")
        ("field", po::value<std::string>()->default_value("binary8"),
            "The field to use.")
        ("data-availablity", po::value<uint32_t>()->default_value(50U),
            "The likelyhood of packet loss in percent.")
        ("record_dir", po::value<std::string>(),
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
    uint32_t error_rate = vm["error-rate"].as<uint32_t>();
    assert(error_rate < 100U);

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

    text_viewer.set_text(generate_stats(0, 0, symbols, symbol_size, error_rate,
        packets, lost, linear_dependent));

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

    std::unordered_map<std::string, kodo_finite_field> finte_field_map({
        {"binary", kodo_binary},
        {"binary4", kodo_binary4},
        {"binary8", kodo_binary8},
        {"binary16", kodo_binary16},
        {"prime2325", kodo_prime2325}
    });
    assert(finte_field_map.count(vm["field"].as<std::string>()));
    kodo_finite_field finte_field = finte_field_map[vm["field"].as<std::string>()];

    std::unordered_map<std::string, kodo_code_type> algorithm_map({
        {"full_rlnc", kodo_full_rlnc},
        {"on_the_fly", kodo_on_the_fly},
        {"sliding_window", kodo_sliding_window},
        {"seed_rlnc", kodo_seed_rlnc}
    });
    assert(algorithm_map.count(vm["algorithm"].as<std::string>()));
    kodo_code_type code_type = algorithm_map[vm["algorithm"].as<std::string>()];

    // Initilization of encoder and decoder
    kodocpp::encoder_factory encoder_factory(
        code_type,
        finte_field,
        symbols,
        symbol_size,
        true);

    kodocpp::encoder encoder = encoder_factory.build();

    kodocpp::decoder_factory decoder_factory(
        code_type,
        finte_field,
        symbols,
        symbol_size,
        true);

    kodocpp::decoder decoder = decoder_factory.build();

    encode_state_viewer.set_callback(encoder);
    decode_state_viewer.set_callback(decoder);

    // Allocate some storage for a "payload" the payload is what we would
    // eventually send over a network
    std::vector<uint8_t> payload(encoder.payload_size());

    uint32_t data_availablity = vm["data-availablity"].as<uint32_t>();
    assert(data_availablity < 100U);

    if (code_type != kodo_on_the_fly && code_type != kodo_sliding_window)
    {
        // Assign the data buffer to the encoder so that we may start
        // to produce encoded symbols from it
        encoder.set_symbols(image.data(), image.size());
    }
    else
    {
        encode_state_viewer.set_symbols(symbols);
    }

    if (vm.count("non-systematic"))
    {
        encoder.set_systematic_off();
    }

    while (!decoder.is_complete())
    {
        SDL_Delay(vm["delay"].as<uint32_t>());

        if ((code_type == kodo_on_the_fly || code_type == kodo_sliding_window))
        {
            if (encoder.rank() < encoder.symbols() &&
                ((uint32_t)(rand() % 100)) > data_availablity)
            {
                //The rank of an encoder indicates how many symbols have been
                // added, i.e how many symbols are available for encoding
                uint32_t rank = encoder.rank();

                //Calculate the offset to the next symbol to instert
                uint8_t* symbol = image.data() + (rank * encoder.symbol_size());

                encoder.set_symbol(rank, symbol, encoder.symbol_size());
            }
        }

        if (encoder.rank() == 0)
        {
            goto print_stats;
        }

        std::cout << encoder.rank() << std::endl;
        // Encode a packet into the payload buffer
        encoder.write_payload(payload.data());
        packets += 1;

        if (((uint32_t)(rand() % 101)) < error_rate)
        {
            lost += 1;
            goto print_stats;
        }
        {
            uint32_t old_rank = decoder.rank();
            // Pass that packet to the decoder

            decoder.read_payload(payload.data());

            if (decoder.rank() == old_rank)
                linear_dependent += 1;
        }

        print_stats:
        text_viewer.set_text(generate_stats(
            encoder.rank(),
            decoder.rank(),
            symbols,
            symbol_size,
            error_rate,
            packets,
            lost,
            linear_dependent));

        image_viewer.display_decoding(decoder);
    }
    SDL_Delay(1000);

    canvas.stop();
}

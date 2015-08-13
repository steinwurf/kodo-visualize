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
#include <thread>

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
#include <kodo_visualize/to_file.hpp>

namespace kodo_visualize
{
namespace
{

    struct context
    {

    public:
        context():
            algorithm(kodo_full_vector),
            field(kodo_binary),
            image_file(),
            symbols(0),
            symbol_size(0),
            non_systematic(false),
            per(0),
            feedback_per(0),
            data_availability(0),
            encoder_rank(0),
            decoder_rank(0),
            packets(0),
            feedback(0),
            lost(0),
            lost_feedback(0),
            linear_dependent(0),
            delay(0),
            disable_decoder_state(false),
            disable_encoder_state(false),
            scale(1.0),
            field_map({
                {"binary", kodo_binary},
                {"binary4", kodo_binary4},
                {"binary8", kodo_binary8},
                {"binary16", kodo_binary16},
                {"prime2325", kodo_prime2325}
            }),
            algorithm_map({
                {"full_vector", kodo_full_vector},
                {"on_the_fly", kodo_on_the_fly},
                {"sliding_window", kodo_sliding_window},
                {"seed", kodo_seed},
                {"perpetual", kodo_perpetual}
            })
        { }

        std::string to_string() const
        {
            std::stringstream stats;
            for (auto& stat_to_print : stats_to_print)
            {
                bool all = stat_to_print == "all";
                if (stat_to_print == "algorithm" || all)
                {
                    std::string algorithm_str;
                    for (auto& algorithm_pair : algorithm_map)
                    {
                        if (algorithm_pair.second == algorithm)
                        {
                            algorithm_str = algorithm_pair.first;
                            break;
                        }
                    }

                    stats << "algorithm:         " << algorithm_str << std::endl;
                }

                if (stat_to_print == "field" || all)
                {
                    std::string field_str;
                    for (auto& field_pair : field_map)
                    {
                        if (field_pair.second == field)
                        {
                            field_str = field_pair.first;
                            break;
                        }
                    }
                    stats << "field:             " << field_str << std::endl;
                }

                if (stat_to_print == "image_file" || all)
                    stats << "image_file:        " << image_file << std::endl;
                if (stat_to_print == "symbols" || all)
                    stats << "symbols:           " << symbols << std::endl;
                if (stat_to_print == "symbol_size" || all)
                    stats << "symbol_size:       " << symbol_size << std::endl;
                if (stat_to_print == "non_systematic" || all)
                    stats << "non_systematic:    " << non_systematic << std::endl;
                if (stat_to_print == "per" || all)
                    stats << "per:               " << per << std::endl;
                if (stat_to_print == "feedback_per" || all)
                    stats << "feedback_per:      " << feedback_per << std::endl;
                if (stat_to_print == "data_availability" || all)
                    stats << "data_availability: " << data_availability << std::endl;

                if (stat_to_print == "decoder_rank" || all)
                    stats << "decoder_rank:      " << decoder_rank << std::endl;
                if (stat_to_print == "encoder_rank" || all)
                    stats << "encoder_rank:      " << encoder_rank << std::endl;
                if (stat_to_print == "lost" || all)
                    stats << "lost:              " << lost << std::endl;
                if (stat_to_print == "lost_feedback" || all)
                    stats << "lost_feedback:     " << lost_feedback << std::endl;
                if (stat_to_print == "packets" || all)
                    stats << "packets:           " << packets << std::endl;
                if (stat_to_print == "feedback" || all)
                    stats << "feedback:          " << feedback << std::endl;
                if (stat_to_print == "linear_dependent" || all)
                    stats << "linear_dependent:  " << linear_dependent << std::endl;
            }

            return stats.str();
        }

    public:

        kodo_code_type algorithm;
        kodo_finite_field field;
        std::string image_file;
        uint32_t symbols;
        uint32_t symbol_size;
        bool non_systematic;

        uint32_t per;
        uint32_t feedback_per;
        uint32_t data_availability;

        uint32_t encoder_rank;
        uint32_t decoder_rank;
        uint32_t packets;
        uint32_t feedback;
        uint32_t lost;
        uint32_t lost_feedback;
        uint32_t linear_dependent;

        uint32_t delay;
        bool disable_decoder_state;
        bool disable_encoder_state;
        double scale;
        std::vector<std::string> stats_to_print;

        std::string recording_directory;
        std::string font_file;

        std::unordered_map<std::string, kodo_finite_field> field_map;
        std::unordered_map<std::string, kodo_code_type> algorithm_map;
    };

    bool parse_args(int argc, char* argv[], context& c)
    {
        // Create context
        namespace fs = boost::filesystem;
        namespace po = boost::program_options;
        // Get path of executable
        auto executable_path = fs::system_complete(fs::path(argv[0])).parent_path();
        c.font_file = (executable_path / fs::path("font.ttf")).string();

        // Get commandline arguments
        std::string application_name = "Kodo Visualize App";
        po::options_description description(application_name);

        description.add_options()
            ("help,h", "Produce help message")
            ("algorithm", po::value<std::string>()->default_value("full_vector"),
                "The algorithm to use.")
            ("field", po::value<std::string>()->default_value("binary8"),
                "The field to use.")
            ("image-file", po::value<std::string>(),
                "Image file to encode/decode (required if symbols and symbol_size "
                "is not set).")
            ("symbols", po::value<uint32_t>(),
                "Number of symbols (required if image-file hasn't been set).")
            ("symbol-size", po::value<uint32_t>(),
                "Size of each symbol (required if image-file hasn't been set).")
            ("non-systematic", "Use non-systematic encoding.")
            ("per", po::value<uint32_t>()->default_value(50U),
                "PER (Packet Error Rate) the likelihood of packet loss in percent.")
            ("feedback-per", po::value<uint32_t>()->default_value(50U),
                "The likelihood of feedback packets being lost in percent.")
            ("data-availability", po::value<uint32_t>()->default_value(100U),
                "The availability of new data in percent. Only relevant for "
                "certain algorithms. A high number means high availability.")
            ("stats-to-print", po::value<std::vector<std::string>>()->multitoken(),
                "List of statistics to be shown, use all to print all.")
            ("scale", po::value<double>()->default_value(1.0),
                "The scaling of the canvas.")
            ("delay", po::value<uint32_t>()->default_value(0U),
                "Delay the coding process (symbols * ms).")
            ("disable-encoder-state", "Don't show encoder state.")
            ("disable-decoder-state", "Don't show decoder state.")
            ("recording-directory", po::value<std::string>(),
                "Create recording of simulation (in BMP images) and store it "
                "in the given directory.");

        po::variables_map vm;
        try
        {
            po::store(po::parse_command_line(argc, argv, description), vm);
            if (vm.count("help"))
            {
                std::cout << description << std::endl;
                return false;
            }
            po::notify(vm);
        }
        catch(po::error& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl
                      << description << std::endl;
            return false;
        }

        // ALGORITHM
        if (!c.algorithm_map.count(vm["algorithm"].as<std::string>()))
        {
            std::cerr << "ERROR: \"" << vm["algorithm"].as<std::string>()
                      << "\" is not a valid algorithm." << std::endl
                      << "Use either of these valid algorithms:" << std::endl;

            for (auto& algorithm : c.algorithm_map)
            {
                std::cerr << "\t" << algorithm.first << std::endl;
            }
            return false;
        }
        c.algorithm = c.algorithm_map[vm["algorithm"].as<std::string>()];

        // FINITE FIELD
        if (!c.field_map.count(vm["field"].as<std::string>()))
        {
            std::cerr << "ERROR: \"" << vm["field"].as<std::string>()
                      << "\" is not a valid field." << std::endl
                      << "Use either of these valid fields:" << std::endl;

            for (auto& field : c.field_map)
            {
                std::cerr << "\t" << field.first << std::endl;
            }
            return false;
        }
        c.field = c.field_map[vm["field"].as<std::string>()];

        // IMAGE FILE
        if (vm.count("image-file"))
        {
            c.image_file = vm["image-file"].as<std::string>();
        }

        // SYMBOLS
        if (vm.count("symbols"))
        {
            c.symbols = vm["symbols"].as<uint32_t>();
        }

        // SYMBOL_SIZE
        if (vm.count("symbol-size"))
        {
            c.symbol_size = vm["symbol-size"].as<uint32_t>();
        }

        // NON SYSTEMATIC
        c.non_systematic = vm.count("non-systematic");

        // PER
        c.per = vm["per"].as<uint32_t>();
        if (c.per >= 100U)
        {
            std::cerr << "ERROR: PER should be a value between 0 and 99, not "
                      << c.per << std::endl;
            return false;
        }

        // FEEDBACK PER
        c.feedback_per = vm["feedback-per"].as<uint32_t>();
        if (c.feedback_per > 100U)
        {
            std::cerr << "ERROR: Feedback PER should be a value between 0 and "
                         "100, not " << c.feedback_per << std::endl;
            return false;
        }

        // DATA AVAILABILITY
        c.data_availability = vm["data-availability"].as<uint32_t>();
        if (c.data_availability == 0 || c.data_availability > 100U)
        {
            std::cerr << "ERROR: Data availability should be a value between "
                      << "1 and 100, not " << c.data_availability << std::endl;
            return false;
        }

        // STATS TO PRINT
        if (vm.count("stats-to-print"))
        {
            c.stats_to_print = vm["stats-to-print"].as<std::vector<std::string>>();
        }

        // SCALE
        c.scale = vm["scale"].as<double>();

        // DELAY
        c.delay = vm["delay"].as<uint32_t>();

        // DISABLE ENCODER STATE
        c.disable_encoder_state = vm.count("disable-encoder-state");

        // DISABLE DECODER STATE
        c.disable_decoder_state = vm.count("disable-decoder-state");

        // RECORDING DIRECTORY
        if (vm.count("recording-directory"))
        {
            c.recording_directory = vm["recording-directory"].as<std::string>();

            // Add trailing / or \ depending on platform.
            if (c.recording_directory.back() != fs::path::preferred_separator)
            {
                c.recording_directory += fs::path::preferred_separator;
            }

            if (!fs::exists(fs::path(c.recording_directory)))
            {
                std::cerr << "ERROR: The given recoding path, \""
                          << c.recording_directory << "\" does not exists." << std::endl;
                return false;
            }
        }

        // Verify that we have something to encode.
        if (c.image_file.empty())
        {
            if (c.symbols == 0 || c.symbol_size == 0)
            {
                std::cerr << "ERROR: If image-file hasn't been set, both "
                             "symbols and symbol-size needs to be set."
                          << std::endl;
                return false;
            }
        }
        else if (c.symbols != 0 || c.symbol_size != 0)
        {
            std::cerr << "ERROR: If image-file has been set, neither symbols "
                         "or symbol-size should to be set." << std::endl;
            return false;
        }

        return true;
    }

    void run_visualization(context& c)
    {
        uint32_t size_x = 0;
        uint32_t size_y = 0;

        std::shared_ptr<text_viewer> stats_viewer;

        if (c.stats_to_print.size() > 0)
        {
            stats_viewer = std::make_shared<text_viewer>(size_x,
                size_y);

            stats_viewer->set_font(c.font_file, 12);
            stats_viewer->set_text(c.to_string());
            size_x += stats_viewer->width() + 10;
        }

        // Set up visualization
        std::shared_ptr<image_reader> image;
        if (!c.image_file.empty())
        {
            assert(c.symbols == 0);
            assert(c.symbol_size == 0);

            image = std::make_shared<image_reader>(
                c.image_file);
            c.symbols = image->height();
            c.symbol_size = image->pitch();
        }

        assert(c.symbols != 0);
        assert(c.symbol_size != 0);

        std::shared_ptr<encode_state_viewer> encoder_viewer;
        if (!c.disable_encoder_state)
        {
            encoder_viewer = std::make_shared<encode_state_viewer>(
                c.symbols,
                size_x,
                size_y);
            size_x += c.symbols;
        }

        std::shared_ptr<image_viewer> decoded_image_viewer;
        if (!c.image_file.empty())
        {
            decoded_image_viewer = std::make_shared<image_viewer>(
                image->format(),
                image->width(),
                image->height(),
                size_x,
                size_y);
            size_x += image->width();
        }

        std::shared_ptr<decode_state_viewer> decoder_viewer;
        if (!c.disable_decoder_state)
        {
            decoder_viewer = std::make_shared<decode_state_viewer>(
                c.symbols, size_x, size_y);
            size_x += c.symbols;
        }


        size_y += c.symbols;

        canvas canvas(size_x, size_y, c.scale);
        if (!c.disable_encoder_state)
        {
            canvas.add(encoder_viewer.get());
        }
        if (!c.image_file.empty())
        {
            canvas.add(decoded_image_viewer.get());
        }
        if (!c.disable_decoder_state)
        {
            canvas.add(decoder_viewer.get());
        }

        if (c.stats_to_print.size() > 0)
        {
            canvas.add(stats_viewer.get());
        }

        // Must be added last
        std::shared_ptr<to_file> simulation_to_file;
        if (!c.recording_directory.empty())
        {
            simulation_to_file = std::make_shared<to_file>(
                c.recording_directory);
            canvas.add(simulation_to_file.get());
        }

        canvas.start();

        // Initialization of encoder and decoder
        kodocpp::encoder_factory encoder_factory(
            c.algorithm,
            c.field,
            c.symbols,
            c.symbol_size,
            true);

        kodocpp::encoder encoder = encoder_factory.build();

        kodocpp::decoder_factory decoder_factory(
            c.algorithm,
            c.field,
            c.symbols,
            c.symbol_size,
            true);

        kodocpp::decoder decoder = decoder_factory.build();
        if (!c.disable_encoder_state)
        {
            encoder_viewer->set_callback(encoder);
        }
        if (!c.disable_decoder_state)
        {
            decoder_viewer->set_callback(decoder);
        }

        // Allocate some storage for a "payload" the payload is what we would
        // eventually send over a network
        std::vector<uint8_t> payload(encoder.payload_size());

        std::vector<uint8_t> data(encoder.block_size());
        std::generate(data.begin(), data.end(), rand);

        uint8_t* data_ptr = data.data();
        uint32_t data_size = data.size();

        if (!c.image_file.empty())
        {
            data_ptr = image->data();
            data_size = image->size();
        }

        if (c.algorithm != kodo_on_the_fly &&
            c.algorithm != kodo_sliding_window)
        {
            // Assign the data buffer to the encoder so that we may start
            // to produce encoded symbols from it
            encoder.set_symbols(data_ptr, data_size);
        }
        else
        {
            if (!c.disable_encoder_state)
            {
                encoder_viewer->set_symbols(c.symbols);
            }
        }

        if (c.non_systematic)
        {
            if (encoder.has_set_systematic_off())
            {
                encoder.set_systematic_off();
            }
        }

        while (!decoder.is_complete())
        {
            if (c.delay != 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(c.delay));
            }

            if (c.algorithm == kodo_on_the_fly ||
                c.algorithm == kodo_sliding_window)
            {
                if (encoder.rank() < encoder.symbols() &&
                    ((uint32_t)(rand() % 100) + 1) < c.data_availability)
                {
                    //The rank of an encoder indicates how many symbols have been
                    // added, i.e how many symbols are available for encoding
                    uint32_t rank = encoder.rank();

                    //Calculate the offset to the next symbol to insert
                    uint8_t* symbol = data_ptr + (rank * encoder.symbol_size());

                    encoder.set_symbol(rank, symbol, encoder.symbol_size());
                }
            }

            if (encoder.rank() == 0)
            {
                goto print_stats;
            }

            // Encode a packet into the payload buffer
            encoder.write_payload(payload.data());
            c.packets += 1;

            if (((uint32_t)(rand() % 100) + 1) < c.per)
            {
                c.lost += 1;
                goto decoder_feedback;
            }
            {
                uint32_t old_rank = decoder.rank();
                // Pass that packet to the decoder

                decoder.read_payload(payload.data());

                if (decoder.rank() == old_rank)
                    c.linear_dependent += 1;
            }

            decoder_feedback:

            if (c.algorithm == kodo_sliding_window)
            {
                std::vector<uint8_t> feedback(encoder.feedback_size());
                decoder.write_feedback(feedback.data());
                c.feedback += 1;

                if (((uint32_t)(rand() % 100) + 1) < c.feedback_per)
                {
                    c.lost_feedback += 1;
                    goto print_stats;
                }
                encoder.read_feedback(feedback.data());
            }

            print_stats:

            c.encoder_rank = encoder.rank();
            c.decoder_rank = decoder.rank();

            if (c.stats_to_print.size() > 0)
            {
                stats_viewer->set_text(c.to_string());
            }

            if (!c.image_file.empty())
            {
                decoded_image_viewer->display_decoding(decoder);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));

        canvas.stop();
    }
}
}

int main(int argc, char* argv[])
{
    kodo_visualize::context c;
    if (kodo_visualize::parse_args(argc, argv, c))
        kodo_visualize::run_visualization(c);
}

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
            m_codec(kodocpp::codec::full_vector),
            m_field(kodocpp::field::binary),
            m_symbols(0),
            m_symbol_size(0),
            m_non_systematic(false),
            m_per(0),
            m_feedback_per(0),
            m_data_availability(0),
            m_encoder_rank(0),
            m_decoder_rank(0),
            m_packets(0),
            m_feedback(0),
            m_lost(0),
            m_lost_feedback(0),
            m_linear_dependent(0),
            m_delay(0),
            m_disable_decoder_state(false),
            m_disable_encoder_state(false),
            m_scale(1.0),
            m_field_map({
                {"binary", kodocpp::field::binary},
                {"binary4", kodocpp::field::binary4},
                {"binary8", kodocpp::field::binary8}
            }),
            m_codec_map({
                {"full_vector", kodocpp::codec::full_vector},
                {"on_the_fly", kodocpp::codec::on_the_fly},
                {"sliding_window", kodocpp::codec::sliding_window},
                {"seed", kodocpp::codec::seed},
                {"perpetual", kodocpp::codec::perpetual}
            })
        { }

        std::string to_string() const
        {
            std::stringstream stats;
            for (auto& m_stat_to_print : m_stats_to_print)
            {
                bool all = m_stat_to_print == "all";
                if (m_stat_to_print == "codec" || all)
                {
                    std::string codec_str;
                    for (auto& codec_pair : m_codec_map)
                    {
                        if (codec_pair.second == m_codec)
                        {
                            codec_str = codec_pair.first;
                            break;
                        }
                    }

                    stats << "codec:             " << codec_str
                          << std::endl;
                }

                if (m_stat_to_print == "field" || all)
                {
                    std::string field_str;
                    for (auto& field_pair : m_field_map)
                    {
                        if (field_pair.second == m_field)
                        {
                            field_str = field_pair.first;
                            break;
                        }
                    }
                    stats << "field:             " << field_str << std::endl;
                }

                if (m_stat_to_print == "image_file" || all)
                {
                    stats << "image_file:        " << m_image_file << std::endl;
                }
                if (m_stat_to_print == "symbols" || all)
                {
                    stats << "symbols:           " << m_symbols << std::endl;
                }
                if (m_stat_to_print == "symbol_size" || all)
                {
                    stats << "symbol_size:       " << m_symbol_size << std::endl;
                }
                if (m_stat_to_print == "non_systematic" || all)
                {
                    stats << "non_systematic:    " << m_non_systematic
                          << std::endl;
                }
                if (m_stat_to_print == "per" || all)
                {
                    stats << "per:               " << m_per << std::endl;
                }
                if (m_stat_to_print == "feedback_per" || all)
                {
                    stats << "feedback_per:      " << m_feedback_per << std::endl;
                }
                if (m_stat_to_print == "data_availability" || all)
                {
                    stats << "data_availability: " << m_data_availability
                          << std::endl;
                }

                if (m_stat_to_print == "decoder_rank" || all)
                {
                    stats << "decoder_rank:      " << m_decoder_rank << std::endl;
                }
                if (m_stat_to_print == "encoder_rank" || all)
                {
                    stats << "encoder_rank:      " << m_encoder_rank << std::endl;
                }
                if (m_stat_to_print == "packets" || all)
                {
                    stats << "packets:           " << m_packets << std::endl;
                }
                if (m_stat_to_print == "feedback" || all)
                {
                    stats << "feedback:          " << m_feedback << std::endl;
                }
                if (m_stat_to_print == "lost" || all)
                {
                    stats << "lost:              " << m_lost << std::endl;
                }
                if (m_stat_to_print == "lost_feedback" || all)
                {
                    stats << "lost_feedback:     " << m_lost_feedback
                          << std::endl;
                }
                if (m_stat_to_print == "linear_dependent" || all)
                {
                    stats << "linear_dependent:  " << m_linear_dependent
                          << std::endl;
                }
            }

            return stats.str();
        }

    public:

        kodocpp::codec m_codec;
        kodocpp::field m_field;
        std::string m_image_file;
        uint32_t m_symbols;
        uint32_t m_symbol_size;
        bool m_non_systematic;

        uint32_t m_per;
        uint32_t m_feedback_per;
        uint32_t m_data_availability;

        uint32_t m_encoder_rank;
        uint32_t m_decoder_rank;
        uint32_t m_packets;
        uint32_t m_feedback;
        uint32_t m_lost;
        uint32_t m_lost_feedback;
        uint32_t m_linear_dependent;

        uint32_t m_delay;
        bool m_disable_decoder_state;
        bool m_disable_encoder_state;
        double m_scale;
        std::vector<std::string> m_stats_to_print;

        std::string m_recording_directory;
        std::string m_font_file;

        std::unordered_map<std::string, kodocpp::field> m_field_map;
        std::unordered_map<std::string, kodocpp::codec> m_codec_map;
    };

    bool parse_args(int argc, char* argv[], context& c)
    {
        // Create context
        namespace fs = boost::filesystem;
        namespace po = boost::program_options;
        // Get path of executable
        auto executable_path =
            fs::system_complete(fs::path(argv[0])).parent_path();
        c.m_font_file = (executable_path / fs::path("font.ttf")).string();

        // Get commandline arguments
        std::string application_name = "Kodo Visualize App";
        po::options_description description(application_name);

        description.add_options()
            ("help,h", "Produce help message")
            ("codec",
                po::value<std::string>()->default_value("full_vector"),
                "The codec to use.")
            ("field", po::value<std::string>()->default_value("binary8"),
                "The field to use.")
            ("image-file", po::value<std::string>(),
                "Image file to encode/decode (required if symbols and "
                "symbol_size is not set).")
            ("symbols", po::value<uint32_t>(),
                "Number of symbols (required if image-file hasn't been set).")
            ("symbol-size", po::value<uint32_t>(),
                "Size of each symbol (required if image-file hasn't been set).")
            ("non-systematic", "Use non-systematic encoding.")
            ("per", po::value<uint32_t>()->default_value(50U),
                "PER (Packet Error Rate) the likelihood of packet loss in "
                "percent.")
            ("feedback-per", po::value<uint32_t>()->default_value(50U),
                "The likelihood of feedback packets being lost in percent.")
            ("data-availability", po::value<uint32_t>()->default_value(100U),
                "The availability of new data in percent. Only relevant for "
                "certain codecs. A high number means high availability.")
            ("stats-to-print",
                po::value<std::vector<std::string>>()->multitoken(),
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

        // codec
        if (!c.m_codec_map.count(vm["codec"].as<std::string>()))
        {
            std::cerr << "ERROR: \"" << vm["codec"].as<std::string>()
                      << "\" is not a valid codec." << std::endl
                      << "Use either of these valid codecs:" << std::endl;

            for (auto& codec : c.m_codec_map)
            {
                std::cerr << "\t" << codec.first << std::endl;
            }
            return false;
        }
        c.m_codec = c.m_codec_map[vm["codec"].as<std::string>()];

        // FINITE FIELD
        if (!c.m_field_map.count(vm["field"].as<std::string>()))
        {
            std::cerr << "ERROR: \"" << vm["field"].as<std::string>()
                      << "\" is not a valid field." << std::endl
                      << "Use either of these valid fields:" << std::endl;

            for (auto& field : c.m_field_map)
            {
                std::cerr << "\t" << field.first << std::endl;
            }
            return false;
        }
        c.m_field = c.m_field_map[vm["field"].as<std::string>()];

        // IMAGE FILE
        if (vm.count("image-file"))
        {
            c.m_image_file = vm["image-file"].as<std::string>();
        }

        // SYMBOLS
        if (vm.count("symbols"))
        {
            c.m_symbols = vm["symbols"].as<uint32_t>();
        }

        // SYMBOL_SIZE
        if (vm.count("symbol-size"))
        {
            c.m_symbol_size = vm["symbol-size"].as<uint32_t>();
        }

        // NON SYSTEMATIC
        c.m_non_systematic = vm.count("non-systematic");

        // PER
        c.m_per = vm["per"].as<uint32_t>();
        if (c.m_per >= 100U)
        {
            std::cerr << "ERROR: PER should be a value between 0 and 99, not "
                      << c.m_per << std::endl;
            return false;
        }

        // FEEDBACK PER
        c.m_feedback_per = vm["feedback-per"].as<uint32_t>();
        if (c.m_feedback_per > 100U)
        {
            std::cerr << "ERROR: Feedback PER should be a value between 0 and "
                         "100, not " << c.m_feedback_per << std::endl;
            return false;
        }

        // DATA AVAILABILITY
        c.m_data_availability = vm["data-availability"].as<uint32_t>();
        if (c.m_data_availability == 0 || c.m_data_availability > 100U)
        {
            std::cerr << "ERROR: Data availability should be a value between "
                      << "1 and 100, not " << c.m_data_availability << std::endl;
            return false;
        }

        // STATS TO PRINT
        if (vm.count("stats-to-print"))
        {
            c.m_stats_to_print =
                vm["stats-to-print"].as<std::vector<std::string>>();
        }

        // SCALE
        c.m_scale = vm["scale"].as<double>();

        // DELAY
        c.m_delay = vm["delay"].as<uint32_t>();

        // DISABLE ENCODER STATE
        c.m_disable_encoder_state = vm.count("disable-encoder-state");

        // DISABLE DECODER STATE
        c.m_disable_decoder_state = vm.count("disable-decoder-state");

        // RECORDING DIRECTORY
        if (vm.count("recording-directory"))
        {
            c.m_recording_directory = vm["recording-directory"].as<std::string>();

            // Add trailing / or \ depending on platform.
            if (c.m_recording_directory.back() != fs::path::preferred_separator)
            {
                c.m_recording_directory += fs::path::preferred_separator;
            }

            if (!fs::exists(fs::path(c.m_recording_directory)))
            {
                std::cerr << "ERROR: The given recoding path, \""
                          << c.m_recording_directory << "\" does not exists."
                          << std::endl;
                return false;
            }
        }

        // Verify that we have something to encode.
        if (c.m_image_file.empty())
        {
            if (c.m_symbols == 0 || c.m_symbol_size == 0)
            {
                std::cerr << "ERROR: If image-file hasn't been set, both "
                             "symbols and symbol-size needs to be set."
                          << std::endl;
                return false;
            }
        }
        else if (c.m_symbols != 0 || c.m_symbol_size != 0)
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

        if (c.m_stats_to_print.size() > 0)
        {
            stats_viewer = std::make_shared<text_viewer>(size_x, size_y);

            stats_viewer->set_font(c.m_font_file, 12);
            stats_viewer->set_text(c.to_string());
            size_x += stats_viewer->width() + 10;
        }

        // Set up visualization
        std::shared_ptr<image_reader> image;
        if (!c.m_image_file.empty())
        {
            assert(c.m_symbols == 0);
            assert(c.m_symbol_size == 0);

            image = std::make_shared<image_reader>(c.m_image_file);
            c.m_symbols = image->height();
            c.m_symbol_size = image->pitch();
        }

        assert(c.m_symbols != 0);
        assert(c.m_symbol_size != 0);

        std::shared_ptr<encode_state_viewer> encoder_viewer;
        if (!c.m_disable_encoder_state)
        {
            encoder_viewer = std::make_shared<encode_state_viewer>(
                c.m_symbols,
                size_x,
                size_y);
            size_x += c.m_symbols;
        }

        std::shared_ptr<image_viewer> decoded_image_viewer;
        if (!c.m_image_file.empty())
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
        if (!c.m_disable_decoder_state)
        {
            decoder_viewer = std::make_shared<decode_state_viewer>(
                c.m_symbols, size_x, size_y);
            size_x += c.m_symbols;
        }


        size_y += c.m_symbols;

        canvas canvas(size_x, size_y, c.m_scale);
        if (!c.m_disable_encoder_state)
        {
            canvas.add(encoder_viewer.get());
        }
        if (!c.m_image_file.empty())
        {
            canvas.add(decoded_image_viewer.get());
        }
        if (!c.m_disable_decoder_state)
        {
            canvas.add(decoder_viewer.get());
        }

        if (c.m_stats_to_print.size() > 0)
        {
            canvas.add(stats_viewer.get());
        }

        // Must be added last
        std::shared_ptr<to_file> simulation_to_file;
        if (!c.m_recording_directory.empty())
        {
            simulation_to_file = std::make_shared<to_file>(
                c.m_recording_directory);
            canvas.add(simulation_to_file.get());
        }

        canvas.start();

        // Initialization of encoder and decoder
        kodocpp::encoder_factory encoder_factory(
            c.m_codec,
            c.m_field,
            c.m_symbols,
            c.m_symbol_size);

        kodocpp::encoder encoder = encoder_factory.build();

        kodocpp::decoder_factory decoder_factory(
            c.m_codec,
            c.m_field,
            c.m_symbols,
            c.m_symbol_size);

        kodocpp::decoder decoder = decoder_factory.build();
        if (!c.m_disable_encoder_state)
        {
            encoder_viewer->set_callback(encoder);
        }
        if (!c.m_disable_decoder_state)
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

        if (!c.m_image_file.empty())
        {
            data_ptr = image->data();
            data_size = image->size();
        }

        if (c.m_codec != kodocpp::codec::on_the_fly &&
            c.m_codec != kodocpp::codec::sliding_window)
        {
            // Assign the data buffer to the encoder so that we may start
            // to produce encoded symbols from it
            encoder.set_const_symbols(data_ptr, data_size);
        }
        else
        {
            if (!c.m_disable_encoder_state)
            {
                encoder_viewer->set_symbols(c.m_symbols);
            }
        }

        if (c.m_non_systematic)
        {
            if (encoder.has_systematic_interface())
            {
                encoder.set_systematic_off();
            }
        }

        std::vector<uint8_t> data_out(decoder.block_size());
        decoder.set_mutable_symbols(data_out.data(), data_out.size());

        while (!decoder.is_complete())
        {
            if (c.m_delay != 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(c.m_delay));
            }

            if (c.m_codec == kodocpp::codec::on_the_fly ||
                c.m_codec == kodocpp::codec::sliding_window)
            {
                if (encoder.rank() < encoder.symbols() &&
                    ((uint32_t)(rand() % 100) + 1) < c.m_data_availability)
                {
                    //The rank of an encoder indicates how many symbols have
                    // been added, i.e how many symbols are available for
                    // encoding
                    uint32_t rank = encoder.rank();

                    //Calculate the offset to the next symbol to insert
                    uint8_t* symbol = data_ptr + (rank * encoder.symbol_size());

                    encoder.set_const_symbol(rank, symbol, encoder.symbol_size());
                }
            }

            if (encoder.rank() == 0)
            {
                goto print_stats;
            }

            // Encode a packet into the payload buffer
            encoder.write_payload(payload.data());
            c.m_packets += 1;

            if (((uint32_t)(rand() % 100) + 1) < c.m_per)
            {
                c.m_lost += 1;
                goto decoder_feedback;
            }
            {
                uint32_t old_rank = decoder.rank();
                // Pass that packet to the decoder

                decoder.read_payload(payload.data());

                if (decoder.rank() == old_rank)
                    c.m_linear_dependent += 1;
            }

            decoder_feedback:

            if (c.m_codec == kodocpp::codec::sliding_window)
            {
                std::vector<uint8_t> feedback(encoder.feedback_size());
                decoder.write_feedback(feedback.data());
                c.m_feedback += 1;

                if (((uint32_t)(rand() % 100) + 1) < c.m_feedback_per)
                {
                    c.m_lost_feedback += 1;
                    goto print_stats;
                }
                encoder.read_feedback(feedback.data());
            }

            print_stats:

            c.m_encoder_rank = encoder.rank();
            c.m_decoder_rank = decoder.rank();

            if (c.m_stats_to_print.size() > 0)
            {
                stats_viewer->set_text(c.to_string());
            }

            if (!c.m_image_file.empty())
            {
                decoded_image_viewer->display_decoding(data_out.data());
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

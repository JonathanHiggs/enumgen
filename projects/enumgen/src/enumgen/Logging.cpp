#include <enumgen/Logging.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>


namespace enumgen
{

    void initLogging(std::filesystem::path const & outputDirectory)
    {
        // Output logger
        auto outputFile = outputDirectory / "enumgen.output.log";
        auto outputSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(outputFile.string(), true);
        outputSink->set_level(spdlog::level::info);
        outputSink->set_pattern("[%l] %v");

        auto errorFile = outputDirectory / "enumgen.error.log";
        auto errorSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(errorFile.string(), true);
        errorSink->set_level(spdlog::level::trace);
        errorSink->set_pattern("[%l] %v");

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_sink_st>();
        consoleSink->set_level(spdlog::level::warn);
        consoleSink->set_pattern("[enumgen][%l] %v");

        auto logger = std::make_shared<spdlog::logger>(
            "logger",
            std::initializer_list<spdlog::sink_ptr>{ std::move(outputSink),
                                                     std::move(errorSink),
                                                     std::move(consoleSink) });

        logger->set_level(spdlog::level::trace);
        logger->trace("Initialized logging");

        spdlog::register_logger(std::move(logger));
    }

}  // namespace enumgen
#include <enumgen/Logging.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace enumgen
{

    std::shared_ptr<spdlog::logger> & logger() noexcept
    {
        static std::shared_ptr<spdlog::logger> logger = nullptr;
        return logger;
    }

    void initLogging(std::filesystem::path const & outputDirectory)
    {
        // Output logger
        auto outputFile = outputDirectory / "enumgen.output.log";
        auto outputSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(outputFile.string());
        outputSink->set_level(spdlog::level::info);
        outputSink->set_pattern("[%p] %v");

        auto errorFile = outputDirectory / "enumgen.error.log";
        auto errorSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(errorFile.string());
        outputSink->set_level(spdlog::level::trace);
        outputSink->set_pattern("[%p][%L] %v");

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_sink_st>();
        consoleSink->set_level(spdlog::level::warn);
        consoleSink->set_pattern("[enumgen][%p] %v");

        auto logger = std::make_shared<spdlog::logger>(
            "out",
            std::initializer_list<spdlog::sink_ptr>{ std::move(outputSink), std::move(errorSink), std::move(consoleSink) });

        enumgen::logger() = std::move(logger);
    }

}  // namespace enumgen
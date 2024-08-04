#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/bin_to_hex.h>

class Logger
{
	public:
		Logger();
		~Logger();

		spdlog::logger* Get();

	private:
		std::shared_ptr<spdlog::logger> _logger;
};
#include "logger.h"

Logger::Logger()
{
	try
	{
		_logger = spdlog::basic_logger_st("CustomLocalizations", "../CustomLocalizations.log");
	}
	catch (const spdlog::spdlog_ex &exception)
	{
		std::exit(-1);
	}
}

spdlog::logger* Logger::Get()
{
	return _logger.get();
}

Logger::~Logger()
{
}

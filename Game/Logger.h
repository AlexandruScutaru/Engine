#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "spdlog/spdlog.h"


#define GAME_FILE_LOGGER_NAME "game_logger"

namespace utilities{

	class Log{
	public:
		static Log& Instance();

		Log(Log const&) = delete;
		void operator=(Log const&) = delete;

		std::shared_ptr<spdlog::logger> rotFileLogger;
	private:
		Log();
		void initRotatingFileLogger(const std::string& location, int max_size, int number);

	};

}

#define TRACE(...) SPDLOG_TRACE(utilities::Log::Instance().rotFileLogger, __VA_ARGS__)
#define LOG_INFO(...) utilities::Log::Instance().rotFileLogger->info(__VA_ARGS__)
#define LOG_WARN(...) utilities::Log::Instance().rotFileLogger->warn(__VA_ARGS__)
#define LOG_ERROR(...) utilities::Log::Instance().rotFileLogger->error(__VA_ARGS__)

#define LOG_ERROR_TRACEABLE(...) TRACE(); utilities::Log::Instance().rotFileLogger->error(__VA_ARGS__)

#endif // !LOGGER_H

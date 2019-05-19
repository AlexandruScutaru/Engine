#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "spdlog/spdlog.h"


#define ENGINE_FILE_LOGGER_NAME "engine_logger"

namespace utilities{

	class Log_e{
	public:
		static Log_e& Instance();

		Log_e(Log_e const&) = delete;
		void operator=(Log_e const&) = delete;

		std::shared_ptr<spdlog::logger> rotFileLogger;
	private:
		Log_e();
		void initRotatingFileLogger(const std::string& location, int max_size, int number);

	};

}

#define TRACE_E(...) SPDLOG_TRACE(utilities::Log_e::Instance().rotFileLogger, __VA_ARGS__)
#define LOG_E_INFO(...) utilities::Log_e::Instance().rotFileLogger->info(__VA_ARGS__)
#define LOG_E_WARN(...) utilities::Log_e::Instance().rotFileLogger->warn(__VA_ARGS__)
#define LOG_E_ERROR(...) utilities::Log_e::Instance().rotFileLogger->error(__VA_ARGS__)

#define LOG_E_ERROR_TRACEABLE(...) TRACE_E(); utilities::Log_e::Instance().rotFileLogger->error(__VA_ARGS__)

#endif // !LOGGER_H

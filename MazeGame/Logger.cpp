#include "Logger.h"
#include "Config.h"

#include <spdlog/sinks/rotating_file_sink.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

namespace utilities{

	Log::Log(){
		initRotatingFileLogger(CONFIG.logger_cfg.file_name, CONFIG.logger_cfg.size, CONFIG.logger_cfg.number_of_files);
		rotFileLogger->set_level((spdlog::level::level_enum)CONFIG.logger_cfg.level);
		rotFileLogger->info("========================================================================");
		rotFileLogger->info("Log::rotFileLogger constructed");
	}

	Log & Log::Instance(){
		static Log instance;
		return instance;
	}
	
	void Log::initRotatingFileLogger(const std::string& filename, int max_size, int number){
		std::fstream file;
		file.exceptions(std::ofstream::badbit);
		try{
			//in case file doesn't exist
			file.open(filename, std::fstream::app);
			file.close();

			rotFileLogger = spdlog::rotating_logger_st(GAME_FILE_LOGGER_NAME, filename, max_size, number);
		} catch(const spdlog::spdlog_ex& ex){
			std::cout << "logger init failed: " << ex.what() << std::endl;
			exit(EXIT_FAILURE);
		} catch(std::ifstream::failure e){
			std::cout << "error opening/creating '" << filename << "'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

}

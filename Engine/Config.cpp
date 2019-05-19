#include "Config.h"
#include "Logger.h"

#include <string>
#include <fstream>
#include <unordered_map>

#include <JSON/json.hpp>
using json = nlohmann::json;

#define CONFIG_GILE "config_engine.json"

namespace utilities{

	Config_e::Config_e(){
		std::ifstream file(CONFIG_GILE);
		if(file.fail()){
			std::cout << "Couldn't open confing file: '" << CONFIG_GILE << "'! Setting default values!" << std::endl;
			SetDefaults();
		} else{
			json cfg;
			file >> cfg;
			file.close();

			std::unordered_map<std::string, unsigned int> log_levels = {{"trace", 0}, {"debug", 1}, {"info", 2}, {"warn", 3}, {"err", 4}, {"critical", 5}, {"off", 6}};
			try{
				logger_cfg.file_name = cfg["logger"]["log_file"].get<std::string>();
			} catch(...){
				std::cout << "logger.log_file not found" << std::endl;
				logger_cfg.file_name = "logs/engine_log.txt";
			}
			try{
				logger_cfg.level = log_levels[cfg["logger"]["level"].get<std::string>()];
			} catch(...){
				std::cout << "logger.level not found" << std::endl;
				logger_cfg.level = spdlog::level::trace;
			}
			try{
				logger_cfg.size = cfg["logger"]["log_file_size"].get<int>();
			} catch(std::exception& ex){
				std::cout << ex.what() << std::endl;
				std::cout << "logger.log_file_size not found" << std::endl;
				logger_cfg.size = 1024 * 1024 * 5;
			}
			try{
				logger_cfg.number_of_files = cfg["logger"]["no_log_files"].get<int>();
			} catch(...){
				std::cout << "logger.no_log_files not found" << std::endl;
				logger_cfg.number_of_files = 1;
			}
		}
	}

	Config_e& Config_e::Instance(){
		static Config_e instance;
		return instance;
	}

	void Config_e::SetDefaults(){
		logger_cfg.file_name = "logs/engine_log.txt";
		logger_cfg.size = 1024 * 1024 * 5;
		logger_cfg.number_of_files = 1;
		logger_cfg.level = spdlog::level::trace;
	}

}
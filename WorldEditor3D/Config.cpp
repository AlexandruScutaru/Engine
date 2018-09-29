#include "Config.h"
#include "Logger.h"
#include <Engine/Window.h>

#include <string>
#include <fstream>
#include <unordered_map>

#include <JSON/json.hpp>
using json = nlohmann::json;

#define CONFIG_GILE "config_editor.json"

namespace utilities{

	Config::Config(){
		std::ifstream file(CONFIG_GILE);
		if(file.fail()){
			std::cout << "Couldn't open confing file: '" << CONFIG_GILE << "'! Setting default values!" << std::endl;
			SetDefaults();
		} else{
			json cfg;
			file >> cfg;
			file.close();

			//logger
			std::unordered_map<std::string, unsigned int> log_levels = {{"trace", 0}, {"debug", 1}, {"info", 2}, {"warn", 3}, {"err", 4}, {"critical", 5}, {"off", 6}};
			try{
				logger_cfg.file_name = cfg["logger"]["log_file"].get<std::string>();
			} catch(...){
				std::cout << "logger.log_file not found" << std::endl;
				logger_cfg.file_name = "logs/editor_log.txt";
			}
			try{
				logger_cfg.level = log_levels[cfg["logger"]["level"].get<std::string>()];
			} catch(...){
				std::cout << "logger.level not found" << std::endl;
				logger_cfg.level = spdlog::level::trace;
			}
			try{
				logger_cfg.size = cfg["logger"]["log_file_size"].get<int>();
			} catch(...){
				std::cout << "logger.log_file_size not found" << std::endl;
				logger_cfg.size = 1024 * 1024 * 5;
			}
			try{
				logger_cfg.number_of_files = cfg["logger"]["no_log_files"].get<int>();
			} catch(...){
				std::cout << "logger.no_log_files not found" << std::endl;
				logger_cfg.number_of_files = 1;
			}

			//window
			try{
				window_cfg.title = cfg["window"]["title"].get<std::string>();
			} catch(...){
				std::cout << "window.title not found" << std::endl;
				window_cfg.title = "Map Editor";
			}
			try{
				window_cfg.width = cfg["window"]["width"].get<int>();
			} catch(...){
				std::cout << "window.width not found" << std::endl;
				window_cfg.width = 900;
			}
			try{
				window_cfg.height = cfg["window"]["height"].get<int>();
			} catch(...){
				std::cout << "window.height not found" << std::endl;
				window_cfg.height = 540;
			}
			try{
				window_cfg.trap_mouse = cfg["window"]["trap_mouse"].get<bool>();
			} catch(...){
				std::cout << "window.trap_mouse not found" << std::endl;
				window_cfg.trap_mouse = false;
			}
			try{
				window_cfg.window_mode = cfg["window"]["window_mode"].get<int>();
			} catch(...){
				std::cout << "window.window_mode not found" << std::endl;
				window_cfg.window_mode = renderer::WindowFlags::RESIZABLE;
			}

			//editor
			try{
				vsync = cfg["editor"]["vsync"].get<bool>();
			} catch(...){
				std::cout << "editor.vsync not found" << std::endl;
				vsync = true;
			}
			try{
				max_fps = cfg["editor"]["max_fps"].get<int>();
			} catch(...){
				std::cout << "editor.max_fps not found" << std::endl;
				max_fps = 120;
			}
		}
	}

	Config& Config::Instance(){
		static Config instance;
		return instance;
	}

	void Config::SetDefaults(){
		logger_cfg.file_name = "logs/editor_log.txt";
		logger_cfg.size = 1024 * 1024 * 5;
		logger_cfg.number_of_files = 1;
		logger_cfg.level = spdlog::level::trace;

		window_cfg.title = "Map Editor";
		window_cfg.width = 900;
		window_cfg.height = 540;
		window_cfg.trap_mouse = false;
		window_cfg.window_mode = renderer::WindowFlags::RESIZABLE;
		
		vsync = true;
		max_fps = 120;
	}

}
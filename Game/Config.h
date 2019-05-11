#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace utilities{

	namespace{
		struct logger_cfg_t{
			std::string file_name;
			int size;
			int number_of_files;
			unsigned int level;
		};

		struct window_cfg_t{
			std::string title;
			int width;
			int height;
			int window_mode;
			bool trap_mouse;
		};
	}

	class Config{
	public:
		static Config& Instance();

		Config(Config const&) = delete;
		void operator=(Config const&) = delete;

		logger_cfg_t logger_cfg;
		window_cfg_t window_cfg;
		bool vsync;
		int max_graphics_fps;
		int max_physics_fps;
		float max_frame_time;
		std::string map;

	private:
		void SetDefaults();
		Config();
	};

}

#define CONFIG utilities::Config::Instance()

#endif // !CONFIG_H

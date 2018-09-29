#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace utilities{

	class Config{
	public:
		static Config& Instance();

		Config(Config const&) = delete;
		void operator=(Config const&) = delete;

		struct logger_cfg_t{
			std::string file_name;
			int size;
			int number_of_files;
			unsigned int level;
		};

		logger_cfg_t logger_cfg;

	private:
		void SetDefaults();
		Config();
	};

}

#define CONFIG utilities::Config::Instance()

#endif // !CONFIG_H

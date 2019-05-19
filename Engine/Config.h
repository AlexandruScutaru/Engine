#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace utilities{

	class Config_e{
	public:
		static Config_e& Instance();

		Config_e(Config_e const&) = delete;
		void operator=(Config_e const&) = delete;

		struct logger_cfg_t{
			std::string file_name;
			int size;
			int number_of_files;
			unsigned int level;
		};

		logger_cfg_t logger_cfg;

	private:
		void SetDefaults();
		Config_e();
	};

}

#define CONFIG_E utilities::Config_e::Instance()

#endif // !CONFIG_H

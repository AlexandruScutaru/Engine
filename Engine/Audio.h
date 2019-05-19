#ifndef AUDIO_H
#define AUDIO_H

#include <MIXER/SDL_mixer.h>

#include <string>
#include <map>

namespace audio{

	class SoundEffect{
	public:
		friend class AudioManager;

		void play(int loops = 0);

	private:
		Mix_Chunk* m_chunk = nullptr;

	};

	class Music{
	public:
		friend class AudioManager;

		void play(int loops = 1);
		static void pause();
		static void resume();
		static void stop();

	private:
		Mix_Music* m_music = nullptr;

	};

	class AudioManager{
	public:
		AudioManager();
		~AudioManager();

		void init();
		void destroy();

		SoundEffect loadSountEffect(const std::string& filePath);
		Music loadMusic(const std::string& filePath);

	private:
		std::map<std::string, Mix_Chunk*> m_soundEffectsMap;
		std::map<std::string, Mix_Music*> m_musicMaps;

		bool m_isInitialized = false;
	};

}

#endif // !AUDIO_H
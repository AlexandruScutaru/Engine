#include "Audio.h"
#include "Logger.h"

namespace audio{

	///SoundEffect Class
	void SoundEffect::play(int loops /* = 0*/){
		if(Mix_PlayChannel(-1, m_chunk, loops) == -1){
			if(Mix_PlayChannel(0, m_chunk, loops) == -1){
				LOG_E_ERROR_TRACEABLE("audio::SoundEffect::play error: ", Mix_GetError());
				exit(EXIT_FAILURE);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////

	///Music Class
	void Music::play(int loops /* = 1*/){
		Mix_PlayMusic(m_music, loops);
	}

	void Music::pause(){
		Mix_PauseMusic();
	}

	void Music::resume(){
		Mix_ResumeMusic();
	}

	void Music::stop(){
		Mix_HaltMusic();
	}

	///////////////////////////////////////////////////////////////////////

	///AudioEngine Class
	AudioManager::AudioManager(){}

	AudioManager::~AudioManager(){
		destroy();
	}

	void AudioManager::init(){
		if(!m_isInitialized){
			//Parameter can be a bitwise combination of:
			//MIX_INIT_FAC, MIX_INIT_MOD, MIX_INIT_MP3, MIX_INIT_OGG
			if(Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1){
				LOG_E_ERROR_TRACEABLE("audio::AudioEngine::init::Mix_init error: " + std::string(Mix_GetError()));
				exit(EXIT_FAILURE);
			}

			if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1){
				LOG_E_ERROR_TRACEABLE("audio::AudioEngine::init::Mix_OpenAudio error: " + std::string(Mix_GetError()));
				exit(EXIT_FAILURE);
			}

			m_isInitialized = true;
			LOG_E_INFO("audio::AudioManager::init succeeded");
		}
	}

	void AudioManager::destroy(){
		if(m_isInitialized){
			for(auto& it : m_soundEffectsMap){
				Mix_FreeChunk(it.second);
			}
			for(auto& it : m_musicMaps){
				Mix_FreeMusic(it.second);
			}

			m_soundEffectsMap.clear();
			m_musicMaps.clear();

			Mix_CloseAudio();
			Mix_Quit();

			m_isInitialized = false;
		}
	}

	SoundEffect AudioManager::loadSountEffect(const std::string& filePath){
		SoundEffect effect;

		auto it = m_soundEffectsMap.find(filePath);
		if(it == m_soundEffectsMap.end()){
			Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
			if(chunk == nullptr){
				LOG_E_ERROR_TRACEABLE("audio::AudioEngine::loadSoundEffect::Mix_LoadWAV error: " + std::string(Mix_GetError()));
				exit(EXIT_FAILURE);
			}
			effect.m_chunk = chunk;
			m_soundEffectsMap[filePath] = chunk;
		} else{
			effect.m_chunk = it->second;
		}

		return effect;
	}

	Music AudioManager::loadMusic(const std::string& filePath){
		Music music;

		auto it = m_musicMaps.find(filePath);
		if(it == m_musicMaps.end()){
			Mix_Music* loadedMusic = Mix_LoadMUS(filePath.c_str());
			if(loadedMusic == nullptr){
				LOG_E_ERROR_TRACEABLE("audio::AudioEngine::loadSoundEffect::Mix_LoadMUS error: " + std::string(Mix_GetError()));
				exit(EXIT_FAILURE);
			}
			music.m_music = loadedMusic;
			m_musicMaps[filePath] = loadedMusic;
		} else{
			music.m_music = it->second;
		}

		return music;
	}

}
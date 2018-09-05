#include "Timing.h"

#include <SDL/SDL.h>


#define MS_PER_S 1000.0f


namespace utilities{

	/// FpsLimiter
	FpsLimiter::FpsLimiter(){}

	FpsLimiter::~FpsLimiter(){}


	void FpsLimiter::setMaxFPS(unsigned int maxFPS){
		m_maxFPS = maxFPS;
		m_previousTicks = SDL_GetTicks();
	}

	float FpsLimiter::begin(){
		m_startTicks = SDL_GetTicks();

		//unsigned int newTicks = SDL_GetTicks();
		unsigned int frameTime = m_startTicks - m_previousTicks;
		m_previousTicks = m_startTicks;
		float deltaTime = frameTime / MS_PER_S;
		return deltaTime;
	}

	void FpsLimiter::end(){
		unsigned int frameTicks = SDL_GetTicks() - m_startTicks;
		//limit the FPS to max FPS
		if(1000.0f / m_maxFPS > (float)frameTicks){
			SDL_Delay((unsigned int)(1000.0f / m_maxFPS) - frameTicks);
		}
	}


	/// Timer
	Timer::Timer() :
		m_startTicks(0),
		m_pausedTicks(0),
		m_paused(false),
		m_started(false)
	{}

	Timer::~Timer(){}


	void Timer::start(){
		m_started = true;
		m_paused = false;
		m_startTicks = SDL_GetTicks();
		m_pausedTicks = 0;
	}

	void Timer::stop(){
		m_started = false;
		m_paused = false;
		m_startTicks = 0;
		m_pausedTicks = 0;
	}

	void Timer::pause(){
		if(m_started && !m_paused){
			m_paused = true;
			m_pausedTicks = SDL_GetTicks() - m_startTicks;
			m_startTicks = 0;
		}
	}

	void Timer::unpause(){
		if(m_started && m_paused){
			m_paused = false;
			m_startTicks = SDL_GetTicks() - m_pausedTicks;
			m_pausedTicks = 0;
		}
	}

	unsigned int Timer::getTicks(){
		Uint32 time = 0;
		if(m_started){
			if(m_paused){
				time = m_pausedTicks;
			} else{
				time = SDL_GetTicks() - m_startTicks;
			}
		}

		return time;
	}

}
#ifndef TIMING_H
#define TIMING_H

#include <SDL/SDL.h>

namespace renderer{

	//limits FPS
	class FpsLimiter{
	public:
		FpsLimiter();
		~FpsLimiter();

		void setMaxFPS(unsigned int maxFPS);
		float begin();
		void end();

	private:
		unsigned int m_maxFPS;
		float m_frameTime;

		unsigned int m_startTicks;
		unsigned int m_previousTicks;
	};

	//maybe I will use it
	class Timer{
	public:
		Timer();
		~Timer();

		void start();
		void stop();
		void pause();
		void unpause();

		unsigned int getTicks();
		bool isStarted(){ return m_started; }
		bool isPaused(){ return m_paused; }

	private:
		unsigned int m_startTicks;
		unsigned int m_pausedTicks;
		bool m_paused;
		bool m_started;

	};

}

#endif // TIMING_H
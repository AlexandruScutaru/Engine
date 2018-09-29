#ifndef WINDOW_H
#define WINDOW_H

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <string>


namespace renderer{

	enum WindowFlags{
		DEFAULT = 0x0,
		INVISIBLE = 0x1,
		FULLSCREEN = 0x2,
		BORDERLESS = 0x4,
		RESIZABLE = 0x8
	};

	//Creates and manages a window
	class Window{
	public:
		Window();
		~Window();

		void create(const std::string& name, int w, int h, unsigned int currentFlags = 0);
		void swapBuffer();

		static void setW(int w) { m_width = w; }
		static void setH(int h) { m_height = h; }
		//trap and get relative coords from the mouse
		static void setMouseTrapping(SDL_bool value);
		static void setVSync(bool value);

		SDL_Window* getWindow(){ return m_sdlWindow; }
		static int getW() { return m_width; }
		static int getH(){ return m_height; }
		
	private:
		static int m_width;
		static int m_height;
		SDL_Window* m_sdlWindow;
		SDL_GLContext m_glContext;

	};

}
#endif // WINDOW_H
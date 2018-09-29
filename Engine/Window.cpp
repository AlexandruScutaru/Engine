#include "Window.h"
#include "Logger.h"

#include <iostream>
#include <cstdlib>


namespace renderer{

	int Window::m_width = 0;
	int Window::m_height = 0;

	Window::Window(){}

	Window::~Window(){
		SDL_GL_DeleteContext(m_glContext);
		SDL_DestroyWindow(m_sdlWindow);
		m_sdlWindow = nullptr;
		SDL_Quit();
	}

	void Window::create(const std::string& name, int w, int h, unsigned int currentFlags){
		m_width = w;
		m_height = h;

		//initialize SDL VIDEO system
		if(SDL_Init(SDL_INIT_VIDEO) < 0){
			LOG_ERROR_TRACEABLE("SDL_Init::SDL_INIT_VIDEO failed with error: '{}'!", SDL_GetError());
			exit(EXIT_FAILURE);
		} else{
			LOG_INFO("SDL_Init::SDL_INIT_VIDEO succeeded");
		}
 
		//creates "at least" 2^8 shades of the chosen color 
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		//results 32 bits of color data(RGBA)

		//how much data in bits to be allocated for a pixel 
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

		//allocate memory for the zBuffer(depth buffer)
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

		//if value is 1 => SDL allocates space for 2 windows
		//for swapping over 2 contexts for smoothness
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		//set opengl version
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		//Default flag - so openGL cant take control over it
		Uint32 flags = SDL_WINDOW_OPENGL;
		//check the user flags
		if(currentFlags & INVISIBLE)
			flags |= SDL_WINDOW_HIDDEN;
		if(currentFlags & FULLSCREEN)
			flags |= SDL_WINDOW_FULLSCREEN;
		if(currentFlags & BORDERLESS)
			flags |= SDL_WINDOW_BORDERLESS;
		if(currentFlags & RESIZABLE)
			flags |= SDL_WINDOW_RESIZABLE;

		//create a SDL window
		m_sdlWindow = SDL_CreateWindow(
			name.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			w, 
			h, 
			flags
		);
		if(m_sdlWindow == nullptr){
			LOG_ERROR_TRACEABLE("SDL_CreateWindow failed with error: '{}'!", SDL_GetError());
			exit(EXIT_FAILURE);
		} else{
			LOG_INFO("SDL_CreateWindow succeeded");
		}

		//tells SDL to create a context from the window
		//now we can use openGL and the GPU to draw on the window
		m_glContext = SDL_GL_CreateContext(m_sdlWindow);
		if(m_glContext == nullptr){
			LOG_ERROR_TRACEABLE("SDL_GL_CreateContext from window failed with error: '{}'!", SDL_GetError());
			exit(EXIT_FAILURE);
		} else{
			LOG_INFO("SDL_GL_CreateContext succeeded");
		}

		//initialize glew
		glewExperimental = GL_TRUE;
		//searches operating system for every OpenGL function that is supported :-??
		GLenum status = glewInit();
		if(status != GLEW_OK){
			LOG_ERROR_TRACEABLE("glewInit failed with error: {} '{}'!", status, glewGetErrorString(status));
			exit(EXIT_FAILURE);
		} else{
			LOG_INFO("glewInit succeeded");
		}

		glEnable(GL_DEPTH_TEST);

		//Check the OpenGL version
		LOG_INFO("OpenGL version: {}", glGetString(GL_VERSION));

		setVSync(true);
		setMouseTrapping(SDL_FALSE);
		
		LOG_INFO("window creation succeeded");
	}

	void Window::swapBuffer(){
		//Swap our buffer and draw everything to the screen!
		SDL_GL_SwapWindow(m_sdlWindow);
	}

	void Window::setMouseTrapping(SDL_bool value){
		SDL_SetRelativeMouseMode(value);
	}

	void Window::setVSync(bool value){
		//set V-Sync (0 = off | 1 = on)
		SDL_GL_SetSwapInterval((int)value);
	}

}
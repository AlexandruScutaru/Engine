#include <cstdlib>
#include <iostream>

#include <Renderer/Renderer.h>


int main(int argc, char** argv){
	
	renderer::Window* window = new renderer::Window();
	window->create("test", 800, 600);
	SDL_Delay(2000);
	delete window;

	exit(EXIT_SUCCESS);
}
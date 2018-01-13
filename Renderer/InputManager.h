#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <unordered_map>
#include <glm/glm.hpp>

namespace renderer{

	// Input manager stores a key map that maps SDL_Keys to booleans.
	// If the value in the key map is true, then the key is pressed.
	// Otherwise it is released.
	class InputManager{
	public:
		InputManager();
		~InputManager();

		void update();

		void pressKey(unsigned int keyID);
		void releaseKey(unsigned int keyID);

		//returns true if the key is held down
		bool isKeyDown(unsigned int keyID);
		//returns true if the key was just pressed
		bool isKeyPressed(unsigned int keyID);

		void setMouseCoords(float x, float y);
		glm::vec2 getMouseCoords() const;
		float getMouseDY();
		float getMouseDX();

		void setMouseWheel(int value){ m_wheel = value; }
		int getMouseWheel(){ return m_wheel; }

	private:
		bool wasKeyDown(unsigned int keyID);

		std::unordered_map<unsigned int, bool> m_keyMap;
		std::unordered_map<unsigned int, bool> m_previousKeyMap;

		glm::vec2 m_mouseCoords;
		int m_wheel;

	};

}

#endif // INPUTMANAGER_H
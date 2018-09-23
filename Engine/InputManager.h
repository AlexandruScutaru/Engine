#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <glm/glm.hpp>

#include <unordered_map>

namespace utilities{

	// Input manager stores a key map that maps SDL_Keys to booleans.
	// If the value in the key map is true, then the key is pressed.
	// Otherwise it is released.
	class InputManager{
	public:
		InputManager();
		~InputManager();

		//saves the previous states of the keys
		void update();
		//tells input manager that this key was pressed
		void pressKey(unsigned int keyID);
		//tells input manager that this key was released
		void releaseKey(unsigned int keyID);
		//returns true if the key is held down
		bool isKeyDown(unsigned int keyID);
		//returns true if the key was just pressed
		bool isKeyPressed(unsigned int keyID);
		//sets relative mouse motion in X and Y direction
		void setRelMouseCoords(float x, float y);
		//gets relative mouse motion in X and Y direction
		glm::vec2 getRelMouseCoords() const;
		//sets mouse position relative to the window
		void setActualMouseCoords(float x, float y);
		//gets mouse position relative to the window
		glm::vec2 getActualMouseCoords() const;
		//gets relative mouse motion in X direction
		float getMouseDX();
		//gets relative mouse motion in Y direction
		float getMouseDY();
		//sets amount scrolled vertically, positive away, negative toward
		void setMouseWheel(int value){ m_wheel = value; }
		//gets amount scrolled vertically, positive away, negative toward
		int getMouseWheel(){ return m_wheel; }

	private:
		bool wasKeyDown(unsigned int keyID);

		std::unordered_map<unsigned int, bool> m_keyMap;
		std::unordered_map<unsigned int, bool> m_previousKeyMap;

		glm::vec2 m_relMouseCoords;
		glm::vec2 m_actualMouseCoords;
		int m_wheel;

	};

}

#endif // INPUTMANAGER_H
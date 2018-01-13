#include "InputManager.h"
#include <SDL/SDL.h>


namespace renderer{

	InputManager::InputManager() :
		m_mouseCoords(0.0f, 0.0f),
		m_wheel(0)
	{}

	InputManager::~InputManager(){}


	//saves the previous states of the keys
	void InputManager::update(){
		for(auto& it : m_keyMap){
			//_previousKeyMap[it.first] = _keyMap[it.second];
			m_previousKeyMap[it.first] = it.second;
		}
		m_mouseCoords = glm::vec2(0.0f);
		//m_prevMouseCoords = m_mouseCoords;
		m_wheel = 0;
	}

	void InputManager::pressKey(unsigned int keyID){
		// if keyID doesn't already exist in _keyMap, it will be added
		m_keyMap[keyID] = true;
	}

	void InputManager::releaseKey(unsigned int keyID){
		m_keyMap[keyID] = false;
	}

	bool InputManager::isKeyDown(unsigned int keyID){
		// We dont want to use the associative array approach here
		// because we don't want to create a key if it doesnt exist.
		// So we do it manually
		auto it = m_keyMap.find(keyID);
		if(it != m_keyMap.end()){
			return it->second;
		} else {
			// Didn't find the key
			return false;
		}
	}

	bool InputManager::isKeyPressed(unsigned int keyID){
		if((isKeyDown(keyID) == true) && (wasKeyDown(keyID) == false)){
			return true;
		}
		return false;
	}

	void InputManager::setMouseCoords(float x, float y){
		m_mouseCoords.x = x;
		m_mouseCoords.y = y;
	}

	glm::vec2 InputManager::getMouseCoords() const{
		return m_mouseCoords;
	}

	float InputManager::getMouseDY(){
		return m_mouseCoords.y;// -m_prevMouseCoords.y;
	}

	float InputManager::getMouseDX(){
		return m_mouseCoords.x;// -m_prevMouseCoords.x;
	}

	bool InputManager::wasKeyDown(unsigned int keyID){
		auto it = m_previousKeyMap.find(keyID);
		if(it != m_previousKeyMap.end()){
			return it->second;
		} else {
			// Didn't find the key
			return false;
		}
	}

}
#include "InputManager.h"

#include <SDL/SDL.h>

namespace utilities{

	InputManager::InputManager() :
		m_relMouseCoords(0.0f, 0.0f),
		m_actualMouseCoords(0.0f, 0.0f),
		m_wheel(0)
	{}

	InputManager::~InputManager(){}


	void InputManager::update(){
		for(auto& it : m_keyMap){
			m_previousKeyMap[it.first] = it.second;
		}
		m_relMouseCoords = glm::vec2(0.0f);
		m_wheel = 0;
	}

	void InputManager::pressKey(unsigned int keyID){
		m_keyMap[keyID] = true;
	}

	void InputManager::releaseKey(unsigned int keyID){
		m_keyMap[keyID] = false;
	}

	bool InputManager::isKeyDown(unsigned int keyID){
		auto it = m_keyMap.find(keyID);
		if(it != m_keyMap.end()){
			return it->second;
		}
		return false;
	}

	bool InputManager::isKeyPressed(unsigned int keyID){
		if((isKeyDown(keyID) == true) && (wasKeyDown(keyID) == false)){
			return true;
		}
		return false;
	}

	void InputManager::setRelMouseCoords(float x, float y){
		m_relMouseCoords.x = x;
		m_relMouseCoords.y = y;
	}

	glm::vec2 InputManager::getRelMouseCoords() const{
		return m_relMouseCoords;
	}

	void InputManager::setActualMouseCoords(float x, float y){
		m_actualMouseCoords.x = x;
		m_actualMouseCoords.y = y;
	}

	glm::vec2 InputManager::getActualMouseCoords() const{
		return m_actualMouseCoords;
	}

	float InputManager::getMouseDY(){
		return m_relMouseCoords.y;
	}

	float InputManager::getMouseDX(){
		return m_relMouseCoords.x;
	}

	bool InputManager::wasKeyDown(unsigned int keyID){
		auto it = m_previousKeyMap.find(keyID);
		if(it != m_previousKeyMap.end()){
			return it->second;
		}
		return false;
	}

}

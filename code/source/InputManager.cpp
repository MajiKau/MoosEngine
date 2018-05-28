#include "code/headers/InputManager.h"

InputManager* InputManager::inputManager = 0;

InputManager* InputManager::Inst() { if (inputManager == 0)inputManager = new InputManager(); return inputManager; }

InputManager::InputManager()
{
	mousePosition = vector<unsigned int>(2);
    mouseDeltaPosition = vector<int>(2) = { 0, 0 };
};
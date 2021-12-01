#pragma once

#include <dinput.h>
#include "camera.h"

class InputHandling
{
private:
	IDirectInput8* m_direct_input;
	IDirectInputDevice8* m_keyboard_device;
	unsigned char m_keyboard_keys_state[256];

public:
	HRESULT InitialiseInput(HINSTANCE hInst, HWND hWnd);
	void ShutdownInput();
	
	void ReadInputStates();
	void HandleInput(HWND hWnd, Camera* camera);

	bool IsKeyPressed(unsigned char DI_keycode);
};
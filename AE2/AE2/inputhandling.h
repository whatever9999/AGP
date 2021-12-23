#pragma once

#include <dinput.h>
#include "player.h"

class InputHandling
{
private:
	enum MOUSE_INPUT
	{
		// Movement
		X_POSITIVE,
		X_NEGATIVE,
		Y_POSITIVE,
		Y_NEGATIVE,
		SCROLL_POSITIVE,
		SCROLL_NEGATIVE,
		// Buttons
		LEFT_BUTTON,
		RIGHT_BUTTON,
		CENTRE_BUTTON,
		OTHER_BUTTON,
	};

	enum BUTTONS
	{
		JUMP,
		MELEE_ATTACK,
		SPELL_ATTACK,
		NUM_BUTTONS,
	};

	IDirectInput8* m_direct_input;

	IDirectInputDevice8* m_keyboard_device;
	IDirectInputDevice8* m_mouse_device;

	unsigned char m_keyboard_keys_state[256];
	DIMOUSESTATE m_mouse_state;

	bool m_button_pressed[NUM_BUTTONS];

public:
	HRESULT InitialiseInput(HINSTANCE hInst, HWND hWnd);
	void ShutdownInput();
	
	void ReadInputStates();
	void HandleInput(HWND hWnd, Player* player);

	bool IsKeyPressed(unsigned char DI_keycode);
	bool IsMouseChanged(MOUSE_INPUT mouse_input);
};
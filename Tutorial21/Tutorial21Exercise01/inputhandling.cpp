#include "inputhandling.h"


HRESULT InputHandling::InitialiseInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// Create DirectInput object
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_direct_input, NULL);
	if (FAILED(hr)) return hr;
	
	// KEYBOARD
	// Clear the keyboard state array
	ZeroMemory(m_keyboard_keys_state, sizeof(m_keyboard_keys_state));

	// Create a keyboard
	hr = m_direct_input->CreateDevice(GUID_SysKeyboard, &m_keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	// Tell input device how to interpret data
	hr = m_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	// Tell input device how to interact with other apps using DirectInput
	hr = m_keyboard_device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	// Tell device to start providing input
	hr = m_keyboard_device->Acquire();
	if (FAILED(hr)) return hr;

	// MOUSE
	// Create a mouse
	hr = m_direct_input->CreateDevice(GUID_SysMouse, &m_mouse_device, NULL);
	if (FAILED(hr)) return hr;

	// Tell input device how to interpret data
	hr = m_mouse_device->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) return hr;

	// Tell input device how to interact with other apps using DirectInput
	hr = m_mouse_device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(hr)) return hr;

	// Tell device to start providing input
	hr = m_mouse_device->Acquire();
	if (FAILED(hr)) return hr;

	return S_OK;
}
void InputHandling::ShutdownInput()
{
	if (m_mouse_device)
	{
		m_mouse_device->Unacquire();
		m_mouse_device->Release();
	}

	if (m_keyboard_device)
	{
		m_keyboard_device->Unacquire();
		m_keyboard_device->Release();
	}

	if (m_direct_input) m_direct_input->Release();
}

void InputHandling::ReadInputStates()
{
	HRESULT hr;

	// KEYBOARD
	hr = m_keyboard_device->GetDeviceState(sizeof(m_keyboard_keys_state), (LPVOID)&m_keyboard_keys_state);
	
	// Try to reaquire input if needed
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			m_keyboard_device->Acquire();
		}
	}

	// MOUSE
	hr = m_mouse_device->GetDeviceState(sizeof(m_mouse_state), (LPVOID)&m_mouse_state);

	// Try to reaquire input if needed
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			m_mouse_device->Acquire();
		}
	}
}
void InputHandling::HandleInput(HWND hWnd, Camera* camera)
{
	// Exit
	if (IsKeyPressed(DIK_ESCAPE)) DestroyWindow(hWnd);

	// Keyboard Movement
	if (IsKeyPressed(DIK_W)) camera->Forward(0.01);
	if (IsKeyPressed(DIK_S)) camera->Forward(-0.01);
	if (IsKeyPressed(DIK_D)) camera->Strafe(-0.01);
	if (IsKeyPressed(DIK_A)) camera->Strafe(0.01);

	if (IsKeyPressed(DIK_Z)) camera->Up(0.01);
	if (IsKeyPressed(DIK_X)) camera->Up(-0.01);

	if (IsKeyPressed(DIK_UP)) camera->Pitch(0.01);
	if (IsKeyPressed(DIK_DOWN)) camera->Pitch(-0.01);
	if (IsKeyPressed(DIK_Q)) camera->Rotate(-0.05);
	if (IsKeyPressed(DIK_E)) camera->Rotate(0.05);

	// Mouse Movement
	//if (IsMouseChanged(Y_POSITIVE)) camera->Pitch(-1.5);
	//if (IsMouseChanged(Y_NEGATIVE)) camera->Pitch(1.5);
	if (IsMouseChanged(X_POSITIVE)) camera->Rotate(2);
	if (IsMouseChanged(X_NEGATIVE)) camera->Rotate(-2);

	// Buttons (don't trigger when held)
	if (!m_button_pressed[JUMP])
	{
		// Only allow jump if we're on the floor
		if (IsKeyPressed(DIK_SPACE) && camera->GetY() < 0.1)
		{
			camera->Jump();
			m_button_pressed[JUMP] = true;
		}
	}
	else
	{
		if (!IsKeyPressed(DIK_SPACE))
		{
			m_button_pressed[JUMP] = false;
		}
	}
}

bool InputHandling::IsKeyPressed(unsigned char DI_keycode)
{
	return m_keyboard_keys_state[DI_keycode] & 0x80;
}
bool InputHandling::IsMouseChanged(MOUSE_INPUT mouse_input)
{
	// Movement
	switch (mouse_input)
	{
	case InputHandling::X_POSITIVE:
		return m_mouse_state.lX > 0;
	case InputHandling::X_NEGATIVE:
		return m_mouse_state.lX < 0;
	case InputHandling::Y_POSITIVE:
		return m_mouse_state.lY > 0;
	case InputHandling::Y_NEGATIVE:
		return m_mouse_state.lY < 0;
	case InputHandling::SCROLL_POSITIVE:
		return m_mouse_state.lZ > 0;
	case InputHandling::SCROLL_NEGATIVE:
		return m_mouse_state.lZ < 0;
	}
	// Buttons
	return m_mouse_state.rgbButtons[mouse_input - LEFT_BUTTON] & 0x80;
}
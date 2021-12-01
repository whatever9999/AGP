#include "inputhandling.h"


HRESULT InputHandling::InitialiseInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;
	
	// Clear the keyboard state array
	ZeroMemory(m_keyboard_keys_state, sizeof(m_keyboard_keys_state));

	// Create DirectInput object
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_direct_input, NULL);
	if (FAILED(hr)) return hr;

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

	return S_OK;
}
void InputHandling::ShutdownInput()
{
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

	hr = m_keyboard_device->GetDeviceState(sizeof(m_keyboard_keys_state), (LPVOID)&m_keyboard_keys_state);
	
	// Try to reaquire input if needed
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			m_keyboard_device->Acquire();
		}
	}
}
void InputHandling::HandleInput(HWND hWnd, Camera* camera)
{
	// Exit
	if (IsKeyPressed(DIK_ESCAPE)) DestroyWindow(hWnd);

	// Camera
	if (IsKeyPressed(DIK_SPACE)) camera->Jump();
	if (IsKeyPressed(DIK_UP)) camera->Pitch(1);
	if (IsKeyPressed(DIK_DOWN)) camera->Pitch(-1);
	if (IsKeyPressed(DIK_W)) camera->Forward(1);
	if (IsKeyPressed(DIK_S)) camera->Forward(-1);
	if (IsKeyPressed(DIK_Z)) camera->Up(1);
	if (IsKeyPressed(DIK_X)) camera->Up(-1);
	if (IsKeyPressed(DIK_Q)) camera->Rotate(-5);
	if (IsKeyPressed(DIK_E)) camera->Rotate(5);
	if (IsKeyPressed(DIK_D)) camera->Strafe(-1);
	if (IsKeyPressed(DIK_A)) camera->Strafe(1);
}

bool InputHandling::IsKeyPressed(unsigned char DI_keycode)
{
	return m_keyboard_keys_state[DI_keycode] & 0x80;
}
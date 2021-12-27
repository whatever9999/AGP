#include "game.h"

Game* g_game;

// Forward declaration
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#pragma region Initialisation
	g_game = new Game();

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	g_game->SetInputHandling(new InputHandling());
	if (FAILED(g_game->GetInputHandling()->InitialiseInput(g_game->GetInst(), g_game->GetWnd())))
	{
		DXTRACE_MSG("Failed to initialise input");
		return 0;
	}

	MSG msg = { 0 };

	if (FAILED(g_game->InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(g_game->InitialiseGame()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
	}
#pragma endregion

	// Main Loop
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_game->GameLoop();
		}
	}

	// Quitting
	g_game->ShutdownD3D();
	if (g_game) delete g_game;

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_SIZE:
		if (g_game->GetSwapChain())
		{
			UINT new_size[2];
			new_size[0] = LOWORD(lParam);
			new_size[1] = HIWORD(lParam);

			g_game->ResizeWindow(new_size);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	char Name[100] = "Game\0";

	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	g_game->SetInst(hInstance);
	RECT rc = { 0, 0, 960, 720 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_game->SetWnd(CreateWindow(Name, "AGP AE2\0", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL));

	if (!g_game->GetWnd())
	{
		return E_FAIL;
	}

	ShowWindow(g_game->GetWnd(), nCmdShow);

	return S_OK;
}
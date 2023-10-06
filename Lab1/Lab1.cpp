
#include "framework.h"
#include "Lab1.h"
#include <windows.h>

#pragma comment(lib, "Msimg32.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const int BITMAP_SIZE = 200;
HDC hdcBack;
HANDLE handleBack;
RECT clientFrame;
POINT imageCoorg;
RECT clientRect;

void InitializeBackground(HWND hWnd, int width, int height)
{
	HDC hdcWindow;
	hdcWindow = GetDC(hWnd);
	hdcBack = CreateCompatibleDC(hdcWindow);
	handleBack = CreateCompatibleBitmap(hdcWindow, width, height);
	ReleaseDC(hWnd, hdcWindow);
	SaveDC(hdcBack);
	SelectObject(hdcBack, handleBack);
}

void FreeBackground()
{
	if (hdcBack)
	{
		RestoreDC(hdcBack, -1);
		DeleteObject(handleBack);
		DeleteDC(hdcBack);
		hdcBack = 0;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
	wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"FirstLab";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	if (!RegisterClassEx(&wcex))
	{
		return 0;
	}

	hWnd = CreateWindowEx(0, L"FirstLab", L"SP1", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;

	static HDC hdc;
	static BITMAP image;
	static HANDLE handleBitmap;

	GetClientRect(hWnd, &clientRect);

	switch (uMsg)
	{
	case WM_DESTROY:
	{
		FreeBackground();
		PostQuitMessage(0);
		break;
	}
	case WM_SIZE:
	{
		GetClientRect(hWnd, &clientFrame);
		FreeBackground();
		InitializeBackground(hWnd, clientFrame.right - clientFrame.left, clientFrame.bottom - clientFrame.top);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



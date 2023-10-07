
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

void DrawImage(HANDLE& hBmp, BITMAP& bitmap, POINT& bitmapCoordinates)
{
	// Save dc to place image 
	SaveDC(hdcBack);
	FillRect(hdcBack, &clientFrame, (HBRUSH)(COLOR_WINDOW + 1));
	RECT image_rect;
	SetRect(&image_rect, bitmapCoordinates.x, bitmapCoordinates.y, bitmapCoordinates.x + bitmap.bmWidth, bitmapCoordinates.y + bitmap.bmHeight);
	Rectangle(hdcBack, bitmapCoordinates.x, bitmapCoordinates.y, bitmapCoordinates.x + bitmap.bmWidth, bitmapCoordinates.y + bitmap.bmHeight);//
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 255));
	FillRect(hdcBack, &image_rect, hBrush);
	HDC hMemDC = CreateCompatibleDC(hdcBack);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);
	if (hOldBmp)
	{
		SetMapMode(hMemDC, GetMapMode(hdcBack));
		TransparentBlt(hdcBack, bitmapCoordinates.x, bitmapCoordinates.y, BITMAP_SIZE, BITMAP_SIZE, hMemDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0, 0, 0));
		SelectObject(hMemDC, hOldBmp);
	}
	DeleteDC(hMemDC);

	// Replace context with image
	RestoreDC(hdcBack, -1);
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
	wcex.hIcon = LoadIcon(hInstance, NULL);
	wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"FirstLab";
	wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);

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

void MoveImage(WPARAM wParam) {
	int direction = GET_WHEEL_DELTA_WPARAM(wParam);
	if (LOWORD(wParam) & MK_SHIFT)
	{
		(imageCoorg.x - (direction / 25) >= clientRect.left) && (imageCoorg.x - (direction / 25) <= clientRect.right - BITMAP_SIZE) ? imageCoorg.x -= (direction / 25) : imageCoorg.x += (direction / 25);
	}
	else
	{
		(imageCoorg.y + (direction / 25) >= clientRect.top) && (imageCoorg.y + (direction / 25) <= clientRect.bottom - BITMAP_SIZE) ? imageCoorg.y += (direction / 25) : imageCoorg.y -= (direction / 25);
	}
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
	case WM_CREATE:
	{
		handleBitmap = LoadImage(NULL, L"picture.bmp", IMAGE_BITMAP, BITMAP_SIZE, BITMAP_SIZE, LR_LOADFROMFILE | LR_LOADTRANSPARENT);
		GetObject(handleBitmap, sizeof(image), &image);

		imageCoorg = { 100, 100 };
		break;
	}
	case WM_DESTROY:
	{
		FreeBackground();
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT:
	{
		DrawImage(handleBitmap, image, imageCoorg);

		hdc = BeginPaint(hWnd, &paintStruct);
		BitBlt(hdc, 0, 0, clientFrame.right - clientFrame.left, clientFrame.bottom - clientFrame.top, hdcBack, 0, 0, SRCCOPY);
		EndPaint(hWnd, &paintStruct);
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
	case WM_MOUSEWHEEL:
	{
		MoveImage(wParam);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



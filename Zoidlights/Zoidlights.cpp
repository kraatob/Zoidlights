// Zoidlights.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Zoidlights.h"
#include "Constants.h"
#include "Light.h"
#include "LightManager.h"

// Global Variables:
HINSTANCE hInst;
LightManager *lightManager = nullptr;

// Forward declarations of functions included in this code module:
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void                InitLights();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   const wchar_t szWindowClass[] = L"Zoidlights Main Window";
   WNDCLASSEXW wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZOIDLIGHTS));
   wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszClassName = szWindowClass;
   wcex.lpszMenuName = NULL;
   wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

   RegisterClassExW(&wcex);

   HWND hWnd = CreateWindowW(szWindowClass, L"Zoidlights", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   SetTimer(hWnd, IDT_LOOP_TIMER, (UINT) (1000.0 / FRAME_RATE), NULL);

   ShowWindow(hWnd, nCmdShow);

   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
        }
        break;
    case WM_PAINT:
	{
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (lightManager) {
            RECT boundingBox;
            GetClientRect(hWnd, &boundingBox);
            for (UINT i = 0; i < lightManager->m_lightCount; i++) {
                Light* light = &lightManager->m_lights[i];
                LOGBRUSH lbBrushDefinition;
                lbBrushDefinition.lbStyle = BS_SOLID;
                lbBrushDefinition.lbColor = *light->color;
                LONG x = (LONG)(light->x * (boundingBox.right - boundingBox.left)) + boundingBox.left;
                LONG y = (LONG)(light->y * (boundingBox.bottom - boundingBox.top)) + boundingBox.top;
                RECT drawBox = RECT{ x - 10, y - 10, x + 10, y + 10 };
                HBRUSH hBrush = CreateBrushIndirect(&lbBrushDefinition);
                FillRect(hdc, &drawBox, hBrush);
                DeleteObject(hBrush);
            }
        }
        EndPaint(hWnd, &ps);
    }
	break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        switch (wParam) {
		case IDT_LOOP_TIMER: 
            if ( !lightManager ) {
                lightManager = new LightManager();
            }
            if (!lightManager->Update()) {
                // something went wrong, we reinitialize
                delete lightManager;
                lightManager = nullptr;
            }
            InvalidateRect(hWnd, NULL, FALSE);
            UpdateWindow(hWnd);
            break;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

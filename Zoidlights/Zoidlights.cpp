// Zoidlights.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Zoidlights.h"
#include "DuplicationManager.h"
#include "LightRegion.h"

// Global Variables:
HINSTANCE hInst;                                // current instance
DuplicationManager duplicationManager;
Light lights[40];
const UINT FRAME_RATE = 15;



// Forward declarations of functions included in this code module:
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
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

   duplicationManager.InitDuplication(2);

   InitLights();

   UpdateWindow(hWnd);

   return TRUE;
}

void InitLight(Light* light, double x, double y)
{
    const UINT displayWidth = 1920;
    const UINT displayHeight = 1080;
    const UINT size = 10;
    UINT displayX = (UINT)(x * displayWidth) - size / 2;
    UINT displayY = (UINT)(y * displayHeight) - size / 2;
    light->x = x;
    light->y = y;
    light->color = RGB(0, 0, 0);
    light->region = new LightRegion(&duplicationManager, displayX, displayY, size, size);
}

void InitLights()
{
    const double border = 0.1;

    UINT counter = 0;
    double x, y;
    Light *light;
    // top lights
    y = border;
    for (UINT i = 0; i < 13; i++)
    {
        light = &lights[counter++];
        x = border + (1 - 2 * border) * ((double)i / 12);
		InitLight(light, x, y);
   }
    // bottom lights
    y = 1 - border;
    for (UINT i = 0; i < 13; i++)
    {
        light = &lights[counter++];
        x = border + (1 - 2 * border) * ((double)i / 12);
		InitLight(light, x, y);
   }
    // left lights
    x = border;
    for (UINT i = 1; i < 8; i++)
    {
        light = &lights[counter++];
        y = border + (1 - 2 * border) * ((double)i / 8);
		InitLight(light, x, y);
   }
    // right lights
    x = 1 - border;
    for (UINT i = 1; i < 8; i++)
    {
        light = &lights[counter++];
        y = border + (1 - 2 * border) * ((double)i / 8);
		InitLight(light, x, y);
   }
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
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RECT boundingBox;
			GetClientRect(hWnd, &boundingBox);
            for (int i = 0; i < 40; i++)
            {
                Light *light = &lights[i];
				LOGBRUSH lbBrushDefinition;
				lbBrushDefinition.lbStyle = BS_SOLID;
                lbBrushDefinition.lbColor = light->color;
                LONG x = (LONG)(light->x * (boundingBox.right - boundingBox.left)) + boundingBox.left;
                LONG y = (LONG)(light->y * (boundingBox.bottom - boundingBox.top)) + boundingBox.top;
                RECT drawBox = RECT{ x - 10, y - 10, x + 10, y + 10 };
				HBRUSH hBrush = CreateBrushIndirect(&lbBrushDefinition);
				FillRect(hdc, &drawBox, hBrush);
				DeleteObject(hBrush);
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
            duplicationManager.UpdateFrame();
            for (int i = 0; i < 40; i++)
            {
                lights[i].color = lights[i].region->getColor();
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

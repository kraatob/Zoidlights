// Zoidlights.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Zoidlights.h"
#include "DesktopDuplication.h"
#include "LightRegion.h"
#include "Constants.h"

// Global Variables:
HINSTANCE hInst;
Device device;
DesktopDuplication desktopDuplication(&device);
LightRegion* lightRegions[4];
Light* lights;
UINT lightCount;


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

   device.Init();

   desktopDuplication.StartDuplication(0);

   InitLights();

   UpdateWindow(hWnd);

   return TRUE;
}

void InitLights()
{
    Size dimensions = desktopDuplication.GetDimensions();
    double aspectRatio = (double)dimensions.cx / (double)dimensions.cy;
    UINT sampleDistance = (UINT)(BORDER_SIZE_OF_X * dimensions.cx - 1) / (SAMPLES - 1);
    UINT border = sampleDistance * (SAMPLES - 1) + 1;
    UINT lightCountX = dimensions.cx / sampleDistance;
    UINT lightCountY = dimensions.cy / sampleDistance;
    lightCount = 2 * (lightCountX + lightCountY);
    lights = new Light[lightCount];

    UINT lightCounter = 0;

    // top
    lightRegions[0] = new LightRegion(&device, &desktopDuplication, 0, 0, dimensions.cx, border, FALSE, lightCountX);
    for (UINT i = 0; i < lightCountX; i++) {
        Light *light = &lights[lightCounter++];
        light->color = &lightRegions[0]->m_lights[i];
        light->x = i * (1.0 / (lightCountX - 1));
        light->y = 0;
   }

    // right
    lightRegions[1] = new LightRegion(&device, &desktopDuplication, dimensions.cx - border, 0, border, dimensions.cy, TRUE, lightCountY);
    for (UINT i = 0; i < lightCountY; i++) {
        Light *light = &lights[lightCounter++];
        light->color = &lightRegions[1]->m_lights[i];
        light->x = 1; 
        light->y = i * (1.0 / (lightCountY - 1));
   }

    // bottom
    lightRegions[2] = new LightRegion(&device, &desktopDuplication, 0, dimensions.cy - border, dimensions.cx, border, FALSE, lightCountX);
    for (UINT i = 0; i < lightCountX; i++) {
        Light *light = &lights[lightCounter++];
        light->color = &lightRegions[2]->m_lights[i];
        light->x = i * (1.0 / (lightCountX - 1));
        light->y = 1;
   }

    // left
    lightRegions[3] = new LightRegion(&device, &desktopDuplication, 0, 0, border, dimensions.cy, TRUE, lightCountY);
    for (UINT i = 0; i < lightCountY; i++) {
        Light *light = &lights[lightCounter++];
        light->color = &lightRegions[3]->m_lights[i];
        light->x = 0;
        light->y = i * (1.0 / (lightCountY - 1));
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
            for (int i = 0; i < lightCount; i++)
            {
                Light *light = &lights[i];
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
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        switch (wParam) {
		case IDT_LOOP_TIMER: 
            try {
                desktopDuplication.UpdateFrame();
            } catch (const char* exception) {
                desktopDuplication.StartDuplication(0);
                break;
            }
            for (int i = 0; i < 4; i++) {
                lightRegions[i]->UpdateLights();
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

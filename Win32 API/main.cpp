#include <windows.h>
#include "list_recursive.h"

//name of the window
const char g_szClassName[] = "myWindowClass";

// WndProc (Window Procedure) is called for each message and processing them

/*
LRESULT CALLBACK WindowProc(
_In_ HWND   hwnd, -> handle window to apply the message
_In_ UINT   uMsg, -> msg to process
_In_ WPARAM wParam,
_In_ LPARAM lParam
);
*/

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE: //when press X or Alt + F4
		DestroyWindow(hwnd); /*Send message WM_DESTROY to the window. (case WM_DESTROY)
							 If it would be a child, only one would be killed..etc*/
		break;
	case WM_DESTROY:
		PostQuitMessage(0); /*
							Exit. Send WM_QUIT to GetMessage, he returns FALSE, the loop GetMessage ends,
							and WinMain returns the last Msg. Msg.wParam;*/
		break;
	case WM_PAINT: //when update de windows to paint, this message are generated
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);


		TextOut(hdc, 5, 5, "**** Listing all files recursively where the exe is **** \n", 59);
		char NPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, NPath);

		
		int x = 10;
		int y = 25;

		//call function
		list_files_recursive(NPath, hdc, &x, &y);
		
		EndPaint(hwnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam); //Defaults actions like maximice,move ...etc
	}
	return 0;
}

//WinMain function is the entry point on windows. WINAPI -> calling convention from _stdcall
/*
HINSTANCE hInstance => handle to the program executable module (.exe file on memory). Used to load modules, or resources
HINSTANCE hPrevInstance => NULL for win32 Programs. Used on win16.
LPSTR lpCmdLine => command line arguments as a single string (without the program name)
int nCmdShow => Integer which may be passed to ShowWindow()*/

/* LPSTR = Long Pointer String*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	WNDCLASSEX wc; //WNDCLASSEX strutc
	HWND hwnd; //Handle use to create de window
	MSG Msg; //Message to return

			 //Step 1: Registering the Window Class, filling out the members of a WNDCLASSEX structure	
	wc.cbSize = sizeof(WNDCLASSEX); //size of struct -> use sizeof to configure it
	wc.style = 0; //class style (* CS)
	wc.lpfnWndProc = WndProc; //Pointer to window procedure for this class
	wc.cbClsExtra = 0; //Amount of extra data to this class
	wc.cbWndExtra = 0; //Amount of extra data to this window type
	wc.hInstance = hInstance; //Handle to application instance 
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //handle to large icon 32x32 (alt + TAB)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //hanble to cursor that will be displayed over our window
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //Color of window
	wc.lpszMenuName = NULL; //Name of menu resource to use
	wc.lpszClassName = g_szClassName; //name to identity de class -> "myWindowClass";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); //handle to the small icon (taskbar and window)

												  //Call Register Class Ex => if return 0 -> error
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//Create de window with the class created

	/*HWND WINAPI CreateWindowEx(
	_In_     DWORD     dwExStyle, -> extended style windows
	_In_opt_ LPCTSTR   lpClassName, -> the class to use to create the window
	_In_opt_ LPCTSTR   lpWindowName, -> the name of the window
	_In_     DWORD     dwStyle, -> style windows
	_In_     int       x, -> coordenate x from top left corner
	_In_     int       y, -> coordenate y from top left corner
	_In_     int       nWidth, -> width of the window
	_In_     int       nHeight, -> height of the window
	_In_opt_ HWND      hWndParent, -> parent handle
	_In_opt_ HMENU     hMenu, -> menu handle
	_In_opt_ HINSTANCE hInstance, -> instance handle
	_In_opt_ LPVOID    lpParam -> pointer to window data creation
	);
	*/

	//handle to de windows created
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"The title of my window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,
		500,
		NULL,
		NULL,
		hInstance,
		NULL);

	//if fails return NULL
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//Show the window. Handle and nCmdShow params
	ShowWindow(hwnd, nCmdShow);

	//Update the window to make sure it has been painted correctly
	UpdateWindow(hwnd);

	/*Get the actions from the user that have been saved on the application's message queue.
	It returns them to processing and remove them from the queue*/

	/*
	BOOL WINAPI GetMessage(
	_Out_    LPMSG lpMsg,
	_In_opt_ HWND  hWnd,
	_In_     UINT  wMsgFilterMin,
	_In_     UINT  wMsgFilterMax
	);
	*/

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		//Make corrections to keyboard actions
		TranslateMessage(&Msg);

		//Send the action to the corresponding window
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}


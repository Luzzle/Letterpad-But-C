//=========================================================================================================================================================//
//	Includes
//=========================================================================================================================================================//
#include <cstdio>    // C Standard IO
#include <Windows.h> // Windows header file
#include <Richedit.h> // Rich TextBox Header
#include <fstream> // IO Stream
#include <string> // String manipulation
#include <vector> // Vector arrays

//=========================================================================================================================================================//
//	Constants
//=========================================================================================================================================================//
const wchar_t WND_CLASS_NAME[] = L"LetterpadWndClass"; // Window Class Name
const wchar_t WND_TITLE[] = L"Letterpad"; // Window Title

const int WND_HEIGHT = 800;
const int WND_WIDTH = 800;

const int TM_FILE_NEW = 0x32;
const int TM_FILE_SAVE = 0x33;
const int TM_FILE_LOAD = 0x34;
const int TM_FILE_EXIT = 0x35;

//=========================================================================================================================================================//
//	Function Declarations
//=========================================================================================================================================================//
void InitWinClass(HINSTANCE hInstance);
HWND InitWindow(HINSTANCE hInstance);
HWND InitTextField(HWND hWindow, HINSTANCE hInstance);
void InitMenu(HWND hWindow, HINSTANCE hInstance);

//=========================================================================================================================================================//
//	WinMain - Application Entry Point
//=========================================================================================================================================================//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow) {
	
	// Initialize Class and Create Window
	InitWinClass(hInstance);
	HWND hWindow = InitWindow(hInstance);
	
	//Error checking
	if (hWindow == NULL) {
		MessageBox(NULL, L"Error Creating Window", L"Error", NULL);
		return -1;
	}
	
	// Display the window
	ShowWindow(hWindow, nCmdShow);

	HWND hTextField = InitTextField(hWindow, hInstance);
	
	InitMenu(hWindow, hInstance);

	MSG msg = { 0 };
	
	while (GetMessage(&msg, hWindow, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//=========================================================================================================================================================//
//	WndProc - Main Window Process (Handles drawing to the window etc...)
//=========================================================================================================================================================//
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		case WM_DESTROY:
		{
			exit(0);
			return 0;
		}	

		case WM_PAINT:
		{
			RECT clientRect;
			PAINTSTRUCT ps = { 0 };
			HDC hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &clientRect);
			FillRect(hdc, &clientRect, (HBRUSH)COLOR_WINDOW + 1);
			EndPaint(hWnd, &ps);
		}

		case WM_COMMAND: 
		{
			HWND hREBox = FindWindowEx(hWnd, NULL, MSFTEDIT_CLASS, NULL);

			switch (wParam) {
				
				case TM_FILE_NEW:
				{
					SetWindowText(hREBox, L"");
					break;
				}
				case TM_FILE_SAVE:
				{
					int len = GetWindowTextLength(hREBox) + 1;
					std::vector<wchar_t> data(len);
					GetWindowText(hREBox, &data[0], len);

					std::string sData(data.begin(), data.end());
					const char* csData = sData.c_str();

					OPENFILENAME ofn;
					WCHAR szFile[MAX_PATH];

					ZeroMemory(&ofn, sizeof(ofn));

					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFile = szFile;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = L"Text (.txt)\0*.txt\0All Files\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					if (GetSaveFileName(&ofn)) {

						std::ofstream file(szFile);
						file.write(csData, len);
						file.close();

						break;
					}

					MessageBox(NULL, L"Error saving File!", L"Error!", NULL);

					break;
				}

				case TM_FILE_LOAD:
				{
					OPENFILENAME ofn;
					WCHAR szFile[MAX_PATH];
					
					ZeroMemory(&ofn, sizeof(ofn));

					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFile = szFile;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = L"Text (.txt)\0*.txt\0All Files\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					if (GetOpenFileName(&ofn)) {
						std::ifstream openedFile;
						openedFile.open(szFile);

						if (!openedFile.is_open()) {
							MessageBox(NULL, L"Error opening File!", L"Error!", NULL);
							break;
						}

						std::string file = std::string((std::istreambuf_iterator<char>(openedFile)), std::istreambuf_iterator<char>());
						const char* data = file.c_str();
						
						if (!SetWindowTextA(hREBox, data)) {
							MessageBox(NULL, L"Error opening File!", L"Error!", NULL);
							break;
						}
						
						break;
						
					}

					MessageBox(NULL, L"Error selecting file!", L"Error!", NULL);
					break;
				}

				case TM_FILE_EXIT:
				{
					DestroyWindow(hWnd);
					exit(0);
				}
			}
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=========================================================================================================================================================//
//	Function Definitions
//=========================================================================================================================================================//
void InitWinClass(HINSTANCE hInstance) {
	WNDCLASS wndClass = { 0 };
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = WND_CLASS_NAME;
	wndClass.hInstance = hInstance;
	
	// Register Window Class
	RegisterClass(&wndClass);
}

HWND InitWindow(HINSTANCE hInstance) {
	return CreateWindowEx(
		NULL,
		WND_CLASS_NAME,
		WND_TITLE,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WND_WIDTH, WND_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
}

HWND InitTextField(HWND hWindow, HINSTANCE hInstance) {
	LoadLibrary(TEXT("Msftedit.dll"));

	HWND rEditBox = CreateWindowEx(
		0,
		MSFTEDIT_CLASS,
		NULL,
		ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,
		0, 0,
		WND_WIDTH - 15, WND_HEIGHT - 40,
		hWindow,
		NULL,
		hInstance,
		NULL
	);

	return rEditBox;

}

void InitMenu(HWND hWindow, HINSTANCE hInstance) {
	HMENU hMenuBar = CreateMenu();
	HMENU hFileMenu = CreateMenu();

	// TopMenu -> File -> Dropdown
	AppendMenuW(hFileMenu, MF_STRING, TM_FILE_NEW, L"New");
	AppendMenuW(hFileMenu, MF_STRING, TM_FILE_SAVE, L"Save");
	AppendMenuW(hFileMenu, MF_STRING, TM_FILE_LOAD, L"Load");
	AppendMenuW(hFileMenu, MF_STRING, TM_FILE_EXIT, L"Exit");

	AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	AppendMenuW(hMenuBar, MF_STRING, 2, L"Edit");
	AppendMenuW(hMenuBar, MF_STRING, 3, L"Format");

	

	SetMenu(hWindow, hMenuBar);
}

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

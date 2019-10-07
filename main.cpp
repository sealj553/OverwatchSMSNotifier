//pretty buttons
#pragma comment(linker,"\"/manifestdependency:type='win32' \
						name = 'Microsoft.Windows.Common-Controls' \
						version = '6.0.0.0' processorArchitecture = '*' \
						publicKeyToken = '6595b64144ccf1df' language = '*'\"")

#include <windows.h>
#include <streambuf>
#include <fstream>
#include <iostream>
#include <thread>

#include "detector.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using std::cout;
using std::endl;

//forward declarations
void buttonPressed();
void startDetection();
void stopDetection();
void RedirectIO();
void ResetIO();

//some basic parameters
const int width = 250;
const int height = 130;
const int winX = 100;
const int winY = 100;
bool started = 0;

enum { ID_START, ID_TEXT, ID_THRESH, ID_CONSOLE, ID_RUNONCE };

//globals
std::streambuf *CinBuffer, *CoutBuffer, *CerrBuffer;
std::fstream ConsoleInput, ConsoleOutput, ConsoleError;

HWND main_window, start_btn, status_txt, debug_chk, console_chk, continuous_chk;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}

void stopped_callback() {
	SendMessage(start_btn, WM_LBUTTONDOWN, 0, ID_START);
	SendMessage(start_btn, WM_LBUTTONUP, 0, ID_START);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
	detectorInit();
	std::thread detector_thread(waitForEvent);
	detector_thread.detach();

	MSG  msg;
	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"Notifier";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClassW(&wc);

	main_window = CreateWindowEx(
		WS_EX_WINDOWEDGE, "Notifier", "Notifier",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		winX, winY, width, height, 0, 0, hInstance, 0);

	EnumChildWindows(main_window, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

void createUI(HWND hwnd) {
	status_txt = CreateWindowEx(WS_EX_WINDOWEDGE, "Static", "Stopped",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		25, 20, 300, 230,
		hwnd, (HMENU)ID_TEXT, NULL, NULL);

	start_btn = CreateWindowEx(WS_EX_WINDOWEDGE, "Button", "Start",
		WS_VISIBLE | WS_CHILD,
		20, 50, 80, 25,
		hwnd, (HMENU)ID_START, NULL, NULL);

	debug_chk = CreateWindowEx(NULL, "Button", "Show threshold",
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		120, 0, 200, 30,
		hwnd, (HMENU)ID_THRESH, NULL, NULL);

	console_chk = CreateWindowEx(NULL, "Button", "Show console",
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		120, 30, 200, 30,
		hwnd, (HMENU)ID_CONSOLE, NULL, NULL);

	console_chk = CreateWindowEx(NULL, "Button", "Stop after triggered",
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		120, 60, 200, 30,
		hwnd, (HMENU)ID_RUNONCE, NULL, NULL);
}

void startDetection() {
	running = true;
	cout << "started" << endl;
}

void stopDetection() {
	running = false;
	cv::destroyAllWindows();
	cout << "stopped" << endl;
}

void buttonPressed() {
	started = !started;
	if (started) {
		//running now
		SendMessage(start_btn, WM_SETTEXT, 0, (LPARAM)"Stop");
		SendMessage(status_txt, WM_SETTEXT, 0, (LPARAM)"Watching...");
		startDetection();
	} else {
		//stopping
		SendMessage(start_btn, WM_SETTEXT, 0, (LPARAM)"Start");
		SendMessage(status_txt, WM_SETTEXT, 0, (LPARAM)"Stopped");
		stopDetection();
	}
	InvalidateRect(main_window, NULL, FALSE);   // invalidate whole window
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		createUI(hwnd);
		break;
	
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_START:
			buttonPressed();
			break;
		case ID_THRESH:
			show_screen = IsDlgButtonChecked(hwnd, ID_THRESH); 
			cv::destroyAllWindows();
			break;
		case ID_RUNONCE:
			run_once = IsDlgButtonChecked(hwnd, ID_RUNONCE); 
			break;
		case ID_CONSOLE:
			if (IsDlgButtonChecked(hwnd, ID_CONSOLE)) {
				AllocConsole();
				RedirectIO();
			} else {
				ResetIO();
				FreeConsole();
			}
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void RedirectIO() {
	CinBuffer = std::cin.rdbuf();
	CoutBuffer = std::cout.rdbuf();
	CerrBuffer = std::cerr.rdbuf();
	ConsoleInput.open("CONIN$", std::ios::in);
	ConsoleOutput.open("CONOUT$", std::ios::out);
	ConsoleError.open("CONOUT$", std::ios::out);
	std::cin.rdbuf(ConsoleInput.rdbuf());
	std::cout.rdbuf(ConsoleOutput.rdbuf());
	std::cerr.rdbuf(ConsoleError.rdbuf());
}

void ResetIO() {
	ConsoleInput.close();
	ConsoleOutput.close();
	ConsoleError.close();
	std::cin.rdbuf(CinBuffer);
	std::cout.rdbuf(CoutBuffer);
	std::cerr.rdbuf(CerrBuffer);
	CinBuffer = NULL;
	CoutBuffer = NULL;
	CerrBuffer = NULL;
}

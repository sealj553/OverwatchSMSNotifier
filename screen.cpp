#include "screen.h"

using namespace std;
using namespace cv;

Screen::Screen() {
	hwnd = GetDesktopWindow();

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	height = windowsize.bottom;  //change this to whatever size you want to resize to
	width = windowsize.right;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	//http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	src.create(height, width, CV_8UC4);
}

Mat Screen::getMat() {
	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	BitBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, SRCCOPY);
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}
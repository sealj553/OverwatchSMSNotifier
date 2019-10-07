#pragma once

#include <opencv2/opencv.hpp>
#include <Windows.h>

class Screen {
public:
	Screen();
	cv::Mat getMat();
private:
	HWND hwnd;
	int width, height;
	BITMAPINFOHEADER  bi;
	cv::Mat src;
	HDC hwindowDC;
	HDC hwindowCompatibleDC;
	HBITMAP hbwindow;
};
#include "detector.h"
#include "screen.h"
#include "sms.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>

using std::cout;
using std::endl;
using namespace cv;

Screen scr;
float lastPercent;
Rect box(Point(809, 44), Point(1110, 130));
Scalar c_min(101, 70, 84);
Scalar c_max(115, 140, 120);
float thresh = 0.7f;
Mat image;

volatile bool running = false;
volatile bool show_screen = false;
volatile bool run_once = false;
extern void stopped_callback();

void detectorInit() {
	sms_init();
	//...
}

void waitForEvent(){
	lastPercent = 0;
	running = false;

	while (1) {
		while (running) {
			image = scr.getMat();
			//Mat image = imread("found.bmp", IMREAD_COLOR);
			image = image(box);

			Mat conv;
			cvtColor(image, conv, COLOR_BGR2HSV);
			inRange(conv, c_min, c_max, conv);

			int pixelsInRange = countNonZero(conv);
			int totalPixels = conv.rows * conv.cols;
			float percent = (float)pixelsInRange / totalPixels;

			if (lastPercent > thresh && percent < thresh) {
				cout << "game starting" << endl;
				sms_send();
				if (run_once) {
					running = false;
					stopped_callback();
				}
			}
			lastPercent = percent;
			//printf("%f\n", percent);

			if (show_screen) {
				//Mat thresh(conv.rows, conv.cols, CV_8UC3, 0);

				cvtColor(conv, conv, COLOR_GRAY2BGR);
				putText(conv,
					std::to_string(percent) + "%",
					Point(0, 12), FONT_HERSHEY_SIMPLEX, 0.5f,
					Scalar(255, 64, 0));

				imshow("Screen", image);
				imshow("Threshold", conv);
			}
			waitKey(5);
		}
		waitKey(100);
	}
}

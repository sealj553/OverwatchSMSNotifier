#pragma once

void detectorInit();
void waitForEvent();

extern volatile bool running;
extern volatile bool show_screen;
extern volatile bool run_once;

#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>

class Window
{
private:
	int width = 640, height = 480;

	HWND window;
public:
	bool InitWindow(HINSTANCE hInstance);

	// Getters
	HWND getWindow() { return this->window; };
	int getWidth() { return this->width; };
	int getHeight() { return this->height; };
};

#endif
#ifndef gui_element_h
#define gui_element_h 1

#include <stdint.h>

struct _AppleGui {
	int x;
	int y;
	int width;
	int height;
	int depth;
	int xres;
	int yres;
	double rmin;
	double rmax;
	double imin;
	double imax;
};

void gui_init(int x, int y, struct _AppleGui *appleGui, uint8_t ***fbBuf, void *fbFunc);
void updateWerte();

#endif

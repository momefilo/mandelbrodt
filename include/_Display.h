#ifndef _Display_h
#define _Display_h 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <pthread.h>
#include <sys/mman.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

class _Display{
	
private:
	uint8_t *fbpointer;
	uint8_t *fbspiegel;
	int fbfile;
public:
	int xres;
	int yres;
	int linelenght;
	int bufferlenght;
	int bpp;
	_Display();
	~_Display();
	void putSpiegel(int x, int y, int color);
	void putDisplay(int x, int y, int color);
	void drawSpiegel(int offset, int lenght);
};

#endif

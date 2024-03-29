// momefilo Desing
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
#include <cstring>// c++ Com
#include <stdexcept>// c++ Com
#include <vector>

class Display{
	
private:
	uint8_t *fbpointer{0};
	std::vector<std::uint8_t> fbspiegel;
//	uint8_t *fbspiegel{0};
	int fbfile;
public:
	int xres;
	int yres;
	int linelenght;
	int bufferlenght;
	int bpp;
	Display();
	~Display();
	void putSpiegel(int x, int y, int color);
	void putDisplay(int x, int y, int color);
	void drawSpiegel(int offset, int lenght);
};

#endif

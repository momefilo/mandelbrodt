#ifndef _AppleMemory_h
#define _AppleMemory_h 1
#include "_Colorinterface.h"

class AppleMemory{
	
private:
	int picWidth{80};
	int picHeight{45};

public:
	int count{0};
	int xpos{0};
	int ypos{0};
	Colorinterface *ci;
	AppleMemory(int x, int y, Colorinterface &_ci);
	void saveApple();
	void drawPic(std::string filename, int myy);
};

#endif

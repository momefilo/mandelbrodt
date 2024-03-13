#ifndef _AppleMemory_h
#define _AppleMemory_h 1
#include "_Colorinterface.h"
struct MemApple{
	struct ApplePara paras;
	int pic[100][45];
};
class AppleMemory{
	
private:
	int picWidth{100};
	int picHeight{45};
	int rightBorder{20};
	int topBorder{50};
	int satz, satzcount, satzakt, satzrest;
public:
	int xpos{0};
	int ypos{0};
	int width{100};
	int height{0};
	int count{0};
	Colorinterface *ci;
	std::vector<MemApple> memApples;
	
	AppleMemory(int x, int y, Colorinterface &_ci);
	void saveApple();
	void drawApples();
};

#endif

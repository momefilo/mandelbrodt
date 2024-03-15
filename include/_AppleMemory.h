#ifndef _AppleMemory_h
#define _AppleMemory_h 1
#include "_Colorinterface.h"
struct MemApple{
	struct ApplePara paras;
	int pic[100][45];
	std::vector<struct _CiElement> ci_elements;
};
class AppleMemory{
	
private:
	int picWidth{100};
	int picHeight{45};
	int rightBorder{20};
	int topBorder{50};
	int satz, satzcount, satzakt, satzrest;
	void initMemory();
	void delApple(int id);
	void loadApple(int id);
public:
	int xpos{0};
	int ypos{0};
	int width{100};
	int height{0};
	Colorinterface *ci;
	std::vector<MemApple> memApples;
	
	AppleMemory(int x, int y, Colorinterface &_ci);
	void onMouseOver(int x, int y, int taste);
	void writeToFile();
	void saveApple();
	void drawApple(int id, int dpos);
	void showSatz(int _satz);
};

#endif

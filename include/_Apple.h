// momefilo Desing
#ifndef _Apple_h
#define _Apple_h 1
#include "_Userinterface.h"
#include <memory>
#include <algorithm>

class Apple{
private:
	int Startpoint[2]{0,0};
	int Endpoint[2]{0,0};
	void allocMatrizen(int xres, int yres);
	void freeMatrizen(int xres);
public:
	Userinterface *ui;
	ApplePara paras;
	int **matrix = nullptr;
	int **colormatrix = nullptr;
	
	std::vector<std::array<int, 2>> iterMembers;
	std::vector<int> oneMembers, tenMembers;
	Apple(Userinterface &_ui, ApplePara data);
	~Apple();
	void paint();
	void calc();
	void sort();
	void clearScreen();
	void setDepth(int tiefe);
	int getDepth();
	void onMouseOver(int x, int y, int taste);
	ApplePara getPara(){return paras;};
	void init(ApplePara data, bool _sort);
};


#endif

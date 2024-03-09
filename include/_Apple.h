#ifndef _Apple_h
#define _Apple_h 1
#include "_Userinterface.h"

class Apple{
private:
	int Startpoint[2]{0,0};
	int Endpoint[2]{0,0};
public:
	Userinterface *ui;
	ApplePara paras;
	int **matrix;
	int **colormatrix;
	int **iterMembers, *oneMembers, *tenMembers, countsOfIter;
	Apple(Userinterface &_ui);
	~Apple();
	void paint();
	void calc();
	void sort();
	void clearScreen();
	void onMouseOver(int x, int y, int taste);
	void setPara(ApplePara data){this->paras = data;};
	ApplePara getPara(){return paras;};
	void init();
	void init(int _xres, 
					int _yres, 
					long double rmin,
					long double rmax,
					long double imin,
					long double imax);
};


#endif

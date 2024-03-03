#ifndef _Apple_h
#define _Apple_h 1
#include "_Userinterface.h"
class _Apple{
private:
	int **matrix;
	int **colormatrix;
	int Startpoint[2]{0,0};
	int Endpoint[2]{0,0};
public:
	_Userinterface *ui;
	int xpos;
	int ypos;
	int width;
	int height;
	int xres{1920};
	int yres{1080};
	long double rmin{-1};
	long double rmax{2};
	long double imin{-1};
	long double imax{1};
	int depth{100};
	_Apple(_Userinterface &_ui);
	~_Apple();
	void paint();
	void calc();
	void sort();
	void clearScreen();
	void onMouseOver(int x, int y, int taste);
	void init();
	void init(int _xres, 
					int _yres, 
					long double rmin,
					long double rmax,
					long double imin,
					long double imax);
};


#endif

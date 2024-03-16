// momefilo Desing
#ifndef _Userinterface_h
#define _Userinterface_h 1
#include "_Display.h"

#define Reglerheight 150

struct ApplePara{
	int xpos{};
	int ypos{};
	int width{};
	int height{};
	int xres{1920};
	int yres{1080};
	long double rmin{-1};
	long double rmax{2};
	long double imin{-1};
	long double imax{1};
	int depth{100};
};

struct Element{
	int x{};
	int y{};
	long double wert{0.0L};
};

class Userinterface{
private:
	void write_font16x16(int x, int y, char zeichen, int fgcolor, int bgcolor);
	void write_font8x8(int x, int y, char zeichen, int fgcolor, int bgcolor);
	std::vector<Element> elements;
	int elem_width{132};
	int elem_height{66};
	int elem_count{7};
public:
	Display *display;
	int xpos;
	int ypos;
	int width;
	int height;
	Userinterface(int x, int y, Display &_display, std::function<void(int)> _callback);
	std::function<void(int)> callback;
	void writeText(int x, int y, char *text, int len, int fgcolor, int bgcolor, uint8_t size, bool update);
	void textComplex(double r, double i);
	void textFertig(bool fertig);
	void drawGraphic();
	void drawRect(int xstart, int ystart, int xend, int yend, int color);
	void updateWert(int element);
	void updateWerte();
	void setParas(ApplePara para);
	void setWert(int id, long double wert);
	long double getWert(int id);
	void onMouseOver(int x, int y, int taste);
};

#endif

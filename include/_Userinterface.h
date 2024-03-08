#ifndef _Userinterface_h
#define _Userinterface_h 1
#include "_Display.h"

#define Reglerheight 150
struct _Element{
	int x{};
	int y{};
	long double wert{0.0L};
};

class _Userinterface{
private:
	void write_font16x16(int x, int y, char zeichen, int fgcolor, int bgcolor);
	void write_font8x8(int x, int y, char zeichen, int fgcolor, int bgcolor);
	std::vector<struct _Element> elements;
	int elem_width{132};
	int elem_height{66};
	int elem_count{7};
public:
	Display *display;
	int xpos;
	int ypos;
	int width;
	int height;
	//_Userinterface(int x, int y, std::function<void(int)> _callback);
	_Userinterface(int x, int y, Display &_display, std::function<void(int)> _callback);
	~_Userinterface();
	std::function<void(int)> callback;
	void writeText(int x, int y, char *text, int len, int fgcolor, int bgcolor, uint8_t size, bool update);
	void textComplex(double r, double i);
	void textFertig(bool fertig);
	void drawGraphic();
	void drawRect(int xstart, int ystart, int xend, int yend, int color);
	void updateWert(int element);
	void updateWerte();
	void setWert(int id, long double wert);
	long double getWert(int id);
	void onMouseOver(int x, int y, int taste);
};

#endif

#ifndef _Colorinterface_h
#define _Colorinterface_h 1
#include "_Apple.h"

struct _farbe{ int color; int id;};

struct _CiElement{
	int x{};
	int y{};
	int color{0x00000000};
	int members{1};
	int id{};
	bool verlauf{false};
};

class Colorinterface{
private:
	int elem_width{66};
	int elem_height{Reglerheight};
	int text_height{30};
	int border{66};
	int satz, satzcount, satzakt, satzrest;
public:
	Apple *apple;
	std::vector<struct _CiElement> elements;
	std::vector<struct _farbe> gradient;
	int xpos;
	int ypos;
	int width;
	int height;
	Colorinterface(int x, int y, Apple &myApple, std::function<void(int)> _callback);
//	~Colorinterface();
	void drawElem(int apos, int dpos);
	void addElements();
	void showSatz(int id);
	int getColor(int id);
	void onMouseOver(int x, int y, int taste);
	void drawVerlauf();
	void makeVerlauf();
};

#endif

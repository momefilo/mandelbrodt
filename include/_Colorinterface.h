#ifndef _Colorinterface_h
#define _Colorinterface_h 1
#include "_Apple.h"
#include <string>
#include <vector>

struct _CiElement{
	int x;
	int y;
	int color{0x00000000};
	int id;
};

class _Colorinterface{
private:
	std::vector<struct _CiElement> elements;
	int elem_width{66};
	int elem_height{Reglerheight};
	int text_height{30};
	int border{30};
	int satz, satzcount, satzakt, satzrest;
public:
	_Apple *Apple;
	int xpos;
	int ypos;
	int width;
	int height;
	_Colorinterface(int x, int y, _Apple &myApple, std::function<void(int)> _callback);
	~_Colorinterface();
	void drawElem(int apos, int dpos);
	void addElements();
	void showSatz(int id);
	int getColor(int id);
	void onMouseOver(int x, int y, int taste);
};

#endif

#include "graphics/font8x8.h"
#include "graphics/font16x16.h"
#include "_Userinterface.h"

Userinterface::Userinterface(int x, int y, Display &_display, std::function<void(int)> _callback){
	callback = _callback;
	xpos = x;
	ypos = y;
	display = &_display;
	for(int i=0; i<elem_count; i++){
		Element element;
		element.x = xpos;
		if(i % 2 != 0) element.x = elem_width + xpos;
		element.y = (i / 2) * elem_height + ypos;
		elements.push_back(element);
		elements.at(i).wert = 0;
	}
	width = 2*elem_width;
	height = 4*elem_height;
	drawGraphic();
	updateWerte();
}

void Userinterface::setParas(ApplePara para){
	setWert(0, para.xres);
	setWert(1, para.yres);
	setWert(2, para.rmin);
	setWert(3, para.rmax);
	setWert(4, para.imin);
	setWert(5, para.imax);
	setWert(6, para.depth);
}

void Userinterface::drawRect(int xstart, int ystart, int xend, int yend, int color){
	
	if(xend < xstart){
		int tmp = xstart;
		xstart = xend;
		xend = tmp;
	}
	if(yend < ystart){
		int tmp = ystart;
		ystart = yend;
		yend = tmp;
	}
	//obere Linie
	for(int x=xstart; x<xend; x++)display->putDisplay(x, ystart, color);
	//untere Linie
	for(int x=xstart; x<xend; x++)display->putDisplay(x, yend, color);
	//linke Linie
	for(int y=ystart; y<yend; y++)display->putDisplay(xstart, y, color);
	//rechte Linie
	for(int y=ystart; y<yend; y++)display->putDisplay(xend, y, color);
}

void Userinterface::drawGraphic(){
	FILE *efile = fopen("include/graphics/element.data","rb");
	if( !efile){
		throw std::runtime_error{
			std::string{ "Failed to open include/graphics/element.data " }
			+ std::strerror(errno)
		};
	}
	for(int i=0; i<elem_count; i++){
		int inRow = 1;
		fseek(efile,0,SEEK_SET);
		for(int x=elements[i].x; x<elements[i].x + elem_width; x++){
			for(int y=elements[i].y; y<elements[i].y + elem_height; y++){
				uint8_t farbe[3];
				fread(farbe, sizeof(uint8_t),3,efile);
				int color = farbe[0]<<16 | farbe[1]<<8 | farbe[2];
				display->putSpiegel(x, y, color);
			}
		}
	}
	fclose(efile);
	int fgfarbe = 0x00000000;//255<<16 | 200<<8 | 0;
	int bgfarbe = 96<<16 | 96<<8 | 96;
	char texte[7][10]={"  X-Res ","  Y-Res ","  r-Min ","  r-Max ","  i-Min ","  i-Max ","  Depth "};
	for(int h=0; h<7; h++)
		writeText(elements[h].x, elements[h].y, texte[h], 8, fgfarbe, bgfarbe,16, true);
		
	FILE *bfile = fopen("include/graphics/button_1.data","rb");
	if( !bfile){
		throw std::runtime_error{
			std::string{ "Failed to open include/graphics/button_1.data " }
			+ std::strerror(errno)
		};
	}
	int startx = elements[elem_count-1].x + elem_width;
	int starty = elements[elem_count-1].y;
	for(int x=startx; x<startx+ elem_width; x++){
		for(int y=elements[elem_count-1].y; y<elements[elem_count-1].y + elem_height; y++){
			uint8_t farbe[3];
			fread(farbe, sizeof(uint8_t),3,bfile);
			int color = farbe[0]<<16 | farbe[1]<<8 | farbe[2];
			display->putSpiegel(x, y, color);
		}
	}
	fclose(bfile);
	display->drawSpiegel(0,display->bufferlenght);
}
// zeichen < 128
void Userinterface::write_font8x8(int x, int y, char zeichen, int fgcolor, int bgcolor){
	int myy = 0;
	for(int i=0; i<8; i=i+1){ // 8 Byte umfasst ein 8x8Bit Zeichen
		int myx = 0;
		for(int k=7; k>=0; k=k-1){ // Acht Bits pro Byte
			if(FONT8x8[zeichen][i] & (0x80 >> k)){ // ist das Bit gesetzt
				display->putSpiegel(x + myx, y + myy, fgcolor);
			}
			else{ // Das Bit ist nicht gesetzt
				display->putSpiegel(x + myx, y + myy, bgcolor);
			}
			myx++;
		}
		myy++;
	}
}
void Userinterface::write_font16x16(int x, int y, char zeichen, int fgcolor, int bgcolor){
	int myx = 0, myy = 0;
	for(int i=0; i<32; i=i+1){ // 32 Byte umfasst ein 16x16Bit Zeichen
		for(int k=0; k<8; k=k+1){ // Acht Bits pro Byte
			if(FONT16x16[zeichen*32+i] & (0x80 >> k)){ // ist das Bit gesetzt
				display->putSpiegel(x + myx, y + myy, fgcolor);
			}
			else{ // Das Bit ist nicht gesetzt
				display->putSpiegel(x + myx, y + myy, bgcolor);
			}
			myx++;
		}
		if((i+1) % 2 == 0){
			myy++;
			myx = 0;
		}
	}
}
void Userinterface::writeText(int x, int y, char *text, int len, int fgcolor, int bgcolor, uint8_t size, bool update){
	std::cout<<"Start writeText\n";
	if(size == 8)
		for(int i=0; i<len; i++) write_font8x8(x+i*8, y, text[i], fgcolor, bgcolor);
	else if(size == 16)
		for(int i=0; i<len; i++) write_font16x16(x+i*16, y, text[i], fgcolor, bgcolor);
	//update fb
	if(update)
		std::cout<<"in writeText\n";
		display->drawSpiegel(0,display->bufferlenght);
}
void Userinterface::textComplex(double r, double i){// sprintf
	char text[2][30];
	int fgcolor = 0x00FFFFFF;
	int bgcolor = 0x00000000;
	sprintf(text[0], "r = %.24f", r);
	sprintf(text[1], "i = %.24f", i);
	writeText(2, height, text[0], 30, fgcolor, bgcolor,8,false);
	writeText(2, height + 8, text[1], 30, fgcolor, bgcolor,8,false);
}
void Userinterface::textFertig(bool fertig){// sprintf
	std::cout<<"Start textFertig\n";
	char textR[7] = "Rechne";
	char textF[7] = "Fertig";
	int fgcolor = 0x00FF0000;
	int bgcolor = 0x00000000;
	if(fertig){
		fgcolor = 0x0000FF00;
		std::cout<<"in textFertig if\n";
		writeText(2, height + 20, textF, 6, fgcolor, bgcolor, 16, true);
		//sprintf(text, "Fertig");
	}
	else{std::cout<<"End textFertig else\n";writeText(2, height + 20, textR, 6, fgcolor, bgcolor, 16, true);}
//	writeText(2, height + 20, text, 6, fgcolor, bgcolor, 16, true);
}

void Userinterface::updateWert(int element){// sprintf
	char text[8];
	if(element < 2 || element > 5) sprintf(text, "% 8d", (int)elements.at(element).wert);
	else sprintf(text, "%.5f", (float)elements.at(element).wert);
	int fgcolor = 200<<16 | 255<<8 | 255;
	int bgcolor = 96<<16 | 96<<8 | 96;
	writeText(elements.at(element).x+2, elements.at(element).y+35, text, 8, fgcolor, bgcolor, 16,true);
}
void Userinterface::updateWerte(){
	for(int i=0; i<elem_count; i++) updateWert(i);
}
void Userinterface::setWert(int id, long double wert){
	elements.at(id).wert = wert;
	updateWert(id);
}
long double Userinterface::getWert(int id){return elements.at(id).wert;}

void Userinterface::onMouseOver(int x, int y, int taste){
	
	//get element-id (0 bis ElemCount-1. Buttons = -2)
	int elem_id = -1;
	if(y > ypos && y < elements.at(2).y){
		if(x < elements.at(1).x) elem_id = 0; else elem_id = 1;}
	else if(y > elements.at(2).y && y < elements.at(4).y){
		if(x < elements.at(3).x) elem_id = 2; else elem_id = 3;}
	else if(y > elements.at(4).y && y < elements.at(6).y){
		if(x < elements.at(5).x) elem_id = 4; else elem_id = 5;}
	else if(y > elements.at(6).y && y < elements.at(6).y+elem_height){
		if(x < elements.at(6).x+elem_width) elem_id = 6; else elem_id = -2;}

	//get plus or minus and summand
	int minus = -1;
	int sum = -1;
	if(elem_id > -1){
		if(y > (elements.at(elem_id).y + 18) 
			&& y < (elements.at(elem_id).y + 18 + 14)) minus = 0;
		else if(y > (elements.at(elem_id).y + 51)
			&& y < (elements.at(elem_id).y + 51 + 14)) minus = 1;
		
		if(x > (elements.at(elem_id).x + 2)
			&& x < (elements.at(elem_id).x + 34)) sum = 1;
		else if(x > (elements.at(elem_id).x + 34)
			&& x < (elements.at(elem_id).x + 66)) sum = 100;
		else if(x > (elements.at(elem_id).x + 66)
			&& x < (elements.at(elem_id).x + 98)) sum = 1000;
		else if(x > (elements.at(elem_id).x + 98)
			&& x < (elements.at(elem_id).x + 130)) sum = 10000;
	}
	else if(elem_id < -1){// the Button-Element
		if(y > (elements.at(elem_count-1).y + 2) 
			&& y < (elements.at(elem_count-1).y + 33)) minus = 0;
		else if(y > (elements.at(elem_count-1).y + 33)
			&& y < (elements.at(elem_count-1).y + 64)) minus = 1;
		if(x > (elements.at(elem_count-1).x+132 + 2)
			&& x < (elements.at(elem_count-1).x+132 + 45)) sum = 0;
		else if(x > (elements.at(elem_count-1).x+132 + 45)
			&& x < (elements.at(elem_count-1).x+132 + 87)) sum = 1;
		else if(x > (elements.at(elem_count-1).x+132 + 87)
			&& x < (elements.at(elem_count-1).x+132 + 128)) sum = 2;
	}
	if(taste == 1 && elem_id < -1 && minus > -1 && sum > -1){ //klick Buttons-Element
		if(minus == 0){
			switch (sum) {
				case 0: callback(0); break;
				case 1: callback(1); break;
				case 2: callback(2); break;
			}
		}
		else{
			switch (sum) {
				case 0: callback(3); break;
				case 1: callback(4); break;
				case 2: exit(0); break;
			}
		}
	}
	else if(taste == 1 && elem_id > -1 && minus > -1 && sum > -1){ // Analogwerte
		//xres und yres duerfen nur in Zehnerschritten verändert werden
		if(elem_id == 0 || elem_id == 1)
			if(sum > 0 && sum < 10) sum = 10;
		//real- und imaginaer- Anteil duerfen nur in einser-Schritten verändert werden
		if(elem_id > 1 && elem_id <6 && sum > 1) sum = 1;
		if(minus > 0) sum = sum * -1;
		elements.at(elem_id).wert = elements.at(elem_id).wert +sum;
		int maxwidth = 10*(display->xres-2*elem_width);
		int maxheight = 10*(display->yres-Reglerheight);
		if(elements.at(0).wert < 10) elements.at(0).wert = 10;
		if(elements.at(1).wert < 10) elements.at(1).wert = 10;
		if(elements.at(0).wert > maxwidth) elements.at(0).wert = maxwidth;
		if(elements.at(1).wert > maxheight) elements.at(1).wert = maxheight;
		if(elements.at(6).wert <1 ) elements.at(6).wert = 1;
		if(elements.at(6).wert >999999 ) elements.at(6).wert = 999999; //Breichsüberschreitung
		updateWert(elem_id);
	}
}

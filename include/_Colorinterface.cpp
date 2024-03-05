#include "_Colorinterface.h"

std::function<void(int)> ci_callback;

void _Colorinterface::showSatz(int id){
	for(int x=xpos+border; x<xpos+border + width; x++)
		for(int y=ypos; y<ypos+elem_height; y++)
			Apple->ui->display->putSpiegel(x, y, 0x00000000);
	
	if(id<satzcount){
		int z = 0;
		for(int i=id*satz; i<id*satz + satz; i++){
			drawElem(i, z);
			z++;
		}
	}
	else{
		int z = 0;
		for(int i=id*satz; i<id*satz + satzrest; i++){
			drawElem(i, z);
			z++;
		}
	}
}

_Colorinterface::_Colorinterface(int x, int y, _Apple &myApple, std::function<void(int)> _callback){
	Apple = &myApple;
	ci_callback = _callback;
	xpos = x;
	ypos = y;
	width = Apple->ui->display->xres - border;
	satz = (width-border) / elem_width;
}
_Colorinterface::~_Colorinterface(){}

void _Colorinterface::drawElem(int apos, int dpos){
	uint8_t color[3];
	color[0] = (elements.at(apos).color & 0x00FF0000) >> 16;
	color[1] = (elements.at(apos).color & 0x0000FF00) >> 8;
	color[2] = (elements.at(apos).color & 0x000000FF);
	int yred = elem_height - (float)(elem_height - text_height) / 255 * color[0];
	int ygrn = elem_height - (float)(elem_height - text_height) / 255 * color[1];
	int yblu = elem_height - (float)(elem_height - text_height) / 255 * color[2];
	elements.at(apos).x = (xpos + border + dpos * elem_width);
	elements.at(apos).y = ypos;
	for(int x=0; x<elem_width; x++){
		for(int y=0; y<elem_height; y++){
			int fx = x + (xpos + border + dpos * elem_width);
			int fy = y + ypos;
			if(x>2 && x<elem_width-2 && y>text_height/2+2 && y<text_height-1)
				Apple->ui->display->putSpiegel(fx,fy, elements.at(apos).color);
			else if(x>2 && x<(elem_width-4)/3 && y>text_height && y>yred)
				Apple->ui->display->putSpiegel(fx,fy, 0x00FF0000);
			else if(x>(elem_width-4)/3-1 && x<(elem_width-4)/3*2 && y>text_height && y>ygrn)
				Apple->ui->display->putSpiegel(fx,fy, 0x0000FF00);
			else if(x>(elem_width-4)/3*2-1 && x<elem_width-2 && y>text_height && y>yblu)
				Apple->ui->display->putSpiegel(fx,fy, 0x000000FF);
			else Apple->ui->display->putSpiegel(fx,fy, 0x00808080);
		}
	}
	char text[8];
	sprintf(text,"%d",elements.at(apos).id);
	int fg = 0x00000000;
	int bg = 0x00808080;
	Apple->ui->writeText(elements.at(apos).x+1, elements.at(apos).y+2, text, 6, fg, bg, 8,true);
	Apple->ui->display->drawSpiegel(0, Apple->ui->display->bufferlenght);
}

void _Colorinterface::addElements(){
	elements.erase(elements.begin(),elements.end());
	for(int i=0; i<Apple->countsOfIter; i++){
		_CiElement elem;
		elem.id = Apple->iterMembers[i][0];
		double div = (255.0/Apple->depth) * elem.id;
		elem.color = (int)div<<16 | (int)div<<8 | (int)div;
		elements.push_back(elem);
	}
	satzcount = Apple->countsOfIter / satz;
	satzakt = 0;
	satzrest = 0;
	if(satzcount == 0) satzrest = satz;
	else satzrest = Apple->countsOfIter - satzcount * satz;
}
int _Colorinterface::getColor(int id){return 0;}

void _Colorinterface::onMouseOver(int x, int y, int taste){
	if(x<border/2 && taste==1){ if(satzakt>0){ --satzakt; showSatz(satzakt);}}
	else if(x<border && taste==1){ if(satzakt<satzcount){ ++satzakt; showSatz(satzakt);}}
	
	int elem_id = -1;
	int color = 0; //0=blue, 1=green, 2=red
	if(x>border) 
		elem_id = (x-border)/elem_width + satzakt * satz;
	if(elem_id > -1){
		if( (x) - elements.at(elem_id).x < (elem_width-4)/3) 
			color = 2;
		else if( (x) - elements.at(elem_id).x < (elem_width-4)/3*2) 
			color = 1;
		if(y>ypos+text_height){
			int yrel = (elem_height - text_height) - (y-(ypos+text_height));
			int wert = yrel * (255.0/(elem_height - text_height));
			if(taste==1 && elem_id>-1){
				int mask = ~(0x00000000 | (0xFF << (color * 8)));
				int farbe = elements.at(elem_id).color & mask;
				farbe = farbe | (wert << (color * 8));
				elements.at(elem_id).color = farbe;
				drawElem(elem_id, elem_id - satzakt * satz);
			}
		}
	}
}

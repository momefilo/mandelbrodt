#include "_Colorinterface.h"

std::function<void(int)> ci_callback;

_Colorinterface::_Colorinterface(int x, int y, _Apple &myApple, std::function<void(int)> _callback){
	Apple = &myApple;
	ci_callback = _callback;
	xpos = x;
	ypos = y;
	width = Apple->ui->display->xres - border;
	satz = (width) / elem_width;
	FILE *efile = fopen("include/graphics/color_cmd.data","rb");
	if( !efile){return;}
	for(int x=xpos; x<xpos+border; x++){
		for(int y=ypos; y<ypos+elem_height; y++){
			uint8_t farbe[3];
			fread(farbe, sizeof(uint8_t),3,efile);
			int color = farbe[0]<<16 | farbe[1]<<8 | farbe[2];
			Apple->ui->display->putSpiegel(x, y, color);
		}
	}
	fclose(efile);
	Apple->ui->display->drawSpiegel(0,Apple->ui->display->bufferlenght);
}
_Colorinterface::~_Colorinterface(){}

int _Colorinterface::getColor(int itter){
	for(int i=0; i<elements.size(); i++){
		if(elements.at(i).id == itter) {return elements.at(i).color;}}
	return -1;
}

void _Colorinterface::showSatz(int id){
	//display leeren
	for(int x=xpos+border; x<xpos+border + width; x++)
		for(int y=ypos; y<ypos+elem_height; y++)
			Apple->ui->display->putSpiegel(x, y, 0x00000000);
	if(id<satzcount){// ein ganzen Satz Elemente zeigen
		int z = 0;
		for(int i=id*satz; i<id*satz + satz; i++){
			drawElem(i, z);
			z++;
		}
	}
	else{ // oder den Rest
		int z = 0;
		for(int i=id*satz; i<id*satz + satzrest; i++){
			drawElem(i, z);
			z++;
		}
	}
	char text[8];
	sprintf(text,"%- 7d",id);
	char text2[8];
	sprintf(text2,"%- 7d",satzcount);
	int fg = 0x00000000;
	int bg = 0x00808080;
	Apple->ui->writeText(xpos, ypos+1, text, 7, fg, bg, 8,true);
	Apple->ui->writeText(xpos, ypos+11, text2, 7, fg, bg, 8,true);
}

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
	char text[9];//iter
	sprintf(text,"% 8d",elements.at(apos).id);
	int fg = 0x00000000;
	if(elements.at(apos).verlauf)fg = 0x00FFFFFF;
	int bg = 0x00808080;
	Apple->ui->writeText(elements.at(apos).x+1, elements.at(apos).y+2, text, 8, fg, bg, 8,true);
	
	char text2[9];//members
	int members = 1;
	for(int i=0; i<Apple->countsOfIter; i++){
		if(Apple->iterMembers[i][0] == elements.at(apos).id){
			members = Apple->iterMembers[i][1];
			break;
		}
	}
	sprintf(text2,"% 8d",members);
	Apple->ui->writeText(elements.at(apos).x+1, elements.at(apos).y+10, text2, 8, fg, bg, 8,true);
	
	Apple->ui->display->drawSpiegel(0, Apple->ui->display->bufferlenght);
}

void _Colorinterface::addElements(){
	elements.erase(elements.begin(),elements.end());
	colorgradients.erase(colorgradients.begin(),colorgradients.end());
	for(int i=Apple->countsOfIter-1; i>=0; i--){
		_CiElement elem;
		elem.id = Apple->iterMembers[i][0];
		double div = (255.0/Apple->depth) * elem.id;
		elem.color = (int)div<<16 | (int)div<<8 | (int)div;
		elements.push_back(elem);
	}
	satzcount = Apple->countsOfIter / satz;
	satzakt = 0;
	satzrest = 0;
	if(satzcount == 0) satzrest = Apple->countsOfIter;
	else satzrest = Apple->countsOfIter - satzcount * satz;
}

void _Colorinterface::onMouseOver(int x, int y, int taste){
	//azuzeigenden Farbsatz ändern
	if(x<border/2 
		&& y>ypos + 50
		&& y<ypos + 50 + 20
		&& taste==1){ if(satzakt>0){ --satzakt; showSatz(satzakt);}}
	else if(x<border 
		&& y>ypos + 50
		&& y<ypos + 50 + 20
		&& taste==1){ if(satzakt<satzcount){ ++satzakt; showSatz(satzakt);}}
	else if(x<border/2 
		&& y>ypos + 50 + 20
		&& y<ypos + 50 + 40
		&& taste==1){
			if(satzakt>0){
				satzakt=satzakt-10;
				if(satzakt<0)satzakt=0;
				showSatz(satzakt);
			}
	}
	else if(x<border
		&& y>ypos + 50 + 20
		&& y<ypos + 50 + 40
		&& taste==1){
			if(satzakt<satzcount){
				satzakt=satzakt+10;
				if(satzakt>satzcount)satzakt=satzcount;
				showSatz(satzakt);
			}
	}
	else if(x<border/2 
		&& y>ypos + 50 + 40
		&& y<ypos + 50 + 60
		&& taste==1){			
			satzakt=0;
			showSatz(satzakt);
	}
	else if(x<border
		&& y>ypos + 50 + 40
		&& y<ypos + 50 + 60
		&& taste==1){			
			satzakt=satzcount;
			showSatz(satzakt);
	}
	else if(x<border/2 
		&& y>ypos + 50 + 60
		&& y<ypos + 50 + 80
		&& taste==1){			
			makeVerlauf();
	}
	else if(x<border
		&& y>ypos + 50 + 60
		&& y<ypos + 50 + 80
		&& taste==1){			
			drawVerlauf();
	}
	
	//Farbe eines Elementes aendern
	int elem_id = -1;
	int color = 0; //0=blue, 1=green, 2=red
	if(x>border) 
		elem_id = (x-border)/elem_width + satzakt * satz;
	if(elem_id > -1 && elem_id < Apple->countsOfIter){
		if( (x) - elements.at(elem_id).x < (elem_width-4)/3) {color = 2;}
		else if( (x) - elements.at(elem_id).x < (elem_width-4)/3*2) {color = 1;}
		if(y>ypos 
				&& y< ypos+text_height/2+2
				&& taste==1){// change color to zero
			elements.at(elem_id).color = 0;
			drawElem(elem_id, elem_id - satzakt * satz);
		}
		else if(y>ypos+text_height/2+2 // click to add gradient
				&& y<ypos+text_height
				&& taste==1){
			elements.at(elem_id).verlauf = ! elements.at(elem_id).verlauf;
			drawElem(elem_id, elem_id - satzakt * satz);
		}
		else if(y>ypos+text_height){// click to change color
			int yrel = (elem_height - text_height) - (y-(ypos+text_height));
			int wert = yrel * (255.0/(elem_height - text_height));
			if(taste==1 && elem_id>-1 && elem_id<((satzakt+1)*satz)){
				int mask = ~(0x00000000 | (0xFF << (color * 8)));
				int farbe = elements.at(elem_id).color & mask;
				farbe = farbe | (wert << (color * 8));
				elements.at(elem_id).color = farbe;
				drawElem(elem_id, elem_id - satzakt * satz);
			}
		}
	}
}

void _Colorinterface::drawVerlauf(){
	Apple->ui->textFertig(false);
	for(int x=0; x<Apple->xres; x++){
		for(int y=0; y<Apple->yres; y++){
			for(int i=0; i<elements.size()-1; i++){
				if(Apple->matrix[x][y] == elements.at(i).id){
					Apple->colormatrix[x][y] = elements.at(i).color;
					break;
				}
			}
		}
	}
	Apple->paint();
	Apple->ui->textFertig(true);
}

void _Colorinterface::makeVerlauf(){
	
	struct _farbe{ int color; int id;};
	std::vector<struct _farbe> gradient;
	
	struct _farbe farbe{elements.at(0).color,0};
	gradient.push_back(farbe);
	for(int i=1; i<elements.size(); i++){
		if(elements.at(i).verlauf){
			struct _farbe farbe{elements.at(i).color,i};
			gradient.push_back(farbe);
		}
	}
	struct _farbe farbe2{elements.at((elements.size()-1)).color, (int)(elements.size()-1)};
	gradient.push_back(farbe2);
	
	for(int i=0; i<gradient.size()-1; i++){
		//iterdiff
		int farbe1 = gradient.at(i).color;
		int farbe2 = gradient.at(i+1).color;
		if(farbe1 > farbe2 ){ int tmp=farbe1; farbe1=farbe2; farbe2=tmp;};
		int iter1 = gradient.at(i).id;
		int iter2 = gradient.at(i+1).id;
		if(iter1 > iter2 ){ int tmp=iter1; iter1=iter2; iter2=tmp;};
		int iterdif = iter2 - iter1;
		uint8_t redt8 = (((0x00FF0000 & farbe2)>>16) - ((0x00FF0000 & farbe1)>>16));
		uint8_t grnt8 = (((0x0000FF00 & farbe2)>>8) - ((0x0000FF00 & farbe1)>>8));
		uint8_t blut8 = ((0x000000FF & farbe2) - (0x000000FF & farbe1));
		double reddif = (double)redt8 / iterdif;
		double grndif = (double)grnt8 / iterdif;
		double bludif = (double)blut8 / iterdif;
		int z = 0;
		uint8_t tmpred = ((0x00FF0000 & farbe1)>>16);
		uint8_t tmpgrn = ((0x0000FF00 & farbe1)>>8);
		uint8_t tmpblu = ((0x000000FF & farbe1)>>0);
		uint8_t tmpcolor[] = {tmpred, tmpgrn, tmpblu};
		for(int k=iter1; k<iter2; k++){
			tmpcolor[0] = tmpcolor[0] + reddif;
			tmpcolor[1] = tmpcolor[1] + grndif;
			tmpcolor[2] = tmpcolor[2] + bludif;
			int thecolor = tmpcolor[0]<<16 | tmpcolor[1]<<8 | tmpcolor[2];
			elements.at(k).color = thecolor;
			z++;
		}
	}
	showSatz(satzakt);
}


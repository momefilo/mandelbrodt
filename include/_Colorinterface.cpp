#include "_Colorinterface.h"

std::function<void(int)> ci_callback;

Colorinterface::Colorinterface(int x, int y, Apple &myApple, std::function<void(int)> _callback){
	apple = &myApple;
	ci_callback = _callback;
	xpos = x;
	ypos = y;
	width = apple->ui->display->xres - border;
	satz = (width) / elem_width;
	FILE *efile = fopen("include/graphics/color_cmd.data","rb");
	if( !efile){return;}
	for(int x=xpos; x<xpos+border; x++){
		for(int y=ypos; y<ypos+elem_height; y++){
			uint8_t farbe[3];
			fread(farbe, sizeof(uint8_t),3,efile);
			int color = farbe[0]<<16 | farbe[1]<<8 | farbe[2];
			apple->ui->display->putSpiegel(x, y, color);
		}
	}
	fclose(efile);
	apple->ui->display->drawSpiegel(0,apple->ui->display->bufferlenght);
}
//Colorinterface::~Colorinterface(){}

int Colorinterface::getColor(int itter){
	for(int i=0; i<elements.size(); i++){
		if(elements.at(i).id == itter) {return elements.at(i).color;}}
	return -1;
}

void Colorinterface::showSatz(int id){
	//display leeren
	for(int x=xpos+border; x<xpos+border + width; x++)
		for(int y=ypos; y<ypos+elem_height; y++)
			apple->ui->display->putSpiegel(x, y, 0x00000000);
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
	apple->ui->writeText(xpos, ypos+1, text, 7, fg, bg, 8,true);
	apple->ui->writeText(xpos, ypos+11, text2, 7, fg, bg, 8,true);
}

void Colorinterface::drawElem(int apos, int dpos){
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
				apple->ui->display->putSpiegel(fx,fy, elements.at(apos).color);
			else if(x>2 && x<(elem_width-4)/3 && y>text_height && y>yred)
				apple->ui->display->putSpiegel(fx,fy, 0x00FF0000);
			else if(x>(elem_width-4)/3-1 && x<(elem_width-4)/3*2 && y>text_height && y>ygrn)
				apple->ui->display->putSpiegel(fx,fy, 0x0000FF00);
			else if(x>(elem_width-4)/3*2-1 && x<elem_width-2 && y>text_height && y>yblu)
				apple->ui->display->putSpiegel(fx,fy, 0x000000FF);
			else apple->ui->display->putSpiegel(fx,fy, 0x00808080);
		}
	}
	char text[9];//iter
	sprintf(text,"% 8d",elements.at(apos).id);
	int fg = 0x00000000;
	if(elements.at(apos).verlauf)fg = 0x00FFFFFF;
	int bg = 0x00808080;
	apple->ui->writeText(elements.at(apos).x+1, elements.at(apos).y+2, text, 8, fg, bg, 8,true);
	
	char text2[9];//members
	int members = 1;
	for(int i=0; i<apple->countsOfIter; i++){
		if(apple->iterMembers[i][0] == elements.at(apos).id){
			members = apple->iterMembers[i][1];
			break;
		}
	}
	sprintf(text2,"% 8d",members);
	apple->ui->writeText(elements.at(apos).x+1, elements.at(apos).y+10, text2, 8, fg, bg, 8,true);
	
	apple->ui->display->drawSpiegel(0, apple->ui->display->bufferlenght);
}

void Colorinterface::addElements(){
	elements.erase(elements.begin(),elements.end());
	for(int i=apple->countsOfIter-1; i>=0; i--){
		_CiElement elem;
		elem.id = apple->iterMembers[i][0];
		double div = (255.0/apple->paras.depth) * elem.id;
		elem.color = (int)div<<16 | (int)div<<8 | (int)div;
		elements.push_back(elem);
	}
	satzcount = apple->countsOfIter / satz;
	satzakt = 0;
	satzrest = 0;
	if(satzcount == 0) satzrest = apple->countsOfIter;
	else satzrest = apple->countsOfIter - satzcount * satz;
}

void Colorinterface::onMouseOver(int x, int y, int taste){
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
	if(elem_id > -1 && elem_id < apple->countsOfIter){
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

void Colorinterface::drawVerlauf(){
	apple->ui->textFertig(false);
	for(int x=0; x<apple->paras.xres; x++){
		for(int y=0; y<apple->paras.yres; y++){
			apple->colormatrix[x][y] = getColor(apple->matrix[x][y]);
		}
	}
	apple->paint();
	apple->ui->textFertig(true);
}

void Colorinterface::makeVerlauf(){
	
	gradient.erase(gradient.begin(),gradient.end());
	//get Gradients
	for(int i=0; i<elements.size(); i++){
		if(elements.at(i).verlauf){
			gradient.push_back(_farbe{elements.at(i).color,i});}}
	
	//paint Gradients to elemets
	int tmpsize = gradient.size() - 1;
	for(int i=0; i<tmpsize; i++){
		int farbe1 = gradient.at(i).color;
		int farbe2 = gradient.at(i+1).color;
		int iter1 = gradient.at(i).id;
		int iter2 = gradient.at(i+1).id;
		if(iter1 > iter2 ){ int tmp=iter1; iter1=iter2; iter2=tmp;};
		int iterdif = iter2 - iter1;
		
		//get red
		bool rednegativ = false;
		uint8_t redt8;
		if(((0x00FF0000 & farbe2)>>16) < ((0x00FF0000 & farbe1)>>16)){
			redt8 = (((0x00FF0000 & farbe1)>>16) - ((0x00FF0000 & farbe2)>>16));
			rednegativ = true;}
		else{ redt8 = (((0x00FF0000 & farbe2)>>16) - ((0x00FF0000 & farbe1)>>16));}
		double reddif = (double)redt8 / iterdif;
		uint8_t tmpred = ((0x00FF0000 & farbe1)>>16);
		
		//get green
		bool grnnegativ = false;
		uint8_t grnt8;
		if(((0x0000FF00 & farbe2)>>8) < ((0x0000FF00 & farbe1)>>8)){
			grnt8 = (((0x0000FF00 & farbe1)>>8) - ((0x0000FF00 & farbe2)>>8));
			grnnegativ = true;}
		else{ grnt8 = (((0x0000FF00 & farbe2)>>8) - ((0x0000FF00 & farbe1)>>8));}
		double grndif = (double)grnt8 / iterdif;
		uint8_t tmpgrn = ((0x0000FF00 & farbe1)>>8);
		
		//get blue
		bool blunegativ = false;
		uint8_t blut8;
		if((0x000000FF & farbe2) < (0x000000FF & farbe1)){
			blut8 = ((0x000000FF & farbe1) - (0x000000FF & farbe2));
			blunegativ = true;}
		else{ blut8 = ((0x000000FF & farbe2) - (0x000000FF & farbe1));}
		double bludif = (double)blut8 / iterdif;
		uint8_t tmpblu = ((0x000000FF & farbe1)>>0);
		
		//base-color to add colordiffs
		uint8_t tmpcolor[] = {tmpred, tmpgrn, tmpblu};
		
		//add colordiffs to basecolor and draw them to elements
		for(int k=iter1; k<iter2; k++){
			if(rednegativ){ tmpcolor[0] = tmpcolor[0] - reddif;}
			else{ tmpcolor[0] = tmpcolor[0] + reddif;}
			if(grnnegativ){ tmpcolor[1] = tmpcolor[1] - grndif;}
			else{ tmpcolor[1] = tmpcolor[1] + grndif;}
			if(blunegativ){ tmpcolor[2] = tmpcolor[2] - bludif;}
			else {tmpcolor[2] = tmpcolor[2] + bludif;}
			
			// the color to paint
			int thecolor = tmpcolor[0]<<16 | tmpcolor[1]<<8 | tmpcolor[2];
			elements.at(k).color = thecolor;
		}
	}
	showSatz(satzakt);
}


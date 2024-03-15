#include "_AppleMemory.h"
#include <ctime>
#include <fstream>

AppleMemory::AppleMemory(int x, int y, Colorinterface &_ci){
	xpos = x;
	ypos = y;
	ci = &_ci;
	height = ci->apple->ui->display->yres - ypos - Reglerheight;
	
	FILE *efile = fopen("include/graphics/memApple.data","rb");
	if( !efile){
		throw std::runtime_error{
			std::string{ "Failed to open include/graphics/color_cmd.data " }
			+ std::strerror(errno)
		};
	}
	for(int x=xpos; x<xpos+width; x++){
		for(int y=ypos; y<ypos+topBorder; y++){
			uint8_t farbe[3];
			fread(farbe, sizeof(uint8_t),3,efile);
			int color = farbe[0]<<16 | farbe[1]<<8 | farbe[2];
			ci->apple->ui->display->putSpiegel(x, y, color);
		}
	}
	fclose(efile);
	ci->apple->ui->display->drawSpiegel(0,ci->apple->ui->display->bufferlenght);
	
	std::ifstream file("mem.Apples", std::ios::in|std::ios::binary);
	if(!file.fail()){
		int count = 0;
		file.read((char*) &count, sizeof(int));
		memApples.resize(count);
		for(int i=0;i<count; i++){
			int size = 0;
			file.read(reinterpret_cast<char *> (&memApples[i].paras), sizeof(memApples[i].paras));
			file.read(reinterpret_cast<char *> (&memApples[i].pic), sizeof(memApples[i].pic));
			file.read((char*) &size, sizeof(int));
			memApples.at(i).ci_elements.resize(size);
			
			file.read(reinterpret_cast<char *> 
					(&memApples[i].ci_elements[0]),size * sizeof(memApples[i].ci_elements[0]));
		}
		file.close();
	}
	initMemory();
	showSatz(0);
	
}
void AppleMemory::initMemory(){
	satz = (height - topBorder) / (picHeight + 2);
	satzcount = memApples.size() / satz;
	satzakt = 0;
	satzrest = 0;
	if(satzcount == 0) satzrest = memApples.size();
	else satzrest = memApples.size() - satzcount * satz;
}

void AppleMemory::onMouseOver(int x, int y, int taste){
	//klick scroll_buttons
	if(y<(ypos+topBorder/2) && taste==1){
		if(satzakt>0){ --satzakt; showSatz(satzakt);}}
	else if(y<ypos+topBorder && taste==1){
		if(satzakt<satzcount){ ++satzakt; showSatz(satzakt);}}
		
	else{
		int dpos = (y - (ypos +topBorder)) / (picHeight+2);
		int id = dpos + satzakt * satz;
		bool del = false;
		if(x-xpos > (picWidth-rightBorder)) del = true;
		if(taste == 1){
			if(del)delApple(id);
			else loadApple(id);
		}
	}
		
}

void AppleMemory::writeToFile(){
	std::ofstream file("mem.Apples", std::ios::out|std::ios::binary);
	int count = memApples.size();
	file.write( (char*)(&count), sizeof(int));
	for(int i=0; i<memApples.size(); i++){
		file.write(reinterpret_cast<char*>(&memApples[i].paras),
				sizeof(memApples[i].paras));
		file.write(reinterpret_cast<char*>(&memApples[i].pic),
				sizeof(memApples[i].pic));
		
		int elem_size = memApples.at(i).ci_elements.size();
		file.write( (char*)(&elem_size), sizeof(int));
		
		file.write(reinterpret_cast<char*>(&memApples[i].ci_elements[0]),
			elem_size * sizeof(memApples[i].ci_elements[0]));
	}
	file.close();
}
void AppleMemory::saveApple(){
	MemApple myApple;
	myApple.paras = ci->apple->getPara();
		
	//scale colormatrix to fit in 100x45 pixels
	int divisor = 200;
	long double xres = (long double) (ci->apple->paras.xres) / divisor;
	long double tmpxres = xres;
	int xfak = 1;
	while(xres < picWidth-rightBorder){ xfak++; xres = tmpxres * xfak;}
	xfak--;
	
	long double yres = (long double) (ci->apple->paras.yres) / divisor;
	long double tmpyres = yres;
	int yfak = 1;
	while(yres < picHeight){ yfak++; yres = tmpyres * yfak;}
	yfak--;
	
	if(yfak < xfak) xfak = yfak;
	else yfak = xfak;
	int i_xres = tmpxres * xfak;
	int i_yres = tmpyres * yfak;
	
	//write scaled picture to array
	for(int x=0; x<picWidth; x++){
		for(int y=0; y<picHeight; y++){
			if(x<i_xres && y<i_yres)
				myApple.pic[x][y] = ci->apple->colormatrix[ x*divisor/xfak ][ y*divisor/yfak ];
			else if(x<i_xres) myApple.pic[x][y] = 0;
			else myApple.pic[x][y] = 0x00FF0000;
		}
	}
//	myApple.ci_elements = ci->elements;
	std::copy(ci->elements.begin(), ci->elements.end(),back_inserter(myApple.ci_elements));
	memApples.push_back(myApple);
	writeToFile();
	initMemory();
	satzakt = satzcount;
	showSatz(satzakt);
}

void AppleMemory::delApple(int id){
	memApples.erase(memApples.begin()+id);
	initMemory();
	writeToFile();
	showSatz(0);
	
}
void AppleMemory::loadApple(int id){
	ci->apple->init(memApples.at(id).paras, true);
	ci->elements = memApples.at(id).ci_elements;
	ci->initElements();
	ci->showSatz(0);
	ci->drawVerlauf();
}
void AppleMemory::drawApple(int id, int dpos){
	for(int x=0; x<picWidth; x++){
		for(int y=0; y<picHeight; y++){
			int myy = y + ypos + topBorder + dpos*(picHeight+2);
			int myx = x +xpos;
			ci->apple->ui->display->putSpiegel(myx, myy, memApples.at(id).pic[x][y]);
		}
	}
	ci->apple->ui->display->drawSpiegel(0, ci->apple->ui->display->bufferlenght);
}

void AppleMemory::showSatz(int _satz){
	//display leeren
	for(int x=xpos; x<xpos + width; x++)
		for(int y=ypos+topBorder; y<ypos+height; y++)
			ci->apple->ui->display->putSpiegel(x, y, 0x00000000);
	if(memApples.size() > 0){
		if(_satz<satzcount){// ein ganzen Satz Elemente zeigen
			int dpos = 0;
			for(int i=_satz*satz; i<_satz*satz + satz; i++){
				drawApple(i, dpos);
				dpos++;
			}
		}
		else{ // oder den Rest
			int dpos = 0;
			for(int i=_satz*satz; i<_satz*satz + satzrest; i++){
				drawApple(i, dpos);
				dpos++;
			}
		}
	}
}

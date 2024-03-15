#include "_AppleMemory.h"
#include <ctime>
#include <fstream>

AppleMemory::AppleMemory(int x, int y, Colorinterface &_ci){
	xpos = x;
	ypos = y;
	ci = &_ci;
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
		drawApples();
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
	drawApples();
}

void AppleMemory::drawApples(){
	for(int i=0; i<memApples.size(); i++){
		for(int x=0; x<picWidth; x++){
			for(int y=0; y<picHeight; y++){
				int myy = ypos + i*(picHeight+2) + y;
				int myx = xpos + x;
				ci->apple->ui->display->putSpiegel(myx, myy, memApples.at(i).pic[x][y]);
			}
		}
	}
	ci->apple->ui->display->drawSpiegel(0, ci->apple->ui->display->bufferlenght);
}



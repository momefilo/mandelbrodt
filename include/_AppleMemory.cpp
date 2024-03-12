#include "_AppleMemory.h"
#include <ctime>
#include <fstream>

AppleMemory::AppleMemory(int x, int y, Colorinterface &_ci){
	xpos = x;
	ypos = y;
	ci = &_ci;
	std::ifstream file("mem.Apples", std::ios::in|std::ios::binary);
	if(!file.fail()){
		file.read((char*) &count, sizeof(int));
		std::cout<<"OK konstrukt\n";
		memApples.resize(count);
		file.read(reinterpret_cast<char *> (&memApples[0]), count * sizeof(memApples[0]));
		file.close();
		drawApples();
	}
}
void AppleMemory::saveApple(){
	MemApple myApple;
	myApple.paras = ci->apple->getPara();
		
	//scale colormatrix to fit in 80x45 pixels
	long double xres = (long double) (ci->apple->paras.xres) / 200;
	long double tmpxres = xres;
	int xfak = 1;
	while(xres < picWidth){ xfak++; xres = tmpxres * xfak;}
	xfak--;
	
	long double yres = (long double) (ci->apple->paras.yres) / 200;
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
				myApple.pic[x][y] = ci->apple->colormatrix[ x*200/xfak ][ y*200/yfak ];
			else myApple.pic[x][y] = 0;
		}
	}
	memApples.push_back(myApple);
	
	count++;
	std::ofstream file("mem.Apples", std::ios::out|std::ios::binary);
	file.write( (char*)(&count), sizeof(int));
	std::cout<<"OK save\n";
	file.write(reinterpret_cast<char*>(&memApples[0]), memApples.size() * sizeof(memApples[0]));
	file.close();
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



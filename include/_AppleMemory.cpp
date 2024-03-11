#include "_AppleMemory.h"
#include <ctime>
#include <fstream>

AppleMemory::AppleMemory(int x, int y, Colorinterface &_ci){
	xpos = x;
	ypos = y;
	ci = &_ci;
}
void AppleMemory::saveApple(){
	std::string name = "apples/";
	name.append(std::to_string(time(NULL)));
	std::string name_apple = name;
	name_apple.append(".apple");
	std::ofstream file(name_apple, std::ios::binary);
	
	int elements_lenght = ci->elements.size();
	int gradient_lenght = ci->gradient.size();
	
	file.write( (char*)(&ci->apple->paras.xres), sizeof(int));
	file.write( (char*)(&ci->apple->paras.yres), sizeof(int));
	file.write( (char*)(&ci->apple->paras.rmin), sizeof(long double));
	file.write( (char*)(&ci->apple->paras.rmax), sizeof(long double));
	file.write( (char*)(&ci->apple->paras.imin), sizeof(long double));
	file.write( (char*)(&ci->apple->paras.imax), sizeof(long double));
	file.write( (char*)(&ci->apple->paras.depth), sizeof(int));
	file.write( (char*)(&elements_lenght), sizeof(int));
	file.write( (char*)(&gradient_lenght), sizeof(int));
	file.write( (char*)(&ci->elements[0]), elements_lenght * sizeof(ci->elements[0]));
	file.write( (char*)(&ci->gradient[0]), gradient_lenght * sizeof(ci->gradient[0]));
	file.close();
		
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
	
	//write scaled colormatrix to file
	name.append(".pic");
	std::ofstream filepic(name, std::ios::binary);
	filepic.write( (char*)(&i_xres), sizeof(int));
	filepic.write( (char*)(&i_yres), sizeof(int));
	for(int x=0; x<i_xres; x++){
		for(int y=0; y<i_yres; y++){
			 filepic.write((char*) (&ci->apple->colormatrix[ x*200/xfak ][ y*200/yfak ]), sizeof(int));
		}
	}
	filepic.close();
	drawPic(name, ypos+count*picHeight);
	count++;
}

void AppleMemory::drawPic(std::string filename, int myy){
	std::ifstream filepic(filename, std::ios::binary);
	int xy[2];
	filepic.read((char*) &xy, sizeof(xy));
	for(int x=0; x<xy[0]; x++){
		for(int y=0; y<xy[1]; y++){
			int buf;
			filepic.read((char*) &buf, sizeof(int));
			ci->apple->ui->display->putSpiegel(xpos + x, y + myy, buf);
		}
	}
	filepic.close();
}



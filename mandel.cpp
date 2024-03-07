// momefilo Desing
#include "include/_Apple.h"
#include "include/_Userinterface.h"
#include "include/_Colorinterface.h"
#include "include/_Display.h"

#define BORDER 2

_Display *Display;
_Userinterface *Ui;
_Apple *Apple;
_Colorinterface *Ci;

struct _AppleData{
	int xpos;
	int ypos;
	int width;
	int height;
	int xres{1920};
	int yres{1080};
	long double rmin{-1};
	long double rmax{2};
	long double imin{-1};
	long double imax{1};
	int depth{100};
};
std::vector<struct _AppleData> myApples;
int aktApple = 0;

void newApple(){
		myApples.push_back(*new _AppleData);
		myApples.at(aktApple).xres = Apple->xres;
		myApples.at(aktApple).yres = Apple->yres;
		myApples.at(aktApple).depth = Apple->depth;
		myApples.at(aktApple).rmin = Apple->rmin;
		myApples.at(aktApple).rmax = Apple->rmax;
		myApples.at(aktApple).imin = Apple->imin;
		myApples.at(aktApple).imax = Apple->imax;
		myApples.at(aktApple).xpos = Apple->xpos;
		myApples.at(aktApple).ypos = Apple->ypos;
		myApples.at(aktApple).width = Apple->width;
		myApples.at(aktApple).height = Apple->height;
		aktApple++;
}

void ui_callback(int i){
	if(i<1){
		myApples.at(aktApple-1).xres = Apple->ui->getWert(0);
		myApples.at(aktApple-1).yres = Apple->ui->getWert(1);
		myApples.at(aktApple-1).rmin = Apple->ui->getWert(2);
		myApples.at(aktApple-1).rmax = Apple->ui->getWert(3);
		myApples.at(aktApple-1).imin = Apple->ui->getWert(4);
		myApples.at(aktApple-1).imax = Apple->ui->getWert(5);
		myApples.at(aktApple-1).depth = Apple->ui->getWert(6);
		Apple->rmin = myApples.at(aktApple-1).rmin;
		Apple->rmax = myApples.at(aktApple-1).rmax;
		Apple->imin = myApples.at(aktApple-1).imin;
		Apple->imax = myApples.at(aktApple-1).imax;
		Apple->depth = myApples.at(aktApple-1).depth;
		Apple->clearScreen();
		Apple->init();
		myApples.at(aktApple-1).xpos = Apple->xpos;
		myApples.at(aktApple-1).ypos = Apple->ypos;
		myApples.at(aktApple-1).width = Apple->width;
		myApples.at(aktApple-1).height = Apple->height;
		Apple->calc();
		Apple->paint();
		Apple->sort();
		Ci->addElements();
		Ci->showSatz(0);
	}
	else if(i<2){
		Apple->sort();
		Ci->addElements();
		Ci->showSatz(0);
	}
	else if(i==3){
		if(aktApple>1){
			aktApple--;
			myApples.pop_back();
			Apple->clearScreen();
			Apple->ui->setWert(0, myApples.at(aktApple-1).xres);
			Apple->ui->setWert(1, myApples.at(aktApple-1).yres);
			Apple->ui->setWert(2, myApples.at(aktApple-1).rmin);
			Apple->ui->setWert(3, myApples.at(aktApple-1).rmax);
			Apple->ui->setWert(4, myApples.at(aktApple-1).imin);
			Apple->ui->setWert(5, myApples.at(aktApple-1).imax);
			Apple->ui->setWert(6, myApples.at(aktApple-1).depth);
			Apple->xres = myApples.at(aktApple-1).xres;
			Apple->yres = myApples.at(aktApple-1).yres;
			Apple->xpos = myApples.at(aktApple-1).xpos;
			Apple->ypos = myApples.at(aktApple-1).ypos;
			Apple->width = myApples.at(aktApple-1).width;
			Apple->height = myApples.at(aktApple-1).height;
			Apple->rmin = myApples.at(aktApple-1).rmin;
			Apple->rmax = myApples.at(aktApple-1).rmax;
			Apple->imin = myApples.at(aktApple-1).imin;
			Apple->imax = myApples.at(aktApple-1).imax;
			Apple->depth = myApples.at(aktApple-1).depth;
			Apple->init();
			Apple->calc();
			Apple->paint();
			Apple->sort();
			Ci->addElements();
			Ci->showSatz(0);
		}
	}
	else if(i == 9){
		newApple();
	}
};
void ci_callback(int i){}

void loop(){
	int fdm = open("/dev/input/mice", O_RDONLY);
	if (fdm == -1) { exit(EXIT_FAILURE);}
	int maus_x = Apple->ui->display->xres/2, maus_y = Apple->ui->display->yres/2;
	int8_t paket[3];
	int color = 0x00FF00FF;
	while (1) {
		uint8_t taste = 0;
		// Datenpaket einlesen
		read(fdm, paket, 3);
		if(paket[1]>3 || paket[1]<-3) paket[1] = paket[1]*3;
		if(paket[2]>3 || paket[2]<-3) paket[2] = paket[2]*3;
		maus_x += paket[1];
		maus_y += paket[2];
		if (maus_x < 0) maus_x = 0;
		if (maus_y < 0) maus_y = 0;
		if (maus_x >= Apple->ui->display->xres) maus_x = Apple->ui->display->xres - 1;
		if (maus_y >= Apple->ui->display->yres) maus_y = Apple->ui->display->yres - 1;

		// Status der Maustasten dekodieren
		if(paket[0] & 1) taste++;
		if(paket[0] & 2) taste += 2;
		
		//mauspointer zeichner
		Apple->ui->display->drawSpiegel(0,Apple->ui->display->bufferlenght);
		for(int i=-3; i<4; i++){
			for(int j=-3; j<4; j++){
				int x = (maus_x + i);
				int y = Apple->ui->display->yres - (maus_y + j);
				if(x < 0) x = 0;
				if(x >= Apple->ui->display->xres) x = Apple->ui->display->xres-1;
				if(y < 0) y = 0;
				if(y >= Apple->ui->display->yres) y = Apple->ui->display->yres-1;
				Apple->ui->display->putDisplay(x, y, color);
			}
		}
		
		//Apple
		if(maus_x >= Apple->xpos && (Apple->ui->display->yres-maus_y) <= Apple->height){
			Apple->onMouseOver(maus_x, (Apple->ui->display->yres-maus_y), taste);
		}
		//Ui_element
		if(maus_x < Apple->ui->width && (Apple->ui->display->yres-maus_y) > 0 
				&& (Apple->ui->display->yres-maus_y) < Apple->ui->height){
			Apple->ui->onMouseOver(maus_x, (Apple->ui->display->yres-maus_y), taste);
		}
		//Ci_element
		if((Apple->ui->display->yres-maus_y) > Ci->ypos){
			Ci->onMouseOver(maus_x, (Apple->ui->display->yres-maus_y), taste);
		}
	}
}

int main(){
	Display = new _Display();
	Ui = new _Userinterface(BORDER, BORDER, *Display,  ui_callback);
	Ui->setWert(0, Display->xres);
	Ui->setWert(1, Display->yres);
	Ui->setWert(2, -1);
	Ui->setWert(3, 2);
	Ui->setWert(4, -1);
	Ui->setWert(5, 1);
	Ui->setWert(6, 100);
	Apple = new _Apple(*Ui);
	Apple->init(Display->xres, Display->yres, -1, 2, -1, 1);
	Apple->calc();
	Apple->paint();
	Apple->sort();
	newApple();
	Ci = new _Colorinterface(BORDER, Apple->ui->display->yres - Reglerheight, *Apple, ci_callback);
	Ci->addElements();
	Ci->showSatz(0);
	
	loop();
	//Programmende
	delete Display;
	delete Ui;
	return 0;
}

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

void ui_callback(int i){
	if(i<1){ 
		Apple->xres = Apple->ui->getWert(0);
		Apple->yres = Apple->ui->getWert(1);
		Apple->rmin = Apple->ui->getWert(2);
		Apple->rmax = Apple->ui->getWert(3);
		Apple->imin = Apple->ui->getWert(4);
		Apple->imax = Apple->ui->getWert(5);
		Apple->depth = Apple->ui->getWert(6);
		Apple->clearScreen();
		Apple->init();
		Apple->calc();
		Apple->paint();
	}
	else if(i<2){
		Apple->sort();
		Ci->addElements();
		Ci->showSatz(0);
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
	Ci = new _Colorinterface(BORDER, Apple->ui->display->yres - Reglerheight, *Apple, ci_callback);
	Ci->addElements();
	Ci->showSatz(0);
	
	loop();
	//Programmende
	delete Display;
	delete Ui;
	return 0;
}
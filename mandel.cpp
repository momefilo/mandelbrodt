// momefilo Desing
#include "include/_Apple.h"
#include "include/_Userinterface.h"
#include "include/_Colorinterface.h"
#include "include/_Display.h"

constexpr unsigned int BORDER = 2;

Display *myDisplay;
_Userinterface *Ui;
_Apple *Apple;
_Colorinterface *Ci;

struct AppleData{
	int xpos{};
	int ypos{};
	int width{};
	int height{};
	int xres{1920};
	int yres{1080};
	long double rmin{-1};
	long double rmax{2};
	long double imin{-1};
	long double imax{1};
	int depth{100};
};
std::vector<struct AppleData> myAppleDatas;

void newApple(){
	myAppleDatas.push_back(AppleData());
	myAppleDatas.back().xres = Apple->xres;
	myAppleDatas.back().yres = Apple->yres;
	myAppleDatas.back().depth = Apple->depth;
	myAppleDatas.back().rmin = Apple->rmin;
	myAppleDatas.back().rmax = Apple->rmax;
	myAppleDatas.back().imin = Apple->imin;
	myAppleDatas.back().imax = Apple->imax;
	myAppleDatas.back().xpos = Apple->xpos;
	myAppleDatas.back().ypos = Apple->ypos;
	myAppleDatas.back().width = Apple->width;
	myAppleDatas.back().height = Apple->height;
}

void ui_callback(int i){
	if(i<1){//recalc Apple
		myAppleDatas.back().xres = Apple->ui->getWert(0);
		myAppleDatas.back().yres = Apple->ui->getWert(1);
		myAppleDatas.back().rmin = Apple->ui->getWert(2);
		myAppleDatas.back().rmax = Apple->ui->getWert(3);
		myAppleDatas.back().imin = Apple->ui->getWert(4);
		myAppleDatas.back().imax = Apple->ui->getWert(5);
		myAppleDatas.back().depth = Apple->ui->getWert(6);
		Apple->rmin = myAppleDatas.back().rmin;
		Apple->rmax = myAppleDatas.back().rmax;
		Apple->imin = myAppleDatas.back().imin;
		Apple->imax = myAppleDatas.back().imax;
		Apple->depth = myAppleDatas.back().depth;
		Apple->clearScreen();
		Apple->init();
		myAppleDatas.back().xpos = Apple->xpos;
		myAppleDatas.back().ypos = Apple->ypos;
		myAppleDatas.back().width = Apple->width;
		myAppleDatas.back().height = Apple->height;
		Apple->calc();
		Apple->paint();
		Apple->sort();
		Ci->addElements();
		Ci->showSatz(0);
	}
	else if(i<2){//re-sort the Apple
		Apple->sort();
		Ci->addElements();
		Ci->showSatz(0);
	}
	else if(i==3){//bach to previus Apple
		std::cout<<"myAppleDatas.size()"<<myAppleDatas.size();
		if(myAppleDatas.size()>1){
			myAppleDatas.pop_back();
			Apple->clearScreen();
			Apple->ui->setWert(0, myAppleDatas.back().xres);
			Apple->ui->setWert(1, myAppleDatas.back().yres);
			Apple->ui->setWert(2, myAppleDatas.back().rmin);
			Apple->ui->setWert(3, myAppleDatas.back().rmax);
			Apple->ui->setWert(4, myAppleDatas.back().imin);
			Apple->ui->setWert(5, myAppleDatas.back().imax);
			Apple->ui->setWert(6, myAppleDatas.back().depth);
			Apple->xres = myAppleDatas.back().xres;
			Apple->yres = myAppleDatas.back().yres;
			Apple->xpos = myAppleDatas.back().xpos;
			Apple->ypos = myAppleDatas.back().ypos;
			Apple->width = myAppleDatas.back().width;
			Apple->height = myAppleDatas.back().height;
			Apple->rmin = myAppleDatas.back().rmin;
			Apple->rmax = myAppleDatas.back().rmax;
			Apple->imin = myAppleDatas.back().imin;
			Apple->imax = myAppleDatas.back().imax;
			Apple->depth = myAppleDatas.back().depth;
			Apple->init();
			Apple->calc();
			Apple->paint();
			Apple->sort();
			Ci->addElements();
			Ci->showSatz(0);
		}
	}
	else if(i == 9){// new Apple
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
	try{
		myDisplay = new Display();
		Ui = new _Userinterface(BORDER, BORDER, *myDisplay,  ui_callback);
		Ui->setWert(0, myDisplay->xres);
		Ui->setWert(1, myDisplay->yres);
		Ui->setWert(2, -1);
		Ui->setWert(3, 2);
		Ui->setWert(4, -1);
		Ui->setWert(5, 1);
		Ui->setWert(6, 100);
		Apple = new _Apple(*Ui);
		Apple->init(myDisplay->xres, myDisplay->yres, -1, 2, -1, 1);
		Apple->calc();
		Apple->paint();
		Apple->sort();
		newApple();
		Ci = new _Colorinterface(BORDER, Apple->ui->display->yres - Reglerheight, *Apple, ci_callback);
		Ci->addElements();
		Ci->showSatz(0);

		loop();
		//Programmende
		delete Ci;
		delete Apple;
		delete Ui;
		delete myDisplay;
		return 0;
	}
	catch (const std::exception& e){
		std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}

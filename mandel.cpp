// momefilo Desing
#include "include/_Apple.h"
#include "include/_Userinterface.h"
#include "include/_Colorinterface.h"
#include "include/_Display.h"

constexpr unsigned int BORDER = 2;

Display *myDisplay;
Userinterface *Ui;
Apple *myApple;
_Colorinterface *Ci;

std::vector<struct ApplePara> myAppleDatas;

void newApple(){ myAppleDatas.push_back(myApple->getPara()); }

void ui_callback(int i){
	if(i<1){//recalc Apple
		myAppleDatas.back().xres = myApple->ui->getWert(0);
		myAppleDatas.back().yres = myApple->ui->getWert(1);
		myAppleDatas.back().rmin = myApple->ui->getWert(2);
		myAppleDatas.back().rmax = myApple->ui->getWert(3);
		myAppleDatas.back().imin = myApple->ui->getWert(4);
		myAppleDatas.back().imax = myApple->ui->getWert(5);
		myAppleDatas.back().depth = myApple->ui->getWert(6);
		
		myApple->setPara(myAppleDatas.back());
		myApple->clearScreen();
		myApple->init();
		myAppleDatas.back() = myApple->getPara();
		
		myApple->calc();
		myApple->paint();
		myApple->sort();
		Ci->addElements();
		Ci->showSatz(0);
	}
	else if(i<2){//re-sort the Apple
		myApple->sort();
		Ci->addElements();
		Ci->showSatz(0);
	}
	else if(i==3){//back to previus Apple
		if(myAppleDatas.size()>1){
			myAppleDatas.pop_back();
			myApple->clearScreen();			
			myApple->ui->setParas(myAppleDatas.back());
			myApple->setPara(myAppleDatas.back());
			myApple->init();
			myApple->calc();
			myApple->paint();
			myApple->sort();
			Ci->addElements();
			Ci->showSatz(0);
		}
	}
	else if(i == 9){// new Apple
		newApple();
		myApple->sort();
		Ci->addElements();
		Ci->showSatz(0);
	}
};
void ci_callback(int i){}

void loop(){
	int fdm = open("/dev/input/mice", O_RDONLY);
	if (fdm < 0) {
		throw std::runtime_error{
			std::string{ "Failed to open /dev/input/mice " }
			+ std::strerror(errno)
		};
	}
	int maus_x = myApple->ui->display->xres/2, maus_y = myApple->ui->display->yres/2;
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
		if (maus_x >= myApple->ui->display->xres) maus_x = myApple->ui->display->xres - 1;
		if (maus_y >= myApple->ui->display->yres) maus_y = myApple->ui->display->yres - 1;

		// Status der Maustasten dekodieren
		if(paket[0] & 1) taste++;
		if(paket[0] & 2) taste += 2;
		
		//mauspointer zeichner
		myApple->ui->display->drawSpiegel(0,myApple->ui->display->bufferlenght);
		for(int i=-3; i<4; i++){
			for(int j=-3; j<4; j++){
				int x = (maus_x + i);
				int y = myApple->ui->display->yres - (maus_y + j);
				if(x < 0) x = 0;
				if(x >= myApple->ui->display->xres) x = myApple->ui->display->xres-1;
				if(y < 0) y = 0;
				if(y >= myApple->ui->display->yres) y = myApple->ui->display->yres-1;
				myApple->ui->display->putDisplay(x, y, color);
			}
		}
		
		//Apple
		if(maus_x >= myApple->paras.xpos && (myApple->ui->display->yres-maus_y) <= myApple->paras.height){
			myApple->onMouseOver(maus_x, (myApple->ui->display->yres-maus_y), taste);
		}
		//Ui_element
		if(maus_x < myApple->ui->width && (myApple->ui->display->yres-maus_y) > 0 
				&& (myApple->ui->display->yres-maus_y) < myApple->ui->height){
			myApple->ui->onMouseOver(maus_x, (myApple->ui->display->yres-maus_y), taste);
		}
		//Ci_element
		if((myApple->ui->display->yres-maus_y) > Ci->ypos){
			Ci->onMouseOver(maus_x, (myApple->ui->display->yres-maus_y), taste);
		}
	}
}

int main(){
	try{
		myDisplay = new Display();
		Ui = new Userinterface(BORDER, BORDER, *myDisplay,  ui_callback);
		Ui->setWert(0, myDisplay->xres);
		Ui->setWert(1, myDisplay->yres);
		Ui->setWert(2, -1);
		Ui->setWert(3, 2);
		Ui->setWert(4, -1);
		Ui->setWert(5, 1);
		Ui->setWert(6, 100);
		myApple = new Apple(*Ui);
		myApple->init(myDisplay->xres, myDisplay->yres, -1, 2, -1, 1);
		myApple->calc();
		myApple->paint();
		myApple->sort();
		newApple();
		Ci = new _Colorinterface(BORDER, myApple->ui->display->yres - Reglerheight, *myApple, ci_callback);
		Ci->addElements();
		Ci->showSatz(0);

		loop();
		//Programmende
		delete Ci;
		delete myApple;
		delete Ui;
		delete myDisplay;
		return 0;
	}
	catch (const std::exception& e){
		std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}

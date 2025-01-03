#include "_Apple.h"

/* gibt die anzahl an Iterationenen bis zur Abbruchbedingung der Mandelbrodtmenge,
 * oder 0 wenn (cr|ci) nicht in der Mandelbrodtmenge ist, zurueck*/
int getIterCount(double cr, double ci, int count){
	double r = 0;
	double i = 0;
	double array[] = {0, 0, cr, ci};
	int n = 0;
	while(n < count){
		n++;
		r = array[0]*array[0] - array[1]*array[1] - array[2];
		i = array[0]*array[1] + array[0]*array[1] - array[3];
		if ((r*r + i*i) >= 4){
			return n;
		}
		array[0] = r;
		array[1] = i;
	}
	return 0;
}
int **thr_matrix, **thr_colors;
int thr_yres;
int thr_depth;
long double delta_r;
long double delta_i;
long double thr_imin;
long double thr_rmin;

void Apple::freeMatrizen(int xres){
	for(int x=0; x<xres; x++){
		free(thr_matrix[x]);
		free(thr_colors[x]);
	}
	free(thr_matrix);
	free(thr_colors);
}
void Apple::allocMatrizen(int xres, int yres){
	thr_matrix = (int**)malloc(xres * sizeof(int*));
	thr_colors = (int**)malloc(xres * sizeof(int*));
	for(int i=0; i<paras.xres; i++){
		thr_matrix[i] = (int*)malloc(yres * sizeof(int));
		thr_colors[i] = (int*)malloc(yres * sizeof(int));;
	}
}

static void *thrFunc(void* val){
	int *x = (int*)val;
	for(int y=0; y<thr_yres; y++){
		thr_matrix[*x][y] = getIterCount(thr_rmin+(*x)*delta_r, thr_imin+y*delta_i, thr_depth);
		double div = (255.0/thr_depth) * thr_matrix[*x][y];
		int color = (int)div<<16 | (int)div<<8 | (int)div;
// hier koennen die Farben zu den Spektren hin verschoben werden
//		int color = (int)(div/3*3)<<16 | (int)(div/4*1)<<8 | (int)div/4*1;
		thr_colors[*x][y] = color;
	}
	return NULL;
}

void Apple::setDepth(int tiefe){
	paras.depth = tiefe;
}
int Apple::getDepth(){
	return paras.depth;
}

void Apple::calc(){// start <=13 Threads to fill AppleColors and AppleMatrix
//	ui->textFertig(false);
	thr_yres = paras.yres;
	thr_depth = paras.depth;
	delta_r = (paras.rmax - paras.rmin) / paras.xres;
	delta_i = (paras.imax - paras.imin) / paras.yres;
	thr_imin = paras.imin;
	thr_rmin = paras.rmin;
	int x = 0;
	int maxThr = 13;
	while((paras.xres % maxThr) != 0) maxThr--;
	while(x < paras.xres){
		pthread_t thrIds[maxThr];
		int tmp_x[maxThr];
		int aktThr = 0;
		while(aktThr < maxThr){
			tmp_x[aktThr] = x;
			if(pthread_create( &thrIds[aktThr], NULL, &thrFunc, (void *)tmp_x+aktThr*sizeof(int))){
				throw std::runtime_error{
					std::string{ "Failed to start thread " }
					+ std::strerror(errno)
				};
			}
			aktThr++;
			x++;
		}
		for(int id=0; id<maxThr; id++){
			pthread_join(thrIds[id], NULL);
		}
	}
	matrix = thr_matrix;
	colormatrix = thr_colors;
//	this->ui->textFertig(true);
}

bool mySortFunc(std::array<int, 2> a, std::array<int, 2> b){
	return a[1] < b[1];
}
void Apple::sort(){
	ui->textFertig(false);
	iterMembers.clear();
	oneMembers.clear();
	tenMembers.clear();
	for(int x=0; x<paras.xres; x++){
		for(int y=0; y<paras.yres; y++){
			bool inarray = false;
			for(int i=0; i<iterMembers.size(); i++){
				if(matrix[x][y] == iterMembers[i][0]){
					iterMembers[i][1]++;
					inarray = true;
					break;
				}
			}
			if(! inarray){
				iterMembers.push_back({matrix[x][y], 1});
			}
		}
	}
	std::sort(iterMembers.begin(), iterMembers.end(), mySortFunc);
	char iterText[17];
	int fgcolor = 0x00FFFFFF;
	int bgcolor = 0x00000000;
	sprintf(iterText, "I-Werte % 8d", iterMembers.size());
	ui->writeText(2, ui->height + 40, iterText, 16, fgcolor, bgcolor, 16, true);
	int sm0 = 0, sm1 = 0;
	for(int i=0; i<iterMembers.size(); i++){
		if(iterMembers[i][1] < 2){
			oneMembers.push_back(iterMembers[i][0]);}
		else if(iterMembers[i][1] < 11){
			tenMembers.push_back(iterMembers[i][0]);}
	}
	char text[2][17];
	sprintf(text[0], "oneMembers % 5d", oneMembers.size());
	sprintf(text[1], "tenMembers % 5d", tenMembers.size());
	ui->writeText(2, ui->height + 60, text[0], 16, fgcolor, bgcolor,16, true);
	ui->writeText(2, ui->height + 80, text[1], 16, fgcolor, bgcolor,16, true);
	ui->textFertig(true);
}

Apple::Apple(Userinterface &_ui, ApplePara data){
	ui = &_ui;
	paras = data;
	allocMatrizen(paras.xres, paras.yres);
	init(data, true);
}
Apple::~Apple(){
	freeMatrizen(paras.xres);
}

void Apple::clearScreen(){
	for(int x=paras.xpos; x<paras.xpos+paras.width; x++)
		for(int y=paras.ypos; y<paras.ypos+paras.height; y++) ui->display->putSpiegel(x, y, 0x00000000);
	ui->display->drawSpiegel(0, ui->display->bufferlenght);
}
void Apple::paint(){
	long double fak = (long double)paras.xres / (long double)paras.width;
	for(int x=paras.xpos; x<paras.xpos+paras.width; x++){
		for(int y=paras.ypos; y<paras.ypos+paras.height; y++){
			int cx = (x - paras.xpos)*fak;
			int cy = (y - paras.ypos)*fak;
			ui->display->putSpiegel(x, y, colormatrix[cx][cy]);
		}
	}
	ui->display->drawSpiegel(0,ui->display->bufferlenght);
}
void Apple::onMouseOver(int x, int y, int taste){
	double _rmin, _rmax, _imin, _imax;
	int newstartx, newendx, newstarty, newendy;
	int color = 0x00FF00FF;
	if(paras.width > paras.xres){
		newendx =  (x - paras.xpos) / ((double)paras.width / paras.xres);
		newstarty =  (y - paras.ypos) / ((double)paras.height / paras.yres);
	}
	else{
		newendx =  (x - paras.xpos) * ((double)paras.xres / paras.width);
		newstarty =  (y - paras.ypos) * ((double)paras.yres / paras.height);
	}
	_rmax = paras.rmin+((paras.rmax - paras.rmin) / paras.xres) * (newendx);
	_imin = paras.imin+((paras.imax - paras.imin) / paras.yres) * (newstarty);
	ui->textComplex(_rmax, _imin);
	if(taste == 1){//set start- and endpiont
		if(Startpoint[0] == 0){
			Startpoint[0] = x;
			Startpoint[1] = y;
		}
		else{
			Endpoint[0] = x;
			Endpoint[1] = y;
			int ydiff = ((Endpoint[0] - Startpoint[0]) * paras.height) / paras.width;
			if(ydiff < 0) ydiff = ydiff * -1;
			if(Endpoint[1] < Startpoint[1]) Endpoint[1] = Startpoint[1] - ydiff;
			else Endpoint[1] = Startpoint[1] + ydiff;
			if(Endpoint[0] > 0) ui->drawRect(Startpoint[0], Startpoint[1], Endpoint[0], Endpoint[1], color);
		}
	}
	else if(taste == 3 && Endpoint[0] > 0){//make new Apple
		if(Startpoint[0] == Endpoint[0] || Startpoint[1] == Endpoint[1]) return;
		if(Endpoint[0] < Startpoint[0]){
			int tmp = Startpoint[0];
			Startpoint[0] = Endpoint[0];
			Endpoint[0] = tmp;
		}
		if(Endpoint[1] < Startpoint[1]){
			int tmp = Startpoint[1];
			Startpoint[1] = Endpoint[1];
			Endpoint[1] = tmp;
		}
		
		if(paras.width > paras.xres){
			newstartx =  (Startpoint[0] - paras.xpos) / ((double)paras.width / paras.xres);
			newendx =  (Endpoint[0] - paras.xpos) / ((double)paras.width / paras.xres);
			newstarty =  (Startpoint[1] - paras.ypos) / ((double)paras.height / paras.yres);
			newendy =  (Endpoint[1] - paras.ypos) / ((double)paras.height / paras.yres);
		}
		else{
			newstartx =  (Startpoint[0] - paras.xpos) * ((double)paras.xres / paras.width);
			newendx =  (Endpoint[0] - paras.xpos) * ((double)paras.xres / paras.width);
			newstarty =  (Startpoint[1] - paras.ypos) * ((double)paras.yres / paras.height);
			newendy =  (Endpoint[1] - paras.ypos) * ((double)paras.yres / paras.height);
		}
		_rmin = paras.rmin+((paras.rmax - paras.rmin) / paras.xres) * newstartx;
		_rmax = paras.rmin+((paras.rmax - paras.rmin) / paras.xres) * newendx;
		_imin = paras.imin+((paras.imax - paras.imin) / paras.yres) * newstarty;
		_imax = paras.imin+((paras.imax - paras.imin) / paras.yres) * newendy;
		paras.rmin=_rmin; paras.rmax=_rmax; paras.imin=_imin; paras.imax=_imax;
		ui->setParas(paras);
		calc();
		paint();
		ui->callback(9);
	}
	else if(taste == 0){// clear Start- Endpoint
		Startpoint[0]=0;
		Startpoint[1]=0;
		Endpoint[0]=0;
		Endpoint[1]=0;
	}
}

void Apple::init(ApplePara data, bool _sort){
	
	freeMatrizen(paras.xres);
	//scale the Apple to fit in screen
	paras = data;
	int maxwidth = ui->display->xres - (ui->xpos + ui->width);
	int maxheight = ui->display->yres - Reglerheight;
	int xfak = 1;
	int divisor = 10;
	int tmpxres = paras.xres / divisor;
	int tmpfak = paras.xres / divisor;
	while(tmpxres < maxwidth){ xfak++; tmpxres = tmpfak * xfak;}
	xfak--;
	int yfak = 1;
	int tmpyres = paras.yres / divisor;
	tmpfak = paras.yres / divisor;
	while(tmpyres < maxheight){ yfak++; tmpyres = tmpfak * yfak;}
	yfak--;
	if(xfak < yfak) yfak = xfak;
	else xfak = yfak;
	paras.width = paras.xres / divisor * xfak;
	paras.height = paras.yres / divisor * yfak;
	paras.xpos = ui->display->xres - paras.width;
	paras.ypos = 0;
	
	//malloc memory for matrix and colormatrix
	allocMatrizen(paras.xres, paras.yres);
	calc();
	clearScreen();
	paint();
	if(_sort)sort();
}

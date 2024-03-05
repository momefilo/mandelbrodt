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
static void *thrFunc(void* val){
	int *x = (int*)val;
	for(int y=0; y<thr_yres; y++){
		thr_matrix[*x][y] = getIterCount(thr_rmin+(*x)*delta_r, thr_imin+y*delta_i, thr_depth);
		double div = (255.0/thr_depth) * thr_matrix[*x][y];
		int color = (int)div<<16 | (int)div<<8 | (int)div;
		thr_colors[*x][y] = color;
	}
	return NULL;
}

void _Apple::calc(){// start <=10 Threads to fill AppleColors and AppleMatrix
	ui->textFertig(false);
	thr_matrix = NULL;
	thr_colors = 
	thr_matrix = (int**)malloc(xres * sizeof(int**));
	thr_colors = (int**)malloc(xres * sizeof(int**));
	for(int i=0; i<xres; i++){
		thr_matrix[i] = (int*)malloc(yres * sizeof(int));
		thr_colors[i] = (int*)malloc(yres * sizeof(int));
	}
	thr_yres = yres;
	thr_depth = depth;
	delta_r = (rmax - rmin) / xres;
	delta_i = (imax - imin) / yres;
	thr_imin = imin;
	thr_rmin = rmin;
	int x = 0;
	int maxThr = 13;
	while((xres % maxThr) != 0) maxThr--;
	while(x < xres){
		pthread_t thrIds[maxThr];
		int tmp_x[maxThr];
		int aktThr = 0;
		while(aktThr < maxThr){
			tmp_x[aktThr] = x;
			pthread_create( &thrIds[aktThr], NULL, &thrFunc, (void *)tmp_x+aktThr*sizeof(int));
			aktThr++;
			x++;
		}
		for(int id=0; id<maxThr; id++){
			pthread_join(thrIds[id], NULL);
		}
	}
	memcpy(matrix, thr_matrix, xres*yres*sizeof(int));
	msync(matrix, xres*yres*sizeof(int), MS_SYNC);
	memcpy(colormatrix, thr_colors, xres*yres*sizeof(int));
	msync(colormatrix, xres*yres*sizeof(int), MS_SYNC);
	ui->textFertig(true);
}
void quicksort(int *number[],int first,int last){
	int i, j, pivot, temp1, temp2;
	if(first<last){
		pivot=first;
		i=first;
		j=last;

		while(i<j){
			while(number[i][1]<=number[pivot][1]&&i<last) i++;
			while(number[j][1]>number[pivot][1]) j--;
			if(i<j){
				temp1=number[i][0];
				temp2=number[i][1];
				number[i][0]=number[j][0];
				number[i][1]=number[j][1];
				number[j][0]=temp1;
				number[j][1]=temp2;
			}
		}

	temp1=number[pivot][0];
	temp2=number[pivot][1];
	number[pivot][0]=number[j][0];
	number[pivot][1]=number[j][1];
	number[j][0]=temp1;
	number[j][1]=temp2;
	quicksort(number,first,j-1);
	quicksort(number,j+1,last);
	}
}
void _Apple::sort(){
	ui->textFertig(false);
	countsOfIter = 0;
	iterMembers = NULL;
	oneMembers = NULL;
	tenMembers = NULL;
	for(int x=0; x<xres; x++){
		for(int y=0; y<yres; y++){
			bool inarray = false;
			for(int i=0; i<countsOfIter; i++){
				if(matrix[x][y] == iterMembers[i][0]){
					iterMembers[i][1]++;
					inarray = true;
					break;
				}
			}
			if(! inarray){
				if((countsOfIter % 199) == 0){
					if( !(iterMembers = (int**)realloc(iterMembers, (countsOfIter +200) * sizeof(int *)))){return;}
					for(int k=0; k<200; k++){
						if( !(iterMembers[countsOfIter+k] = (int*)malloc( (2) * sizeof(int)))){return;}
						iterMembers[countsOfIter+k][0] = -1;
						iterMembers[countsOfIter+k][1] = -1;
					}
					iterMembers[countsOfIter][0] = matrix[x][y];
					iterMembers[countsOfIter][1] = 1;
					countsOfIter++;
				}
				else{
					iterMembers[countsOfIter][0] = matrix[x][y];
					iterMembers[countsOfIter][1] = 1;
					countsOfIter++;
				}
			}
		}
	}
	quicksort(iterMembers,0,countsOfIter-1);
	char iterText[17];
	int fgcolor = 0x00FFFFFF;
	int bgcolor = 0x00000000;
	sprintf(iterText, "I-Werte % 8d", countsOfIter);
	ui->writeText(2, ui->height + 40, iterText, 16, fgcolor, bgcolor, 16, true);
	int sm0 = 0, sm1 = 0;
	for(int i=0; i<countsOfIter; i++){
		if(iterMembers[i][1] < 2){
			sm0++;
			if( !(oneMembers = (int*)realloc(oneMembers,(sm0 * sizeof(int))))){return;}
			oneMembers[sm0-1] = iterMembers[i][0];
		}
		else if(iterMembers[i][1] < 11){
			sm1++;
			if( !(tenMembers = (int*)realloc(tenMembers,(sm1 * sizeof(int))))){return;}
			tenMembers[sm1-1] = iterMembers[i][0];
		}
	}
	char text[6][16];
	sprintf(text[0], "One = % 10d", sm0);
	sprintf(text[1], "Ten = % 10d", sm1);
	sprintf(text[2], "Max = % 10d", iterMembers[countsOfIter-1][1]);
	sprintf(text[3], "Itr = % 10d", iterMembers[countsOfIter-1][0]);
	sprintf(text[4], "Max2= % 10d", iterMembers[countsOfIter-2][1]);
	sprintf(text[5], "Itr2= % 10d", iterMembers[countsOfIter-2][0]);
	ui->writeText(2, ui->height + 60, text[0], 16, fgcolor, bgcolor,16, true);
	ui->writeText(2, ui->height + 80, text[1], 16, fgcolor, bgcolor,16, true);
	ui->writeText(2, ui->height + 100, text[2], 16, fgcolor, bgcolor,16, true);
	ui->writeText(2, ui->height + 120, text[3], 16, fgcolor, bgcolor,16, true);
	ui->writeText(2, ui->height + 140, text[4], 16, fgcolor, bgcolor,16, true);
	ui->writeText(2, ui->height + 160, text[5], 16, fgcolor, bgcolor,16, true);
	ui->textFertig(true);
}

_Apple::_Apple(_Userinterface &_ui){
	ui = &_ui;	
}
_Apple::~_Apple(){
	free(thr_matrix);
	free(thr_colors);
	free(iterMembers);
	free(oneMembers);
	free(tenMembers);
	free(matrix);
	free(colormatrix);
}

void _Apple::clearScreen(){
	for(int x=xpos; x<xpos+width; x++)
		for(int y=ypos; y<ypos+height; y++) ui->display->putSpiegel(x, y, 0x00000000);
	ui->display->drawSpiegel(0, ui->display->bufferlenght);
}
void _Apple::paint(){
	long double fak = (long double)xres / (long double)width;
	for(int x=xpos; x<xpos+width; x++){
		for(int y=ypos; y<ypos+height; y++){
			int cx = (x - xpos)*fak;
			int cy = (y - ypos)*fak;
			ui->display->putSpiegel(x, y, colormatrix[cx][cy]);
		}
	}
	ui->display->drawSpiegel(0,ui->display->bufferlenght);
}
void _Apple::onMouseOver(int x, int y, int taste){
	double _rmin, _rmax, _imin, _imax;
	int newstartx, newendx, newstarty, newendy;
	int color = 0x00FF00FF;
	if(width > xres){
		newendx =  (x - xpos) / ((double)width / xres);
		newstarty =  (y - ypos) / ((double)height / yres);
	}
	else{
		newendx =  (x - xpos) * ((double)xres / width);
		newstarty =  (y - ypos) * ((double)yres / height);
	}
	_rmax = rmin+((rmax - rmin) / xres) * (newendx);
	_imin = imin+((imax - imin) / yres) * (newstarty);
	ui->textComplex(_rmax, _imin);
	if(taste == 1){//set start- and endpiont
		if(Startpoint[0] == 0){
			Startpoint[0] = x;
			Startpoint[1] = y;
		}
		else{
			Endpoint[0] = x;
			Endpoint[1] = y;
			int ydiff = ((Endpoint[0] - Startpoint[0]) * height) / width;
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
		
		if(width > xres){
			newstartx =  (Startpoint[0] - xpos) / ((double)width / xres);
			newendx =  (Endpoint[0] - xpos) / ((double)width / xres);
			newstarty =  (Startpoint[1] - ypos) / ((double)height / yres);
			newendy =  (Endpoint[1] - ypos) / ((double)height / yres);
		}
		else{
			newstartx =  (Startpoint[0] - xpos) * ((double)xres / width);
			newendx =  (Endpoint[0] - xpos) * ((double)xres / width);
			newstarty =  (Startpoint[1] - ypos) * ((double)yres / height);
			newendy =  (Endpoint[1] - ypos) * ((double)yres / height);
		}
		_rmin = rmin+((rmax - rmin) / xres) * newstartx;
		_rmax = rmin+((rmax - rmin) / xres) * newendx;
		_imin = imin+((imax - imin) / yres) * newstarty;
		_imax = imin+((imax - imin) / yres) * newendy;
		rmin=_rmin; rmax=_rmax; imin=_imin; imax=_imax;
		ui->setWert(2,rmin);
		ui->setWert(3,rmax);
		ui->setWert(4,imin);
		ui->setWert(5,imax);
		ui->updateWerte();
		calc();
		paint();
	}
	else if(taste == 0){// clear Start- Endpoint
		Startpoint[0]=0;
		Startpoint[1]=0;
		Endpoint[0]=0;
		Endpoint[1]=0;
	}
}

void _Apple::init(){
	int maxwidth = ui->display->xres - (ui->xpos + ui->width);
	int maxheight = ui->display->yres - Reglerheight;
	int xfak = 1;
	int divisor = 10;
	int tmpxres = xres / divisor;
	int tmpfak = xres / divisor;
	while(tmpxres < maxwidth){ xfak++; tmpxres = tmpfak * xfak;}
	xfak--;
	int yfak = 1;
	int tmpyres = yres / divisor;
	tmpfak = yres / divisor;
	while(tmpyres < maxheight){ yfak++; tmpyres = tmpfak * yfak;}
	yfak--;
	if(xfak < yfak) yfak = xfak;
	else xfak = yfak;
	width = xres / divisor * xfak;
	height = yres / divisor * yfak;
	xpos = ui->display->xres - width;
	ypos = 0;
	
	matrix = (int**)malloc(xres * sizeof(int*));
	for(int i=0; i<xres; i++)
		matrix[i] = (int*)malloc(yres * sizeof(int));
	colormatrix = (int**)malloc(xres * sizeof(int*));
	for(int i=0; i<xres; i++)
		colormatrix[i] = (int*)malloc(yres * sizeof(int));
}
void _Apple::init(int _xres, int _yres, 
					long double _rmin,
					long double _rmax,
					long double _imin,
					long double _imax){
	xres = _xres;
	yres = _yres;
	rmin = _rmin;
	rmax = _rmax;
	imin = _imin;
	imax = _imax;
	init();
}

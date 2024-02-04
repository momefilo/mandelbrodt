#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/select.h>
#include "gui_element.h"

// the size and variables of the applepicture
struct _AppleGui AppleGui;

// Variablen
int TextOffset = 330, GuiX = 0, GuiY = 0;
bool GuiInit = false;
int Startpoint[2], Endpoint[2]; // Zoomvariablen
int BufferLenght, LineLenght;// Displayvariablen
double Xres, Yres;// Displayvariablen
uint32_t *Fbp; // HardwareFB
uint8_t ***MyFB;// HardwareFB Spiegel
uint8_t ***MyApple; // Farbwerte des des auf AppleGui skalierten Apfelmaenchen
int **IterMatrix; // Iterationswerte aller Pixel des unskalierten Apfelmaenchen
int CountsOfIter;// Anzahl der Iterationswerte
int **IterMember;//[CountsOfIter][2] 0= anzahl Iterationswerte, 1= Anzahl Member der Iter-werte

// writes a in Parameters defined Area from MyFB to Hardware-FB (the Screen)
void *writeToFb(int x, int y, int width, int height){
	#define FBP(myx, myy) Fbp[myx + myy*(LineLenght >> 2)]
	for (int myx=x; myx<(x+width); myx++){
		for (int myy=y; myy<(y+height); myy++){
			FBP(myx, myy) = MyFB[myx][myy][2] | (MyFB[myx][myy][1] << 8) | (MyFB[myx][myy][0] << 16);
		}
	}
}


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

//statusausgabe
void textFertig(bool fertig){
	uint8_t *text = "Rechne";
	if(fertig) text = "Fertig";
	if(GuiInit){
		uint8_t fgcolor[] = {255,255,255};
		uint8_t bgcolor[] = {0,0,0};
		uint8_t *leer = "                ";
		write_Text(2, TextOffset-16, leer, 6, fgcolor, bgcolor);
		write_Text(2, TextOffset-16, text, 6, fgcolor, bgcolor);
		writeToFb(2, TextOffset-16, 16*16, 16);
	}
}

/* helper-Functions for makeApfel(). Initialisiert restliche globalel Variablen welche nicht in Main
 * initialisiert wurden */
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
void *fillIterMember(){
	textFertig(false);
	int CountsOfIter = 0;
	for(int x=0; x<AppleGui.xres; x++){
		for(int y=0; y<AppleGui.yres; y++){
			bool inarray = false;
			for(int i=0; i<CountsOfIter; i++){
				if(IterMatrix[x][y] == IterMember[i][0]){
					IterMember[i][1]++;
					inarray = true;
					break;
				}
			}
			if(! inarray){
				if((CountsOfIter % 199) == 0){
					if( !(IterMember = realloc(IterMember, (CountsOfIter +200) * sizeof(int *)))){
						printf("Speicherfehler IterMember-Counts1\n");
						return;}
					for(int k=0; k<200; k++){
						if( !(IterMember[CountsOfIter+k] = malloc( (2) * sizeof(int)))){
							printf("Speicherfehler IterMember-Counts2\n");
							return;}
						IterMember[CountsOfIter+k][0] = -1;
						IterMember[CountsOfIter+k][1] = -1;
					}
					IterMember[CountsOfIter][0] = IterMatrix[x][y];
					IterMember[CountsOfIter][1] = 1;
					CountsOfIter++;
				}
				else{
					IterMember[CountsOfIter][0] = IterMatrix[x][y];
					IterMember[CountsOfIter][1] = 1;
					CountsOfIter++;
				}
			}
		}
	}
	quicksort(IterMember,0,CountsOfIter-1);
	uint8_t iterText[17];
	uint8_t fgcolor[] = {255,255,255};
	uint8_t bgcolor[] = {0,0,0};
	sprintf(iterText, "I-Werte % 8d", CountsOfIter);
	write_Text(2, TextOffset - 40, iterText, 16, fgcolor, bgcolor);
	writeToFb(2, TextOffset - 40, 16*16, 16);
	textFertig(true);
}

/* Füllt MyApple mit Daten aus getIterCount() mithilfe der vier *fillBufX() Threadfuntionen und
 * skaliert diese mit AppleGui auf <= 5/4 des Displays und
 * schreibt die Skalierten Daten in MyFB und
 * sendet diese mit writeToFb() auf das Display*/
static void *thrFunc(void* val){
	int *x = (int*)val;
	double delta_r = (AppleGui.rmax - AppleGui.rmin) / (AppleGui.xres - 1);
	double delta_i = (AppleGui.imax - AppleGui.imin) / (AppleGui.yres - 1);
	for(int y=0; y<(AppleGui.yres); y++){
		IterMatrix[*x][y] = getIterCount((AppleGui.rmin)+(*x)*delta_r, (AppleGui.imin)+y*delta_i, (AppleGui.depth));
		double div = (255.0/(AppleGui.depth)) * IterMatrix[*x][y];
		uint8_t cbyte = div;
		for(int l=0; l<3; l++) MyApple[*x][y][l] = cbyte;
	}
	return NULL;
}
void *makeApfel(){
	textFertig(false);
	if(1){// alloc Memory for MyApple and IterMatrix
		if( !(MyApple = realloc(MyApple, (AppleGui.xres * sizeof(uint8_t **))))){
			printf("Speicherfehler Zeilen\n");
			return;}
		for(int i=0; i<AppleGui.xres; i++){
			if( !(MyApple[i] = realloc(MyApple[i],(AppleGui.yres * sizeof(uint8_t *))))){
				printf("Speicherfehler Spalte %d\n", i);
				return;}}
		for(int i=0; i<AppleGui.xres; i++){
			for(int k=0; k<AppleGui.yres; k++){
				if( !(MyApple[i][k] = realloc(MyApple[i][k],(3 * sizeof(uint8_t))))){
					printf("Speicherfehler Zeile=%d Spalte=%d\n", i, k);
					return;}
				for(int l=0; l<3; l++) MyApple[i][k][l] = 0;}}
		if( !(IterMatrix = realloc(IterMatrix, (AppleGui.xres * sizeof(int **))))){
			printf("Speicherfehler IterMatrix-Zeilen\n");
			return;}
		for(int i=0; i<AppleGui.xres; i++){
		if( !(IterMatrix[i] = realloc(IterMatrix[i],(AppleGui.yres * sizeof(int *))))){
			printf("Speicherfehler Spalte %d\n", i);
			return;}}
	}
	
	if(1){//start <=100 Threads to fill MyApple and IterMatrix
		int x = 0;
		double prozent = 0;
		int maxThr = 100;
		while((AppleGui.xres % maxThr) != 0) maxThr--;
		while(x < AppleGui.xres){
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
			prozent++;
			double tmp = (100/(double)(AppleGui.xres/maxThr)) * prozent;
		}
	}

	int x_fak = 1, y_fak = 1;
	if(1){//scale the MyApple and AppleGui to fit in 5/4 of the full-screen
		int tmpxres = AppleGui.xres;
		if(AppleGui.xres > (Xres/5*4)){
			while(tmpxres > (Xres/5*4)) {x_fak++;tmpxres = AppleGui.xres / x_fak;}
			x_fak = -x_fak;
		}
		else{
			while((Xres/5*4) > tmpxres) {x_fak++;tmpxres = AppleGui.xres * x_fak;}
			if(tmpxres > (Xres/5*4)) x_fak--;
		}
		int tmpyres = AppleGui.yres;
		if(AppleGui.yres > (Yres/5*4)){
			while(tmpyres > (Yres/5*4)) {y_fak++;tmpyres = AppleGui.yres / y_fak;}
			y_fak = -y_fak;
		}
		else{
			while((Yres/5*4) > tmpyres) {y_fak++;tmpyres = AppleGui.yres * y_fak;}
			if(tmpyres > (Yres/5*4)) y_fak--;
		}
		
		if(x_fak < y_fak) y_fak = x_fak;
		else x_fak = y_fak;
		
		if(x_fak > 0 && y_fak > 0){
			AppleGui.width = AppleGui.xres * x_fak;
			AppleGui.height = AppleGui.yres * x_fak;
		}
		else if(x_fak < 0 && y_fak < 0){
			AppleGui.width = AppleGui.xres / (x_fak * -1);
			AppleGui.height = AppleGui.yres / (x_fak * -1);
		}
		else printf("Es werden nur Querformate verarbeitet\n");
		
		AppleGui.x = Xres - AppleGui.width;
		AppleGui.y = 0;
	}
	
	if(x_fak < 0){//write MyApple to MyFB
		x_fak = -1*x_fak;
		for (int x=AppleGui.x; x<(AppleGui.x+AppleGui.width); x++){
			for(int y=AppleGui.y; y<(AppleGui.y+AppleGui.height); y++){
				for(int l=0; l<3; l++) MyFB[x][y][l] = MyApple[(x-AppleGui.x) * x_fak][(y-AppleGui.y) * x_fak][l];
			}
		}
	}
	else{
		for (int x=AppleGui.x; x<(AppleGui.x+AppleGui.width); x++){
			for(int y=AppleGui.y; y<(AppleGui.y+AppleGui.height); y++){
				for(int l=0; l<3; l++) MyFB[x][y][l] = MyApple[(x-AppleGui.x)/x_fak][(y-AppleGui.y)/y_fak][l];
			}
		}
	}
	
	//write MyFB to Display
	writeToFb(AppleGui.x,AppleGui.y,AppleGui.width,AppleGui.height);
	textFertig(true);
}

//GUI-Zoomfunktionen
void textComplex(double r, double i){
	if(GuiInit){
		uint8_t text[2][30];
		uint8_t fgcolor[] = {255,255,255};
		uint8_t bgcolor[] = {0,0,0};
		sprintf(text[0], "r = %.24f", r);
		sprintf(text[1], "i = %.24f", i);
		write_Text(AppleGui.x, AppleGui.height, text[0], 30, fgcolor, bgcolor);
		write_Text(AppleGui.x, AppleGui.height + 24, text[1], 30, fgcolor, bgcolor);
		writeToFb(AppleGui.x, AppleGui.height + 40, 16*16, 40);
	}
}
void drawRect(uint32_t color){
	int xstart = Startpoint[0], xend = Endpoint[0], ystart = Startpoint[1], yend = Endpoint[1];
	if(xend < xstart){
		int tmp = xstart;
		xstart = xend;
		xend = tmp;
	}
	if(yend < ystart){
		int tmp = ystart;
		ystart = yend;
		yend = tmp;
	}
	//obere Linie
	for(int x=xstart; x<xend; x++)FBP(x, ystart) = color & (0x00FFFFFF);
	//untere Linie
	for(int x=xstart; x<xend; x++)FBP(x, yend) = color & (0x00FFFFFF);
	//linke Linie
	for(int y=ystart; y<yend; y++)FBP(xstart, y) = color & (0x00FFFFFF);
	//rechte Linie
	for(int y=ystart; y<yend; y++)FBP(xend, y) = color & (0x00FFFFFF);
}
void mouseOverApplegui(int x, int y, uint8_t button){
	double rmin, rmax, imin, imax;
	int newstartx, newendx, newstarty, newendy;
	if(AppleGui.width > AppleGui.xres){
		newendx =  (x-AppleGui.x) / (AppleGui.width/AppleGui.xres);
		newstarty =  (y-AppleGui.y) / (AppleGui.height/AppleGui.yres);
	}
	else{
		newendx =  (x-AppleGui.x) * (AppleGui.xres/AppleGui.width);
		newstarty =  (y-AppleGui.y) * (AppleGui.yres/AppleGui.height);
	}
	rmax = AppleGui.rmin+((AppleGui.rmax - AppleGui.rmin) / AppleGui.xres) * (newendx);
	imin = AppleGui.imin+((AppleGui.imax - AppleGui.imin) / AppleGui.yres) * (newstarty);
	textComplex(rmax, imin);
	if(button == 1){//set start- and endpiont
		if(Startpoint[0] == 0){
			Startpoint[0] = x;
			Startpoint[1] = y;
		}
		else{
			Endpoint[0] = x;
			Endpoint[1] = y;
			int ydiff = ((Endpoint[0] - Startpoint[0]) * AppleGui.height) / (AppleGui.width);
			if(ydiff < 0) ydiff = ydiff * -1;
			if(Endpoint[1] < Startpoint[1]) Endpoint[1] = Startpoint[1] - ydiff;
			else Endpoint[1] = Startpoint[1] + ydiff;
			if(Endpoint[0] > 0) drawRect(0x00FF00FF);
		}
	}
	else if(button == 3 && Endpoint[0] > 0){//make new Apple
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
		
		if(AppleGui.width > AppleGui.xres){
			newstartx =  (Startpoint[0]-AppleGui.x) / (AppleGui.width/AppleGui.xres);
			newendx =  (Endpoint[0]-AppleGui.x) / (AppleGui.width/AppleGui.xres);
			newstarty =  (Startpoint[1]-AppleGui.y) / (AppleGui.height/AppleGui.yres);
			newendy =  (Endpoint[1]-AppleGui.y) / (AppleGui.height/AppleGui.yres);
		}
		else{
			newstartx =  (Startpoint[0]-AppleGui.x) * (AppleGui.xres/AppleGui.width);
			newendx =  (Endpoint[0]-AppleGui.x) * (AppleGui.xres/AppleGui.width);
			newstarty =  (Startpoint[1]-AppleGui.y) * (AppleGui.yres/AppleGui.height);
			newendy =  (Endpoint[1]-AppleGui.y) * (AppleGui.yres/AppleGui.height);
		}
		rmin = AppleGui.rmin+((AppleGui.rmax - AppleGui.rmin) / AppleGui.xres) * (newstartx);
		rmax = AppleGui.rmin+((AppleGui.rmax - AppleGui.rmin) / AppleGui.xres) * (newendx);
		imin = AppleGui.imin+((AppleGui.imax - AppleGui.imin) / AppleGui.yres) * (newstarty);
		imax = AppleGui.imin+((AppleGui.imax - AppleGui.imin) / AppleGui.yres) * (newendy);
		AppleGui.rmin=rmin; AppleGui.rmax=rmax;AppleGui.imin=imin;AppleGui.imax=imax;
		makeApfel();
		updateWerte();
	}
	else if(button == 0){// clear Start- Endpoint
		Startpoint[0]=0;
		Startpoint[1]=0;
		Endpoint[0]=0;
		Endpoint[1]=0;
	}
}

void myLoop(){
	//maus
	int fdm = open("/dev/input/mice", O_RDONLY);
	if (fdm == -1) {
		printf("ERROR: /dev/input/mice konnte nicht geoeffnet werden. Sind Sie root?\n");
		exit(EXIT_FAILURE);
	}
	int maus_x = Xres/2, maus_y = Yres/2;
	char paket[3];
	char text[1];
	while (1) {
		// Datenpaket einlesen
		read(fdm, paket, 3);
		maus_x += paket[1];
		maus_y += paket[2];
		if (maus_x < 0) maus_x = 0;
		if (maus_y < 0) maus_y = 0;
		if (maus_x >= Xres) maus_x = Xres - 1;
		if (maus_y >= Yres) maus_y = Yres - 1;

		// Status der Maustasten dekodieren
		char left   = paket[0] & 1 ? 'L' : ' ';
		char right  = paket[0] & 2 ? 'R' : ' ';
//		char middle = paket[0] & 4 ? 'M' : ' ';

		int tmp_height = ElemHeight * (ElemCount/ElemsInRow);
		if(ElemCount%ElemsInRow != 0) tmp_height = tmp_height + ElemHeight;
		writeToFb(0,0,Xres,Yres);
		for(int i=0; i<6; i++){
			for(int j=0; j<6; j++){
				int myy = (Yres-(maus_y-j));
				if(myy < 0) myy = 0;
				if(myy >= Yres) myy = Yres-1;
				int myx = maus_x+i;
				if(myx < 0) myx = 0;
				if(myx >= Xres) myx = Xres-1;
				FBP(myx, myy) = 0xFF00FF;
			}
		}
		//AppleGui
		if(maus_x >= AppleGui.x && (Yres-maus_y) <= AppleGui.height){
			uint8_t button = 0;
			if(left!=' ') button = 1;
			if(right!=' ') button = button + 2;
			mouseOverApplegui(maus_x, (Yres-maus_y), button);
		}
		//gui_element
		else if(maus_x > GuiX && maus_x < (GuiX+ElemWidth*ElemsInRow)
			&& (Yres-maus_y) > GuiY && (Yres-maus_y) < (GuiY+tmp_height) ){
			uint8_t button = 0;
			if(left!=' ') button = 1;
			if(right!=' ') button = button + 2;
			gui_onMouseOver(maus_x, (Yres-maus_y), button);
		}
		else if( (paket[0] & 2) && (!(paket[0] & 1))) break;
		
	}
}

int main(){
	for(int i=0; i<100; i++)printf("\n");
	if(1){ // inits AppleGui
		for(int i=0; i<2; i++){ // Start- und Endpiont
			Startpoint[i] = 0;
			Endpoint[i] = 0;
		}
		AppleGui.xres = 1920/5*4;
		AppleGui.yres = 1080/5*4;
		AppleGui.rmin = -1.0;
		AppleGui.rmax = 2.0;
		AppleGui.imin = -1.0;
		AppleGui.imax = 1.0;
		AppleGui.depth = 1000;
	}
	if(1){ //Fbp and Displayvariables
		struct fb_fix_screeninfo finfo;
		struct fb_var_screeninfo vinfo;
		int fd = open("/dev/fb0", O_RDWR);
		if (fd == -1) {
			fprintf(stderr, "ERROR: /dev/fb0 konnte nicht geöffnet werden\n");
			exit(EXIT_FAILURE);}
		if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) != 0) {
			printf("ERROR: FBIOGET_FSCREENINFO konnte nicht abgefragt werden\n");
			exit(EXIT_FAILURE);}
		if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) != 0) {
			printf("ERROR: FBIOGET_VSCREENINFO konnte nicht abgefragt werden\n");
			exit(EXIT_FAILURE);}
		Fbp = mmap(NULL, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (Fbp == NULL) {
			printf("ERROR: mmap fehlgeschlagen\n");
			exit(EXIT_FAILURE);
		}
		close(fd);
		BufferLenght = finfo.smem_len;
		LineLenght = finfo.line_length;
		Xres = vinfo.xres;
		Yres = vinfo.yres;
	}
	if(1){ // malloc MyFB
		MyFB = malloc(Xres * sizeof(uint8_t **));
		if(NULL == MyFB) {
			printf("Kein Speicher mehr fuer Zeilen");
			return EXIT_FAILURE;
		}
		for(int i=0; i<Xres; i++){
			MyFB[i] = malloc(Yres * sizeof(uint8_t *));
			if(NULL == MyFB[i]) {
				printf("Kein Speicher mehr fuer Spalte %d\n",i);
				return EXIT_FAILURE;
			}
		}
		for(int i=0; i<Xres; i++){
			for(int k=0; k<Yres; k++){
				MyFB[i][k] = malloc(3 * sizeof(uint8_t));
				if(NULL == MyFB[i][k]) {
					printf("Kein Speicher mehr fuer Farbe %d, %d\n",i, k);
					return EXIT_FAILURE;
				}
				for(int l=0; l<3; l++) MyFB[i][k][l] = 0;
			}
		}
	}

	makeApfel();
	
	gui_init(GuiX, GuiY, &AppleGui, MyFB, &writeToFb, &makeApfel, &fillIterMember);
	GuiInit = true;
//	fillIterMember();
	
	myLoop();
	
	//speicher freigeben
	free(MyFB);
	free(MyApple);
	free(IterMatrix);
	free(IterMember);
	munmap(Fbp, BufferLenght);
}

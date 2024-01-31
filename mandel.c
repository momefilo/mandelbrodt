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
//#include <limits.h>
//#include <sys/types.h>
//#include <sys/stat.h>

// the size of the Applepicture
struct _AppleGui {
	int x;
	int y;
	int width;
	int height;
} AppleGui;

// Variablen
int Startpoint[2], Endpoint[2]; // Zoomvariablen
int BufferLenght, Bpp, LineLenght;// Displayvariablen
double Xres, Yres;// Displayvariablen
int Applewidth, Appleheight, Depth;// Apfelmännchenvariablen
double RMin, RMax, IMin, IMax;// Apfelmännchenvariablen
uint32_t *Fbp; // HardwareFB
uint8_t ***MyFB;// HardwareFB Spiegel
uint8_t ***MyApple; // Farbwerte des des auf AppleGui skalierten Apfelmaenchen
int **IterMatrix; // Iterationswerte aller Pixel des unskalierten Apfelmaenchen
int CountsOfIter;// Anzahl der Iterationswerte
int **IterMember;//[CountsOfIter][2] 0= anzahl Iterationswerte, 1= Anzahl Member der Iter-werte

// writes a in Parameters defined Area from MyFB to Hardware-FB (the Screen)
void writeToFb(int x, int y, int width, int height){
	#define FBP(myx, myy) Fbp[myx + myy*(LineLenght >> 2)]
	for (int myx=x; myx<(x+width); myx++){
		for (int myy=y; myy<(y+height); myy++){
			FBP(myx, myy) = MyFB[myx][myy][2] | (MyFB[myx][myy][1] << 8) | (MyFB[myx][myy][0] << 16);
		}
	}
}

// test
void drawGraphic(){
	FILE *gr_file;
	gr_file=fopen("/home/momefilo/mandelbrodt/graphics/regler.data","rb");
	if(gr_file){
		int width = 40, height = 40;
		for(int x=0; x<width; x++){
			for(int y=0; y<height; y++){
				uint8_t color[3];
				fread(color, sizeof(uint8_t), 3, gr_file);
				for(int i=0; i<3; i++)MyFB[x][y][i] = color[i];
			}
		}
		writeToFb(0, 0, width, height);
	}
	fclose(gr_file);
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
void fillIterMember(){
	int CountsOfIter = 0;
	for(int x=0; x<Applewidth; x++){
		for(int y=0; y<Appleheight; y++){
			bool inarray = false;
			for(int i=0; i<CountsOfIter; i++){
				if(IterMatrix[x][y] == IterMember[i][0]){
					IterMember[i][1]++;
					inarray = true;
					//printf("inarray\n");
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
	for(int i=0; i<CountsOfIter; i++)printf(" A i=%d, c=%d, m=%d\n",i, IterMember[i][0], IterMember[i][1]);
	quicksort(IterMember,0,CountsOfIter-1);
	for(int i=0; i<CountsOfIter; i++)printf(" B i=%d, 0=%d, 1=%d\n",i, IterMember[i][0], IterMember[i][1]);
	printf("CountsOfIter = %d\n",CountsOfIter);
}
/* thread-Functions for makeApfel() jede fuellt 1/4
 * MyApple in Graustufen */
static void *fillBuf1(void* val){
	double delta_r = (RMax - RMin) / (Applewidth - 1);
	double delta_i = (IMax - IMin) / (Appleheight - 1);
	for(int x=0; x<(Applewidth/8*2); x++){
		for(int y=0; y<(Appleheight); y++){
			IterMatrix[x][y] = getIterCount((RMin)+x*delta_r, (IMin)+y*delta_i, (Depth));
			double div = (255.0/(Depth)) * IterMatrix[x][y];
			uint8_t cbyte = div;
			for(int l=0; l<3; l++) MyApple[x][y][l] = cbyte;
		}
	}
	printf("th_1 end\n");
	return NULL;
}
static void *fillBuf2(void* val){
	double delta_r = (RMax - RMin) / (Applewidth - 1);
	double delta_i = (IMax - IMin) / (Appleheight - 1);
	for(int x=(Applewidth/8*2); x<(Applewidth/8*3); x++){
		for(int y=0; y<(Appleheight); y++){
			IterMatrix[x][y] = getIterCount((RMin)+x*delta_r, (IMin)+y*delta_i, (Depth));
			double div = (255.0/(Depth)) * IterMatrix[x][y];
			uint8_t cbyte = div;
			for(int l=0; l<3; l++) MyApple[x][y][l] = cbyte;
		}
	}
	printf("th_2 end\n");
	return NULL;
}
static void *fillBuf3(void* val){
	double delta_r = (RMax - RMin) / (Applewidth - 1);
	double delta_i = (IMax - IMin) / (Appleheight - 1);
	for(int x=(Applewidth/8*3); x<(Applewidth/8*4); x++){
		for(int y=0; y<(Appleheight); y++){
			IterMatrix[x][y] = getIterCount((RMin)+x*delta_r, (IMin)+y*delta_i, (Depth));
			double div = (255.0/(Depth)) * IterMatrix[x][y];
			uint8_t cbyte = div;
			for(int l=0; l<3; l++) MyApple[x][y][l] = cbyte;
		}
	}
	printf("th_3 end\n");
	return NULL;
}
static void *fillBuf4(void* val){
	double delta_r = (RMax - RMin) / (Applewidth - 1);
	double delta_i = (IMax - IMin) / (Appleheight - 1);
	for(int x=(Applewidth/8*4); x<(Applewidth); x++){
		for(int y=0; y<(Appleheight); y++){
			IterMatrix[x][y] = getIterCount((RMin)+x*delta_r, (IMin)+y*delta_i, (Depth));
			double div = (255.0/(Depth)) * IterMatrix[x][y];
			uint8_t cbyte = div;
			for(int l=0; l<3; l++) MyApple[x][y][l] = cbyte;
		}
	}
	printf("th_4 end\n");
	return NULL;
}

/* Füllt MyApple mit Daten aus getIterCount() mithilfe der vier *fillBufX() Threadfuntionen und
 * skaliert diese mit AppleGui auf <= 5/4 des Displays und
 * schreibt die Skalierten Daten in MyFB und
 * sendet diese mit writeToFb() auf das Display*/
void makeApfel(/*double xres, double yres, double min_r, double max_r, double min_i, double max_i, double depth*/){
	// alloc Memory for MyApple
	if( !(MyApple = realloc(MyApple, (Applewidth * sizeof(uint8_t **))))){
		printf("Speicherfehler Zeilen\n");
		return;}
	for(int i=0; i<Applewidth; i++){
		if( !(MyApple[i] = realloc(MyApple[i],(Appleheight * sizeof(uint8_t *))))){
			printf("Speicherfehler Spalte %d\n", i);
			return;}}
	for(int i=0; i<Applewidth; i++){
		for(int k=0; k<Appleheight; k++){
			if( !(MyApple[i][k] = realloc(MyApple[i][k],(3 * sizeof(uint8_t))))){
				printf("Speicherfehler Zeile=%d Spalte=%d\n", i, k);
				return;}
			for(int l=0; l<3; l++) MyApple[i][k][l] = 0;}}
			
	// alloc Memory for IterMatrix
	if( !(IterMatrix = realloc(IterMatrix, (Applewidth * sizeof(int **))))){
		printf("Speicherfehler IterMatrix-Zeilen\n");
		return;}
	for(int i=0; i<Applewidth; i++){
		if( !(IterMatrix[i] = realloc(IterMatrix[i],(Appleheight * sizeof(int *))))){
			printf("Speicherfehler Spalte %d\n", i);
			return;}}
	
	//starts four threads to getIterCount() to fill MyApple with Color-Data and IterMatrix with itercounts
	pthread_t thread1, thread2, thread3, thread4;
	double val[] = {Applewidth, Appleheight, RMax, RMin, IMax, IMin, Depth};
	if(pthread_create( &thread1, NULL, &fillBuf1, (void*)val)) printf("Konnte Thread1 nicht starten\n");
	if(pthread_create( &thread2, NULL, &fillBuf2, (void*)val)) printf("Konnte Thread2 nicht starten\n");
	if(pthread_create( &thread3, NULL, &fillBuf3, (void*)val)) printf("Konnte Thread2 nicht starten\n");
	if(pthread_create( &thread4, NULL, &fillBuf4, (void*)val)) printf("Konnte Thread2 nicht starten\n");
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	pthread_join(thread4, NULL);
	
	//scale the MyApple and AppleGui to fit in 5/4 of the full-screen
	int x_fak = 1, y_fak = 1;
	int tmpxres = Applewidth;
	if(Applewidth > (Xres/5*4)){
		while(tmpxres > (Xres/5*4)) {x_fak++;tmpxres = Applewidth / x_fak;}
		x_fak = -x_fak;
	}
	else{
		while((Xres/5*4) > tmpxres) {x_fak++;tmpxres = Applewidth * x_fak;}
		if(Applewidth > (Xres/5*4)) x_fak--;
	}
	int tmpyres = Appleheight;
	if(Appleheight > (Yres/5*4)){
		while(tmpyres > (Yres/5*4)) {y_fak++;tmpyres = Appleheight / y_fak;}
		y_fak = -y_fak;
	}
	else{
		while((Yres/5*4) > tmpyres) {y_fak++;tmpyres = Appleheight * y_fak;}
		if(Appleheight > (Yres/5*4)) y_fak--;
	}
	
	if(x_fak < y_fak) y_fak = x_fak;
	else x_fak = y_fak;
	
	if(x_fak > 0 && y_fak > 0){
		AppleGui.width = Applewidth * x_fak;
		AppleGui.height = Appleheight * x_fak;
	}
	else if(x_fak < 0 && y_fak < 0){
		AppleGui.width = Applewidth / (x_fak * -1);
		AppleGui.height = Appleheight / (x_fak * -1);
	}
	else printf("Es werden nur Querformate verarbeitet\n");
	
	AppleGui.x = Xres - AppleGui.width;
	AppleGui.y = 0;
	//write MyApple to MyFB
	if(x_fak < 0){
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
	
	fillIterMember();
}

//GUI-Funktionen
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
	if(button == 1){//set start- and endpiont
		if(Startpoint[0] == 0){
			Startpoint[0] = x;
			Startpoint[1] = y;
		}
		else{
			Endpoint[0] = x;
			Endpoint[1] = y;
		}
		int fak = (Endpoint[0]-Startpoint[0]) / (RMax-RMin) * (IMax-IMin);
		Endpoint[1] = (Startpoint[1]) + fak;
		if(Endpoint[0] > 0) drawRect(0x00FF00FF);
	}
	else if(button == 2){// clear start- and endpiont
		for(int i=0; i<2; i++){
			Startpoint[i] = 0;
			Endpoint[i] = 0;
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
		
		if(AppleGui.width > Applewidth){
			int newstartx =  (Startpoint[0]-AppleGui.x) / (AppleGui.width/Applewidth);
			int newendx =  (Endpoint[0]-AppleGui.x) / (AppleGui.width/Applewidth);
			int newstarty =  (Startpoint[1]-AppleGui.y) / (AppleGui.height/Appleheight);
			int newendy =  (Endpoint[1]-AppleGui.y) / (AppleGui.height/Appleheight);
			double rmin = RMin+((RMax - RMin) / Applewidth) * (newstartx);
			double rmax = RMin+((RMax - RMin) / Applewidth) * (newendx);
			double imin = IMin+((IMax - IMin) / Appleheight) * newstarty;
			double imax = IMin+((IMax - IMin) / Appleheight) * newendy;
			makeApfel(Applewidth, Appleheight, rmin, rmax, imin, imax, Depth);
		}
		else{
			int newstartx =  (Startpoint[0]-AppleGui.x) * (Applewidth/AppleGui.width);
			int newendx =  (Endpoint[0]-AppleGui.x) * (Applewidth/AppleGui.width);
			int newstarty =  (Startpoint[1]-AppleGui.y) * (Appleheight/AppleGui.height);
			int newendy =  (Endpoint[1]-AppleGui.y) * (Appleheight/AppleGui.height);
			double rmin = RMin+((RMax - RMin) / Applewidth) * (newstartx);
			double rmax = RMin+((RMax - RMin) / Applewidth) * (newendx);
			double imin = IMin+((IMax - IMin) / Appleheight) * (newstarty);
			double imax = IMin+((IMax - IMin) / Appleheight) * (newendy);
			RMin=rmin; RMax=rmax;IMin=imin;IMax=imax;
			makeApfel();
		}
	}
	else if(button == 0){
		Startpoint[0]=0;
		Startpoint[1]=0;
		Endpoint[0]=0;
		Endpoint[1]=0;
	}

}
void myLoop(){
	//maus
	int fdm = open("/dev/input/mice", O_RDONLY | O_SYNC);
	if (fdm == -1) {
		printf("ERROR: /dev/input/mice konnte nicht geoeffnet werden. Sind Sie root?\n");
		exit(EXIT_FAILURE);
	}
	int maus_x = Xres/2, maus_y = Yres/2;
	char paket[3];
	while (1) {
		// Datenpaket einlesen
		read(fdm, paket, 3);
		maus_x += paket[1];
		maus_y += paket[2];
		if (maus_x < 0) maus_x = 0;
		if (maus_y < 0) maus_y = 0;
		if (maus_x >= Xres) maus_x = Xres - 1;
		if (maus_y >= Yres) maus_y = Yres - 1;

		// Status der Maustasten dekodieren und ausgeben
		char left   = paket[0] & 1 ? 'L' : ' ';
		char right  = paket[0] & 2 ? 'R' : ' ';
//		char middle = paket[0] & 4 ? 'M' : ' ';
		writeToFb(0,0,Xres,Yres);
		for(int i=0; i<4; i++){
			for(int j=0; j<4; j++){
				int myy = (Yres-(maus_y-j));
				if(myy < 0) myy = 0;
				if(myy >= Yres) myy = Yres-1;
				int myx = maus_x+i;
				if(myx < 0) myx = 0;
				if(myx >= Xres) myx = Xres-1;
				FBP(myx, myy) = 0xFF00FF;
			}
		}
		if(maus_x >= AppleGui.x && (Yres-maus_y) <= AppleGui.height){
			uint8_t button = 0;
			if(left!=' ') button = 1;
			if(right!=' ') button = button + 2;
			mouseOverApplegui(maus_x, (Yres-maus_y), button);
		}
//		if(paket[0] & 2)break;
	}
}

int main(){
	if(1){ // inits Globalvariables and Memory and the FB
		for(int i=0; i<2; i++){ // Start- und Endpiont
			Startpoint[i] = 0;
			Endpoint[i] = 0;
		}
		Applewidth = 1920/5*4;
		Appleheight = 1080/5*4;
		RMin = -1.0;
		RMax = 2.0;
		IMin = -1.0;
		IMax = 1.0;
		Depth = 100;
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
		Bpp = vinfo.bits_per_pixel;
		LineLenght = finfo.line_length;
		Xres = vinfo.xres;
		Yres = vinfo.yres;
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
	// Zeichnet ein Apfelmaenchen
	makeApfel();
	
	printf("x = %d, y = %d, width = %d, height = %d\n", AppleGui.x, AppleGui.y, AppleGui.width, AppleGui.height);
//	drawGraphic();
	
	myLoop();
	
	//speicher freigeben
	free(MyFB);
	free(MyApple);
	free(IterMatrix);
	free(IterMember);
	munmap(Fbp, BufferLenght);
}

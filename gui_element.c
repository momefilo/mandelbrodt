#include "gui_element.h"
#include "graphics/font16x16.h"
#include <stdio.h>

#define ElemCount 7

int XPos, YPos, ElemWidth = 132, ElemHeight = 66, ElemsInRow = 2;
struct _AppleGui *Apple;
uint8_t *** FBspiegel;
void (*WriteFunc) (const int, ...);
int ElemAreas[ElemCount][4];

void write_font16x16(int x, int y, uint8_t zeichen, uint8_t *fgcolor, uint8_t *bgcolor){
	//create buffer for font
	uint8_t buf[16][16][3];
	int myx = 0, myy = 0;
	for(int i=0; i<32; i=i+1){ // 32 Byte umfasst ein 16x16Bit Zeichen
		for(int k=0; k<8; k=k+1){ // Acht Bits pro Byte
			if(FONT16x16[zeichen*32+i] & (0x80 >> k)){ // ist das Bit gesetzt
				for(int m=0; m<3; m++)FBspiegel[x + myx][y + myy][m] = fgcolor[m];
			}
			else{ // Das Bit ist nicht gesetzt
				for(int m=0; m<3; m++)FBspiegel[x + myx][y + myy][m] = bgcolor[m];
			}
			myx++;
		}
		if((i+1) % 2 == 0){
			myy++;
			myx = 0;
		}
	}
}
void updateWert(int element){
	char text[8];
	int xpos = 0;
	int ypos = 0;
	switch(element){
		case 0: sprintf(text, "% 8d", (*Apple).xres); xpos = 0; ypos = 35; break;
		case 1: sprintf(text, "% 8d", (*Apple).yres);  xpos = ElemWidth; ypos = 35; break;
		case 2: sprintf(text, "%.5f", (*Apple).rmin); xpos = 0; ypos = 101; break;
		case 3: sprintf(text, "%.5f", (*Apple).rmax); xpos = ElemWidth; ypos = 101; break;
		case 4: sprintf(text, "%.5f", (*Apple).imin); xpos = 0; ypos = 167; break;
		case 5: sprintf(text, "%.5f", (*Apple).imax); xpos = ElemWidth; ypos = 167; break;
		case 6: sprintf(text, "% 8d", (*Apple).depth); xpos = 0; ypos = 233; break;
		default: break;
	}
	for(int x=xpos+2; x<xpos+ElemWidth-2; x++)
		for(int y=ypos; y<ypos+16; y++)
			for(int i=0; i<3; i++) FBspiegel[x][y][i] = 0;
	uint8_t fgcolor[] = {200,255,0};
	uint8_t bgcolor[] = {96,96,96};
	for(int i=0; i<8; i++) write_font16x16(xpos+2+i*16, ypos, text[i], fgcolor, bgcolor);
}
void updateWerte(){
	for(int i=0; i<7; i++) updateWert(i);
	int tmp_h = ElemHeight * (ElemCount/ElemsInRow);
	if(ElemCount%ElemsInRow != 0) tmp_h++;
	WriteFunc(XPos, YPos, ElemWidth * ElemsInRow, tmp_h);
}

//TODO
void drawGraphic(){
	FILE *file = fopen("graphics/element.data","rb");
	if( !file){printf("Kann Graphik nicht oeffnen\n");return;}
	for(int i=0; i<ElemCount; i++){
		int inRow = 1;
		fseek(file,0,SEEK_SET);
		for(int x=ElemAreas[i][0]; x<ElemAreas[i][0] + ElemAreas[i][2]; x++){
			for(int y=ElemAreas[i][1]; y<ElemAreas[i][1] + ElemAreas[i][3]; y++){
				uint8_t *color[3];
				for(int c=0; c<3; c++) fread(&FBspiegel[x][y][c], sizeof(uint8_t),1,file);
			}
		}
	}
	fclose(file);
	//TODO Start
	uint8_t fgcolor[] = {255,200,0};
	uint8_t bgcolor[] = {96,96,96};
	char texte[][8]={"  X-Res ","  Y-Res ","  r-Min ","  r-Max ","  i-Min ","  i-Max ","  Depth "};
	int yfak = 0;
	for(int h=0; h<7; h++){
		int xpos = XPos + ElemWidth + 2;
		if(h % 2 == 0) xpos = XPos + 2;
		int ypos = YPos + 2 + yfak * ElemHeight;
		if((h+1) % 2 == 0) yfak++;
		for(int i=0; i<8; i++) write_font16x16(xpos+i*16, ypos, texte[h][i], fgcolor, bgcolor);
	}
	//TODO End
	int tmp_h = ElemHeight * (ElemCount/ElemsInRow);
	if(ElemCount%ElemsInRow != 0) tmp_h++;
	WriteFunc(XPos, YPos, ElemWidth * ElemsInRow, tmp_h);
}

void gui_init(int x, int y, struct _AppleGui *appleGui, uint8_t ***fbBuf, void *fbFunc){
	XPos = x; YPos = y; FBspiegel = fbBuf; WriteFunc = fbFunc;
	Apple = appleGui;
	printf("MODUL: X=%d, Y=%d, rmin=%.4f\n", XPos, YPos, (*Apple).rmin);
	
	if(1){// init ElemAreas
		int yfak = 0;
		for(int i=0; i<ElemCount; i++){
			// xpos
			ElemAreas[i][0] = XPos + ElemWidth;
			if(i % 2 == 0) ElemAreas[i][0] = XPos;
			// ypos
			ElemAreas[i][1] = YPos + yfak * ElemHeight;
			if((i+1) % 2 == 0) yfak++;
			// width/height
			ElemAreas[i][2] = ElemWidth;
			ElemAreas[i][3] = ElemHeight;
			printf(" i=%d, x=%d, y=%d, w=%d, h=%d\n", i,ElemAreas[i][0],ElemAreas[i][1],ElemAreas[i][2],ElemAreas[i][3]);
		}
			
		drawGraphic();
		updateWerte();
	}
}

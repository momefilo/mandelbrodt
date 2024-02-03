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
void write_Text(int x, int y, uint8_t *text, int len, uint8_t *fgcolor, uint8_t *bgcolor){
	for(int i=0; i<len; i++) write_font16x16(x+i*16, y, text[i], fgcolor, bgcolor);
}
void updateWert(int element){
	char text[8];
	switch(element){
		case 0: sprintf(text, "% 8d", (*Apple).xres); break;
		case 1: sprintf(text, "% 8d", (*Apple).yres); break;
		case 2: sprintf(text, "%.5f", (*Apple).rmin); break;
		case 3: sprintf(text, "%.5f", (*Apple).rmax); break;
		case 4: sprintf(text, "%.5f", (*Apple).imin); break;
		case 5: sprintf(text, "%.5f", (*Apple).imax); break;
		case 6: sprintf(text, "% 8d", (*Apple).depth); break;
		default: break;
	}
	
	uint8_t fgcolor[] = {200,255,0};
	uint8_t bgcolor[] = {96,96,96};
	for(int i=0; i<8; i++)
		write_font16x16(ElemAreas[element][0]+2+i*16, ElemAreas[element][1]+35, text[i], fgcolor, bgcolor);
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
	for(int h=0; h<7; h++){
		for(int i=0; i<8; i++) write_font16x16(ElemAreas[h][0]+i*16+2, ElemAreas[h][1]+2, texte[h][i], fgcolor, bgcolor);
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

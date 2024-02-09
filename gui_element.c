#include "gui_element.h"
#include "graphics/font16x16.h"
#include "graphics/font8x8.h"
#include <stdio.h>

int XPos, YPos, MyWidth, MyHeight;
struct _AppleGui *Apple;
uint8_t *** FBspiegel;
void (*WriteFunc) (const int, ...);
void (*CalcFunc) ();
void (*SortFunc) ();
int ElemAreas[ElemCount][4];

// zeichen < 128
void write_font8x8(int x, int y, uint8_t zeichen, uint8_t *fgcolor, uint8_t *bgcolor){
	int myy = 0;
	for(int i=0; i<8; i=i+1){ // 8 Byte umfasst ein 8x8Bit Zeichen
		int myx = 0;
		for(int k=7; k>=0; k=k-1){ // Acht Bits pro Byte
			if(FONT8x8[zeichen][i] & (0x80 >> k)){ // ist das Bit gesetzt
				for(int m=0; m<3; m++)FBspiegel[x + myx][y + myy][m] = fgcolor[m];
			}
			else{ // Das Bit ist nicht gesetzt
				for(int m=0; m<3; m++)FBspiegel[x + myx][y + myy][m] = bgcolor[m];
			}
			myx++;
		}
		myy++;
	}
}
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
void write_Text(int x, int y, uint8_t *text, int len, uint8_t *fgcolor, uint8_t *bgcolor, uint8_t size){
	if(size == 8)
		for(int i=0; i<len; i++) write_font8x8(x+i*8, y, text[i], fgcolor, bgcolor);
	else if(size == 16)
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
	write_Text(ElemAreas[element][0]+2, ElemAreas[element][1]+35, text, 8, fgcolor, bgcolor, 16);
}
void updateWerte(){
	for(int i=0; i<7; i++) updateWert(i);
	int tmp_h = ElemHeight * (ElemCount/ElemsInRow);
	if(ElemCount%ElemsInRow != 0) tmp_h++;
	WriteFunc(XPos, YPos, ElemWidth * ElemsInRow, tmp_h);
}

void gui_onMouseOver(int x, int y, uint8_t button){
	
	//get element-id (0 bis ElemCount-1. Buttons = -2)
	int elem_id = -1;
	if(y > YPos && y < ElemAreas[2][1]){
		if(x < ElemAreas[1][0]) elem_id = 0; else elem_id = 1;}
	else if(y > ElemAreas[2][1] && y < ElemAreas[4][1]){
		if(x < ElemAreas[3][0]) elem_id = 2; else elem_id = 3;}
	else if(y > ElemAreas[4][1] && y < ElemAreas[6][1]){
		if(x < ElemAreas[5][0]) elem_id = 4; else elem_id = 5;}
	else if(y > ElemAreas[6][1] && y < ElemAreas[6][1]+ElemAreas[6][3]){
		if(x < ElemAreas[6][0]+ElemAreas[6][2]) elem_id = 6; else elem_id = -2;}

	//get plus or minus and summand
	int minus = -1;
	int sum = -1;
	if(elem_id > -1){
		if(y > (ElemAreas[elem_id][1] + 18) 
			&& y < (ElemAreas[elem_id][1] + 18 + 14)) minus = 0;
		else if(y > (ElemAreas[elem_id][1] + 51)
			&& y < (ElemAreas[elem_id][1] + 51 + 14)) minus = 1;
		
		if(x > (ElemAreas[elem_id][0] + 2)
			&& x < (ElemAreas[elem_id][0] + 34)) sum = 1;
		else if(x > (ElemAreas[elem_id][0] + 34)
			&& x < (ElemAreas[elem_id][0] + 66)) sum = 100;
		else if(x > (ElemAreas[elem_id][0] + 66)
			&& x < (ElemAreas[elem_id][0] + 98)) sum = 10000;
		else if(x > (ElemAreas[elem_id][0] + 98)
			&& x < (ElemAreas[elem_id][0] + 130)) sum = 100000;
	}
	else if(elem_id < -1){// the Button-Element
		if(y > (ElemAreas[ElemCount-1][1] + 2) 
			&& y < (ElemAreas[ElemCount-1][1] + 33)) minus = 0;
		else if(y > (ElemAreas[ElemCount-1][1] + 33)
			&& y < (ElemAreas[elem_id][1] + 64)) minus = 1;
			
		if(x > (ElemAreas[ElemCount-1][0]+132 + 2)
			&& x < (ElemAreas[ElemCount-1][0]+132 + 45)) sum = 0;
		else if(x > (ElemAreas[ElemCount-1][0]+132 + 45)
			&& x < (ElemAreas[ElemCount-1][0]+132 + 87)) sum = 1;
		else if(x > (ElemAreas[ElemCount-1][0]+132 + 87)
			&& x < (ElemAreas[ElemCount-1][0]+132 + 128)) sum = 2;
	}
	if(button == 1 && elem_id < -1 && minus > -1 && sum > -1){ //klick Buttons-Element
		if(minus == 0){
			switch (sum) {
				case 0: CalcFunc(); break;
				case 1: SortFunc(); break;
			}
		}
	}
	else if(button == 1 && elem_id > -1 && minus > -1 && sum > -1){ // Analogwerte
		if(elem_id > 1 && elem_id <6 && sum > 1) sum = 1;
		if(minus > 0) sum = sum * -1;
		int itmp;
		switch(elem_id){
			case 0: itmp = Apple->xres; itmp += sum; Apple->xres = itmp; break;
			case 1: itmp = Apple->yres; itmp += sum; Apple->yres = itmp; break;
			case 2: itmp = Apple->rmin; itmp += sum; Apple->rmin = itmp; break;
			case 3: itmp = Apple->rmax; itmp += sum; Apple->rmax = itmp; break;
			case 4: itmp = Apple->imin; itmp += sum; Apple->imin = itmp; break;
			case 5: itmp = Apple->imax; itmp += sum; Apple->imax = itmp; break;
			case 6: itmp = Apple->depth; itmp += sum; Apple->depth = itmp; break;
		}
		updateWert(elem_id);
	}
}

void drawGraphic(){
	FILE *file = fopen("graphics/element.data","rb");
	if( !file){printf("Kann Graphik nicht oeffnen\n");return;}
	for(int i=0; i<ElemCount; i++){
		int inRow = 1;
		fseek(file,0,SEEK_SET);
		for(int x=ElemAreas[i][0]; x<ElemAreas[i][0] + ElemAreas[i][2]; x++){
			for(int y=ElemAreas[i][1]; y<ElemAreas[i][1] + ElemAreas[i][3]; y++){
				for(int c=0; c<3; c++) fread(&FBspiegel[x][y][c], sizeof(uint8_t),1,file);
			}
		}
	}
	fclose(file);
	uint8_t fgcolor[] = {255,200,0};
	uint8_t bgcolor[] = {96,96,96};
	char texte[][8]={"  X-Res ","  Y-Res ","  r-Min ","  r-Max ","  i-Min ","  i-Max ","  Depth "};
	for(int h=0; h<7; h++)
		write_Text(ElemAreas[h][0]+2, ElemAreas[h][1]+2, texte[h], 8, fgcolor, bgcolor,16);
		
	FILE *bfile = fopen("graphics/button.data","rb");
	if( !bfile){printf("Kann Buton nicht oeffnen\n");return;}
	int startx = ElemAreas[ElemCount-1][0] + ElemAreas[ElemCount-1][2];
	int starty = ElemAreas[ElemCount-1][1];
	for(int x=startx; x<startx+ ElemWidth; x++){
		for(int y=ElemAreas[ElemCount-1][1]; y<ElemAreas[ElemCount-1][1] + ElemHeight; y++){
			for(int c=0; c<3; c++) fread(&FBspiegel[x][y][c], sizeof(uint8_t),1,bfile);
		}
	}
	WriteFunc(XPos, YPos, MyWidth, MyHeight);
}

void gui_init(int x, int y, struct _AppleGui *appleGui, \
			uint8_t ***fbBuf, void *fbFunc, void *calcFunc, void *sortFunc){
	XPos=x; YPos=y; FBspiegel=fbBuf; WriteFunc=fbFunc; CalcFunc=calcFunc; SortFunc=sortFunc;
	Apple = appleGui;
	MyHeight = ElemHeight * (ElemCount/ElemsInRow);
	if(ElemCount%ElemsInRow != 0) MyHeight = MyHeight + ElemHeight;
	MyWidth = ElemWidth * ElemsInRow;
	
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
		}
			
		drawGraphic();
		updateWerte();
	}
}

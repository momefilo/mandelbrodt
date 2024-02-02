#include "gui_element.h"
#include <stdio.h>

#define ElemCount 7

int XPos, YPos, ElemWidth = 40, ElemHeight = 40, ElemsInRow = 2;
struct _AppleGui *Apple;
uint8_t *** FBspiegel;
void (*WriteFunc) (const int, ...);
int ElemAreas[ElemCount][4];

void drawGraphic(){
	FILE *file = fopen("graphics/regler.data","rb");
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
	int tmp_h = ElemHeight * (ElemCount/ElemsInRow);
	if(ElemCount%ElemsInRow != 0) tmp_h++;
	WriteFunc(XPos, YPos, ElemWidth*ElemsInRow, tmp_h);
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
	}
}

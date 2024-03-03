#include "_Display.h"

_Display::_Display(){
	struct fb_var_screeninfo Vinfo;
	struct fb_fix_screeninfo Finfo;
	fbfile = open("/dev/fb0", O_RDWR);
	if (fbfile) {
		//read device-info to Vinfo ND Finfo
		if (ioctl(fbfile, FBIOGET_FSCREENINFO, &Finfo)){
			printf("Error reading fixed information.\n");}
		if (ioctl(fbfile, FBIOGET_VSCREENINFO, &Vinfo)){
			printf("Error reading variable information.\n");}
			
		xres = Vinfo.xres;
		yres = Vinfo.yres;
		bpp = Vinfo.bits_per_pixel;
		linelenght = Finfo.line_length;
		bufferlenght = Finfo.smem_len;
		// map framebuffer to user memory 
		fbpointer = (uint8_t*)mmap(0, 
				bufferlenght, 
				PROT_READ | PROT_WRITE, 
				MAP_SHARED, 
				fbfile, 0);
		
		//alloc Fbspiegel
		if( !(fbspiegel = (uint8_t*)malloc(bufferlenght * sizeof(uint8_t)))){
			printf("Failed to alloc Fbspiegel.\n");}
	}
}

_Display::~_Display(){
	munmap(fbpointer, bufferlenght);
	close(fbfile);
}

void _Display::putDisplay(int x, int y, int color){
		if(bpp == 16){
		uint16_t farbe = ((((0x00FF0000&color)>>16)/8)<<11) \
						+ ((((0x0000FF00&color)>>8)/4)<<5) \
						+ ((0x000000FF&color)/8);
		*((uint16_t*)(fbpointer + x*2 + y*linelenght)) = farbe;
	}
	else if(bpp == 24){
		*((uint16_t*)(fbpointer + x*3 + y*linelenght)) = 0x00FFFFFF&color;
	}
}

void _Display::putSpiegel(int x, int y, int color){
		if(bpp == 16){
		uint16_t farbe = ((((0x00FF0000&color)>>16)/8)<<11) \
						+ ((((0x0000FF00&color)>>8)/4)<<5) \
						+ ((0x000000FF&color)/8);
		*((uint16_t*)(fbspiegel + x*2 + y*linelenght)) = farbe;
	}
	else if(bpp == 24){
		*((uint16_t*)(fbspiegel + x*3 + y*linelenght)) = 0x00FFFFFF&color;
	}
}

void _Display::drawSpiegel(int offset, int lenght){
	memcpy(fbpointer+offset, fbspiegel, lenght);
	msync(fbpointer+offset, lenght, MS_SYNC);
}

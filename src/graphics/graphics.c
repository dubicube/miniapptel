/*|==========================================================================|*/
/*|  ____        _    _  _              _                                    |*/
/*| |    \  _ _ | |_ |_|| |_  ___  ___ | |_                                  |*/
/*| |  |  || | || . || ||  _|| -_||  _||   |                                 |*/
/*| |____/ |___||___||_||_|  |___||___||_|_|                                 |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| File name: graphivs.c                                                    |*/
/*|                                                                          |*/
/*| Description:                                                             |*/
/*|                                                                          |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| 24/12/2021 | Creation                                                    |*/
/*|            |                                                             |*/
/*|==========================================================================|*/

#include "graphics.h"

#include "xparameters.h"
#include "xil_io.h"


#define SCREEN_BASE_ADDR XPAR_BRAM_0_BASEADDR
#define SCREENCHAR_BASE_ADDR XPAR_CHAR_DRAWER_IP_0_BASEADDR


void drawPixel(int x, int y, int value) {
    uint32_t v = Xil_In32(SCREEN_BASE_ADDR | (((x<<4)|(y>>5))<<2));
    v = v&(~(1<<(y&0x1F)));
    v = v | ((value&1)<<(y&0x1F));
    Xil_Out32(SCREEN_BASE_ADDR | (((x<<4)|(y>>5))<<2), v);
}

void drawFullRect(int x0, int y0, int x1, int y1, int value) {
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            drawPixel(x, y, value);
        }
    }
}

void drawRect(int x0, int y0, int x1, int y1, int value) {
    for (int y = y0; y < y1; y++) {
        drawPixel(x0, y, value);
        drawPixel(x1-1, y, value);
    }
    for (int x = x0; x < x1; x++) {
        drawPixel(x, y0, value);
        drawPixel(x, y1-1, value);
    }
}

void drawChar(uint8_t c) {
    Xil_Out32(SCREENCHAR_BASE_ADDR, c);
}

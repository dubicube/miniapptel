/*|==========================================================================|*/
/*|  ____        _    _  _              _                                    |*/
/*| |    \  _ _ | |_ |_|| |_  ___  ___ | |_                                  |*/
/*| |  |  || | || . || ||  _|| -_||  _||   |                                 |*/
/*| |____/ |___||___||_||_|  |___||___||_|_|                                 |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| File name: graphics.c                                                    |*/
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

uint32_t* getActiveBuffer() {
    return (uint32_t*)SCREEN_BASE_ADDR;
}

void drawPixel(uint32_t* buffer, int x, int y, int value) {
    uint32_t v = Xil_In32((UINTPTR)buffer | (((x<<4)|(y>>5))<<2));
    v = v&(~(1<<(y&0x1F)));
    v = v | ((value&1)<<(y&0x1F));
    Xil_Out32((UINTPTR)buffer | (((x<<4)|(y>>5))<<2), v);
}

void drawFullRect(uint32_t* buffer, int x0, int y0, int x1, int y1, int value) {
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            drawPixel(buffer, x, y, value);
        }
    }
}

void drawRect(uint32_t* buffer, int x0, int y0, int x1, int y1, int value) {
    for (int y = y0; y < y1; y++) {
        drawPixel(buffer, x0, y, value);
        drawPixel(buffer, x1-1, y, value);
    }
    for (int x = x0; x < x1; x++) {
        drawPixel(buffer, x, y0, value);
        drawPixel(buffer, x, y1-1, value);
    }
}

void drawChar(uint8_t c) {
    Xil_Out32(SCREENCHAR_BASE_ADDR, c);
}

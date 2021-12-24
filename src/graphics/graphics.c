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

#include <string.h>
#include <stdlib.h>

#include "xparameters.h"
#include "xil_io.h"


#define SCREEN_BASE_ADDR XPAR_BRAM_0_BASEADDR
#define SCREENCHAR_BASE_ADDR XPAR_CHAR_DRAWER_IP_0_BASEADDR

uint32_t* getActiveGBuffer() {
    return (uint32_t*)SCREEN_BASE_ADDR;
}

uint32_t* createGBuffer() {
    return (uint32_t*)calloc(GBUFFER_SIZE);
}
void destroyGBuffer(uint32_t* gbuffer) {
    free(gbuffer);
}
void refreshActiveBuffer(uint32_t* buffer) {
    // We could use the DMA engine here
    // Maybe an improvement to do in the future
    memcpy((void*)SCREEN_BASE_ADDR, (void*)buffer, GBUFFER_SIZE);
}

void drawPixel(uint32_t* gbuffer, int x, int y, int value) {
    uint32_t v = Xil_In32((UINTPTR)gbuffer | (((x<<4)|(y>>5))<<2));
    v = v&(~(1<<(y&0x1F)));
    v = v | ((value&1)<<(y&0x1F));
    Xil_Out32((UINTPTR)gbuffer | (((x<<4)|(y>>5))<<2), v);
}

void drawFullRect(uint32_t* gbuffer, int x0, int y0, int x1, int y1, int value) {
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            drawPixel(gbuffer, x, y, value);
        }
    }
}

void drawRect(uint32_t* gbuffer, int x0, int y0, int x1, int y1, int value) {
    for (int y = y0; y < y1; y++) {
        drawPixel(gbuffer, x0, y, value);
        drawPixel(gbuffer, x1-1, y, value);
    }
    for (int x = x0; x < x1; x++) {
        drawPixel(gbuffer, x, y0, value);
        drawPixel(gbuffer, x, y1-1, value);
    }
}

void drawChar(uint8_t c) {
    Xil_Out32(SCREENCHAR_BASE_ADDR, c);
}

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

#include "font.h"

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
    return (uint32_t*)calloc(GBUFFER_SIZE, 1);
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
    if (value == 0) {
        v = v&(~(1<<(y&0x1F)));
    } else {
        v = v | (1<<(y&0x1F));
    }
    Xil_Out32((UINTPTR)gbuffer | (((x<<4)|(y>>5))<<2), v);
}

void drawVLine(uint32_t* gbuffer, int x, int y0, int y1, int value) {
    uint32_t v;
    if (y0/32 == y1/32) {
        v = Xil_In32((UINTPTR)gbuffer | (((x<<4)|(y0>>5))<<2));
        if (value == 0) {
            v = v & (((1<<(y0&0x1F))-1) | ~((1<<(y1&0x1F))-1));
        } else {
            v = v | ~(((1<<(y0&0x1F))-1) | ~((1<<(y1&0x1F))-1));
        }
        Xil_Out32((UINTPTR)gbuffer | (((x<<4)|(y0>>5))<<2), v);
    } else {
        v = Xil_In32((UINTPTR)gbuffer | (((x<<4)|(y0>>5))<<2));
        if (value == 0) {
            v = v & ((1<<(y0&0x1F))-1);
        } else {
            v = v | ~((1<<(y0&0x1F))-1);
        }
        Xil_Out32((UINTPTR)gbuffer | (((x<<4)|(y0>>5))<<2), v);

        memset(
            (UINTPTR)gbuffer | (((x<<4)|((y0>>5)+1))<<2),
            value ? 0xFFFFFFFF : 0x00000000,
            ((y1>>5)-(y0>>5)-1)*sizeof(uint32_t)
        );

        v = Xil_In32((UINTPTR)gbuffer | (((x<<4)|(y1>>5))<<2));
        if (value == 0) {
            v = v & ~((1<<(y1&0x1F))-1);
        } else {
            v = v | ((1<<(y1&0x1F))-1);
        }
        Xil_Out32((UINTPTR)gbuffer | (((x<<4)|(y1>>5))<<2), v);
    }
}

void drawFullRect(uint32_t* gbuffer, int x0, int y0, int x1, int y1, int value) {
    for (int x = x0; x < x1; x++) {
        drawVLine(gbuffer, x, y0, y1, value);
    }
}

void drawRect(uint32_t* gbuffer, int x0, int y0, int x1, int y1, int value) {
    drawVLine(gbuffer, x0, y0, y1, value);
    drawVLine(gbuffer, x1, y0, y1, value);
    for (int x = x0+1; x < x1-1; x++) {
        drawPixel(gbuffer, x, y0, value);
        drawPixel(gbuffer, x, y1-1, value);
    }
}

void drawChar(uint8_t c) {
    Xil_Out32(SCREENCHAR_BASE_ADDR, c);
}

// Light implementation
// void drawChar2B(uint32_t* gbuffer, int x, int y, int size, uint8_t c) {
//     y = y >> 5;
//     for (int i = 0; i < 5; i++) {
//         Xil_Out32((UINTPTR)gbuffer | ((((x+i)<<4)|y)<<2), fontDataTable[c*5 + i]);
//     }
// }

// Full implementation
void drawChar2B(uint32_t* gbuffer, int x, int y, int size, uint8_t c) {
    int slice = 0;
    for (int i = 0; i < 5*size; i++) {
        slice = i/size;
        for (int j = 0; j < 8*size; j++) {
            drawPixel(gbuffer, x+i, y+j, fontDataTable[c*5+slice]&(1<<(j/size)));
        }
    }
}

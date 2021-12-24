/*|==========================================================================|*/
/*|  ____        _    _  _              _                                    |*/
/*| |    \  _ _ | |_ |_|| |_  ___  ___ | |_                                  |*/
/*| |  |  || | || . || ||  _|| -_||  _||   |                                 |*/
/*| |____/ |___||___||_||_|  |___||___||_|_|                                 |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| File name: graphics.h                                                    |*/
/*|                                                                          |*/
/*| Description:                                                             |*/
/*|                                                                          |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| 24/12/2021 | Creation                                                    |*/
/*|            |                                                             |*/
/*|==========================================================================|*/


#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#define SCREEN_WIDTH 768
#define SCREEN_HEIGHT 512

uint32_t* getActiveBuffer();

void drawPixel(uint32_t* buffer, int x, int y, int value);
void drawFullRect(uint32_t* buffer, int x0, int y0, int x1, int y1, int value);
void drawRect(uint32_t* buffer, int x0, int y0, int x1, int y1, int value);
void drawChar(uint8_t c);

#endif

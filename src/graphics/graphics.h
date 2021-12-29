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

#define GBUFFER_SIZE ((SCREEN_WIDTH*SCREEN_HEIGHT/32)*sizeof(uint32_t))

// Get a pointer to the active graphic buffer in PL
// Writing in the active graphic buffer immediately outputs the result
// on the screen.
uint32_t* getActiveGBuffer();
// Allocates a graphic buffer in RAM.
// Drawing in such a graphic buffer does not affect output on the screen.
// Call refreshActiveBuffer() with such a custom gbuffer to apply it
// to the active buffer (to print it to the real screen).
uint32_t* createGBuffer();
// Free a gbuffer, if you don't want memory loss :)
void destroyGBuffer(uint32_t* gbuffer);

// Copy the gbuffer given in parameter to the active graphic buffer in PL,
// thus to the real screen.
void refreshActiveBuffer(uint32_t* buffer);

// Draws 1 pixel at given coordinates in the gbuffer
void drawPixel(uint32_t* gbuffer, int x, int y, int value);

// Draws a vertical line, exploiting gbuffer structure
// to improve drawing speed.
// As gbuffer is organized in vertical lines, a simple memeset() call
// can fill an entire line.
// Care is taken at both ends to not overwrite existing pixels
// on the same 32-bit-word.
// /!\ y1 must be greter than y0 /!\
// y1 is not included in the drawing (thus, line size is y1-y0)
void drawVLine(uint32_t* gbuffer, int x, int y0, int y1, int value);

// Draws a full rectangle
// Assuming x1 > x0 and y1 > y0
// x1 et y1 are not included
void drawFullRect(uint32_t* gbuffer, int x0, int y0, int x1, int y1, int value);

// Draws edges of a rectangle
// Assuming x1 > x0 and y1 > y0
// x1 et y1 are not included
void drawRect(uint32_t* gbuffer, int x0, int y0, int x1, int y1, int value);

// Draw to active gbuffer in char mode. It kinda works like a terminal.
// Some ANSI escape sequences are supported (to move the cursor for example).
// Not easy to use, but highly time-efficient on processor side,
// because char drawing is done in custom prgogrammable logic.
// Chars are drawn a 64x32 grid.
// Each char is drawn with a pixel grid of 12x16, but elementary points are
// actually 4 pixels:
// Chars are graphically defined in a 6x8 pixel grid, but drawn
// in a 12x16 pixel grid on the sceen.
void drawChar(uint8_t c);

// Draw char to gbuffer.
// Drawing is done on processor side, thus it is much slower than drawChar().
// "x", "y" are the top-left (included) pixel coordinates of the char to draw.
// "c" is the char to draw, and "size" is the drawing size.
void drawChar2B(uint32_t* gbuffer, int x, int y, int size, uint8_t c);

#endif

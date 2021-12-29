/*|==========================================================================|*/
/*|  ____        _    _  _              _                                    |*/
/*| |    \  _ _ | |_ |_|| |_  ___  ___ | |_                                  |*/
/*| |  |  || | || . || ||  _|| -_||  _||   |                                 |*/
/*| |____/ |___||___||_||_|  |___||___||_|_|                                 |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| File name: main.c                                                        |*/
/*|                                                                          |*/
/*| Description: This is an example to show how to draw in screen buffer,    |*/
/*| as well as how to get inputs from keyboard.                              |*/
/*|                                                                          |*/
/*|                                                                          |*/
/*|==========================================================================|*/
/*| 24/12/2021 | Creation                                                    |*/
/*|            |                                                             |*/
/*|==========================================================================|*/



#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "xil_io.h"
#include "sleep.h"

#include "xparameters.h"


#include "xscugic.h"

#include "./graphics/graphics.h"


// PL drives core0_nFIQ via VIO, ID = 28
#define PL_INTERRUPT_ID 28LU

/************************** Function Prototypes ******************************/
void fiq(XScuGic *pXScuGic);
int setup_interrupts(u16 DeviceId, XScuGic *pXScuGic);

/************************** Variable Definitions *****************************/
volatile static u32 FiqFlag = FALSE;
volatile static u32 CapturedInterrupt  = 0;
static XScuGic gic;


#define KEYBOARD_BASE_ADDR XPAR_KEYBOARD8X8_IP_0_BASEADDR


u64 getKeyBoard() {
    u64 v = Xil_In32(KEYBOARD_BASE_ADDR);
    v |= ((u64)Xil_In32(KEYBOARD_BASE_ADDR + 4))<<32;
    return v;
}
u32 getChar() {
    return Xil_In32(KEYBOARD_BASE_ADDR + 8);
}

// This example allows to move a rectangle on the screen using the arrow keys
// It also outputs keyboard inputs to the uart

int main() {
    init_platform();

    print("Hello dubicube\n\r");


//    int status;
//    xil_printf("Zynq FIQ interrupt test initializing...\r\n");
//    status = setup_interrupts(XPAR_PS7_SCUGIC_0_DEVICE_ID, &gic);
//    if( status != XST_SUCCESS ) {
//        xil_printf("ERROR while setting up interrupts, aborting\r\n");
//        return status;
//    }
//    xil_printf("Waiting for interrupt(s)...\r\n");

    // uint32_t* activeBuffer = getActiveGBuffer();
    uint32_t* gbuffer = createGBuffer();

    drawChar(27);
    drawChar('[');
    drawChar('7');
    drawChar('C');

    int x = 100;
    int y = 256;
    int width = 20;
    int height = 10;
    int a = 1;
    int b = 1;


    char str[] = "AZRGRZFERHETRHTE";
    uint8_t i;
    i = 0;
    while(str[i]) {
        drawChar2B(gbuffer, 205 + 6*i, 100, 1, str[i]);
        drawChar2B(gbuffer, 205 + 12*i, 100+8, 2, str[i]);
        drawChar2B(gbuffer, 205 + 18*i, 100+8+16, 3, str[i]);
        drawChar2B(gbuffer, 205 + 24*i, 100+8+16+24, 4, str[i]);
        i++;
    }

    // drawVLine(gbuffer, 200, 100, 120, 1);
    // drawVLine(gbuffer, 210, 100, 160, 1);

    while (1) {

        // Rectangle drawing and movement
        drawFullRect(gbuffer, x, y, x+width, y+height, 0);
        x+=a;
        y+=b;
        if (x+width >= SCREEN_WIDTH) {
            x = SCREEN_WIDTH - width;
            a = -a;
        }
        if (x < 0) {
            x = 0;
            a = -a;
        }
        if (y+height >= SCREEN_HEIGHT) {
            y = SCREEN_HEIGHT - height;
            b = -b;
        }
        if (y < 256) {
            y = 256;
            b = -b;
        }
        drawFullRect(gbuffer, x, y, x+width, y+height, 1);
        refreshActiveBuffer(gbuffer);

        // Active sleeping
        usleep(1000);
    }

    destroyGBuffer(gbuffer);


    cleanup_platform();
    return 0;
}


int setup_interrupts(u16 DeviceId, XScuGic *pXScuGic)
{
    int status; u32 reg;
    static XScuGic_Config *gicConfig = NULL;

    // get pointer to GIC config
    gicConfig = XScuGic_LookupConfig(DeviceId);
    if( !gicConfig ) return XST_FAILURE;

    // initialize GIC
    status = XScuGic_CfgInitialize(pXScuGic, gicConfig, gicConfig->CpuBaseAddress);
    if (status != XST_SUCCESS) return XST_FAILURE;

    // Write ICCICR=0x09 ==> XScuGic_CfgInitialize() calls CPUInitialize() which
    // sets ICCICR to 0x07 which tells cpu to signal FIQ interrupt via normal IRQ ==> not what we want
    // so setting bit 3 here (FIQEn, see Zynq 7000 TRM Appendix B mpcore section on ICCICR)
    // to allow FIQ interrupt
    reg = XScuGic_CPUReadReg(pXScuGic, XSCUGIC_CONTROL_OFFSET);
    reg = reg | 0x8; // set bit 3
    xil_printf("Writing 0x%04x to ICCRICR\r\n", reg);
    XScuGic_CPUWriteReg(pXScuGic, XSCUGIC_CONTROL_OFFSET, reg);

    // GIC self-test
    status = XScuGic_SelfTest(pXScuGic);
    if (status != XST_SUCCESS) return XST_FAILURE;

    // register fiq handler
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_FIQ_INT, (Xil_ExceptionHandler)fiq, pXScuGic);

    // enable interrupts
    Xil_ExceptionEnableMask(XIL_EXCEPTION_FIQ);

    // set trigger to rising edge (0b11), priority = 0x0 = highest priority
    XScuGic_SetPriorityTriggerType(pXScuGic, PL_INTERRUPT_ID, 0x0, 0b11);
    XScuGic_Enable(&gic, PL_INTERRUPT_ID);

    return XST_SUCCESS;
}

void fiq(XScuGic *pXScuGic)
{
    u32 IntIDFull, IntID;
    if( !pXScuGic ) return;

    IntIDFull = XScuGic_CPUReadReg(pXScuGic, XSCUGIC_INT_ACK_OFFSET);
    IntID = IntIDFull & XSCUGIC_ACK_INTID_MASK;

    // capture interrupt # and signal main loop that FIQ was handled
    CapturedInterrupt = IntID;
    FiqFlag = TRUE;

    // if valid interrupt, set EOI bit
    if( IntID <= XSCUGIC_MAX_NUM_INTR_INPUTS ) {
        XScuGic_CPUWriteReg(pXScuGic, XSCUGIC_EOI_OFFSET, IntIDFull);
    }
}

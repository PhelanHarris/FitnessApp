#ifndef TOUCH_SCREEN_H_
#define TOUCH_SCREEN_H_

#include "types.h"

#define TOUCH_CONTROL (*(volatile unsigned char *)(0x84000230))
#define TOUCH_STATUS (*(volatile unsigned char *)(0x84000230))
#define TOUCH_TX (*(volatile unsigned char *)(0x84000232))
#define TOUCH_RX (*(volatile unsigned char *)(0x84000232))
#define TOUCH_BAUD  (*(volatile unsigned char *)(0x84000234))


/* a data type to hold a point/coord */



void delay(int milliseconds);
Point GetPress(void);
Point GetRelease(void);
TouchEvent getTouchEvent(void);
void initTouch(void);
void WaitForTouch(void);
void calibrateScreen(void);
int ScreenTouched(void);
TouchEvent getTouchEventTimeout();



#endif /* TOUCH_SCREEN_H_ */

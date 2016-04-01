#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include "graphics.h"
#include "colours.h"
#include "fonts.h"
#include "touch_screen.h"
#include "keyboard.h"
#include "sdcard.h"
#include "constants.h"

int left = 107, right = 922, top = 164, bottom = 849



;

/***************************************************** ************************
 ** Helper functions - put transmitted char
 *****************************************************************************/

unsigned char putchar_Touch (unsigned short c)
{
	//assuming that TOUCH_STATUS's TX bit is updated
	while((TOUCH_STATUS & 0x02 ) != 0X02);

	// HOW TO POLL BIT 1?
	TOUCH_TX =  c;

	return c;
}

/*****************************************************************************
 ** Helper functions - get received data
 *****************************************************************************/

unsigned char getchar_Touch (void)
{
	//assuming that TOUCH_Status's RX bit is updated
	while((TOUCH_STATUS & 0x01)!=0x01);

	// HOW TO POLL BIT 1?
	unsigned char c = TOUCH_RX & 0xFF;

	return c;
}

unsigned char getChar (void)
{
	//assuming that TOUCH_Status's RX bit is updated
	//while((TOUCH_STATUS & 0x01)!=0x01);

	// HOW TO POLL BIT 1?
	unsigned char c = TOUCH_RX & 0xFF;

	return c;
}

/*****************************************************************************
 ** Initialise touch screen controller
 ** Calibrate the screen when first time use
 ** Store and read the calibration configuration from the file for future use
 *****************************************************************************/


void initTouch(void)
{
    // Program 6850 and baud rate generator to communicate with TOUCHSCRN
    // send TOUCHSCRN controller an "enable touch" command
    TOUCH_CONTROL = 0x15;
    TOUCH_BAUD = 0x05; // 9600
    usleep(10000);

    if(ScreenTouched())
    	getchar_Touch();

    // Send commands to touch sreen
    putchar_Touch (0x55); //SYNC
    putchar_Touch (0x01); //SIZE
    putchar_Touch (0x12); //COMMAND

    printf("Init_Touch\n");

    // Check the initialization feedback
    char c;

    c = getchar_Touch();
    assert(c == 0x55);

    c = getchar_Touch();
    assert(c == 2);

    c = getchar_Touch();
    assert(c == 0);

    c = getchar_Touch();
    assert(c == 0x12);

    printf("sync received\n");

    // Send the command
    putchar_Touch(0x55);
    putchar_Touch(0x04);
    putchar_Touch(0x20);
    putchar_Touch(0x00);
    putchar_Touch(0x0C);
    putchar_Touch(0x01);


    c = getchar_Touch();
    printf("Got: %hhx\n",c);
    c = getchar_Touch();
    printf("Got: %hhx\n",c);
    c = getchar_Touch();
    printf("Got: %hhx\n",c);
    c = getchar_Touch();
    printf("Got: %hhx\n",c);
    c = getchar_Touch();
    printf("Got: %hhx\n",c);

    // Check the configuration file from file
    if (fileExists("cal.txt")){
    	int fd = openFile("cal.txt");
    	char buf[10];
    	readLineFromFile(fd, buf);
    	left = atoi(buf);
    	readLineFromFile(fd, buf);
    	right = atoi(buf);
    	readLineFromFile(fd, buf);
    	top = atoi(buf);
    	readLineFromFile(fd, buf);
    	bottom = atoi(buf);
    	closeFile(fd);
    	printf("read calibration data from file. left: %d, right: %d, top: %d, bottom: %d\n", left, right, top, bottom);
    }
    else{
        // Calibrate the screen for the first use
    	calibrateScreen();
    }
}
/*****************************************************************************
 ** test if screen being touched, return true if it is touched
 *****************************************************************************/
int ScreenTouched( void )
{
    // return TRUE if any data received from 6850 connected to TOUCHSCRN
    // or FALSE otherwise
    return ((TOUCH_STATUS & 0x01) == 0x01);
}
/*****************************************************************************
 ** wait for screen to be touched
 *****************************************************************************/
void WaitForTouch()
{
    // Delay loop while waiting
    while(!ScreenTouched());

}

/*****************************************************************************
 * This function waits for a touch screen press event and returns X,Y coord
 *****************************************************************************/
Point GetPress(void)
{
	while(1) {
//    	printf("WaitForTouch_Touched\n");
    	unsigned char c = getchar_Touch();
//    	printf("Got: %hhx\n", c);
    	if ((c & 0x81) == 0x81)
    		break;
	}


	Point p1;

    unsigned short XL, XU, YL, YU;

    // wait for a pen down command then return the X,Y coord of the point



    XL = getchar_Touch() & (unsigned char)0x7F;
    XU = getchar_Touch() & (unsigned char)0x1F;
    YL = getchar_Touch() & (unsigned char)0x7F;
    YU = getchar_Touch() & (unsigned char)0x1F;

    p1.x = (( XU << 8) | (XL << 1) ) >> 3 ; //_5_ _7_ _1_
    p1.y = ((YU << 8) | (YL << 1) ) >> 3 ;

    // calibrated correctly so that it maps to a pixel on screen
    p1.x = 80 + (int)(((float)(640) / (float)(right - left))*(float)(p1.x - left));
    p1.y = 80 + (int)(((float)(320) / (float)(bottom - top))*(float)(p1.y - top));


   // p1.x = p1.x * 800
    printf("GetPress: P1.x %d\t", p1.x);
    printf("GetPress: P1.y  %d\n", p1.y);

    return p1;
}

/*****************************************************************************
 * Get the press but uncalibrated
 *****************************************************************************/
Point GetUncalibratedPress()
{
	while(1) {
    	unsigned char c = getchar_Touch();
    	if ((c & 0x81) == 0x81)
    		break;
	}
	Point p1;
    unsigned short XL, XU, YL, YU;

    XL = getchar_Touch() & (unsigned char)0x7F;
    XU = getchar_Touch() & (unsigned char)0x1F;
    YL = getchar_Touch() & (unsigned char)0x7F;
    YU = getchar_Touch() & (unsigned char)0x1F;

    p1.x = (( XU << 8) | (XL << 1) ) >> 3 ;
    p1.y = ((YU << 8) | (YL << 1) ) >> 3 ;
    return p1;
}
/*****************************************************************************
 * This function waits for a touch screen release event and returns X,Y coord
 *****************************************************************************/
Point GetRelease(void)
{
	while(1) {
//    	printf("WaitForTouch_Touched\n");
    	unsigned short c =   getchar_Touch();
    	if ((c & 0x81) == 0x80)
    		break;
	}


	Point p1;

    unsigned short XL, XU, YL, YU;


    // wait for a pen down command then return the X,Y coord of the point



    XL = getchar_Touch() & (unsigned char)0x7F;
    XU = getchar_Touch() & (unsigned char)0x1F;
    YL = getchar_Touch() & (unsigned char)0x7F;
    YU = getchar_Touch() & (unsigned char)0x1F;


    p1.x = (( XU << 8) | (XL << 1) ) >> 3 ; //_5_ _7_ _1_
    p1.y = ((YU << 8) | (YL << 1) ) >> 3 ;

    // calibrated correctly so that it maps to a pixel on screen
    p1.x = 80 + (int)(((float)(640) / (float)(right - left))*(float)(p1.x - left));
    p1.y = 80 + (int)(((float)(320) / (float)(bottom - top))*(float)(p1.y - top));

   // p1.x = p1.x * 800
    printf("GetRelease: P1.x %d\t", p1.x);
    printf("GetRelease: P1.y  %d\n", p1.y);

    return p1;
}

/*****************************************************************************
 * detect the touch or release actions and store as touchEvent
 *****************************************************************************/

TouchEvent getTouchEvent(){
	TouchEvent event;

	while(1) {
		unsigned short c = getchar_Touch();
    	if ((c & 0x81) == 0x81){
    		event.type = EVENT_PRESS;
    		break;
    	}
    	else if ((c & 0x81) == 0x80) {
    		event.type = EVENT_RELEASE;
    		break;
    	}
	}

	unsigned short XL, XU, YL, YU;

    XL = getchar_Touch() & (unsigned char)0x7F;
    XU = getchar_Touch() & (unsigned char)0x1F;
    YL = getchar_Touch() & (unsigned char)0x7F;
    YU = getchar_Touch() & (unsigned char)0x1F;

    event.x = ((XU << 8) | (XL << 1) ) >> 3;
    event.y = ((YU << 8) | (YL << 1) ) >> 3;

    // calibrated correctly so that it maps to a pixel on screen
    event.x = 80 + (int)(((float)(640) / (float)(right - left))*(float)(event.x - left));
    event.y = 80 + (int)(((float)(320) / (float)(bottom - top))*(float)(event.y - top));

    //printf("(%d, %d)\n", event.x, event.y);
    return event;
}

/*****************************************************************************
 * Get touchEvent that can be one of the three types: press/release/unknown
 *****************************************************************************/
TouchEvent getTouchEventTimeout(){
	TouchEvent event;
	event.type = EVENT_UNKNOWN;

	int start = clock();
	while(clock() < start + TOUCH_TIMEOUT) {
		unsigned short c = getChar();
    	if ((c & 0x81) == 0x81){
    		event.type = EVENT_PRESS;
    		break;
    	}
    	else if ((c & 0x81) == 0x80) {
    		event.type = EVENT_RELEASE;
    		break;
    	}
	}
	if (event.type == EVENT_UNKNOWN)
		return event;

	unsigned short XL, XU, YL, YU;

    XL = getchar_Touch() & (unsigned char)0x7F;
    XU = getchar_Touch() & (unsigned char)0x1F;
    YL = getchar_Touch() & (unsigned char)0x7F;
    YU = getchar_Touch() & (unsigned char)0x1F;

    event.x = ((XU << 8) | (XL << 1) ) >> 3;
    event.y = ((YU << 8) | (YL << 1) ) >> 3;

    // calibrated correctly so that it maps to a pixel on screen
    event.x = 80 + (int)(((float)(640) / (float)(right - left))*(float)(event.x - left));
    event.y = 80 + (int)(((float)(320) / (float)(bottom - top))*(float)(event.y - top));

    printf("(%d, %d)\n", event.x, event.y);
    return event;
}

/*****************************************************************************
 * Calibrate the touch screen
 * Collect 8 coordinate sets from user pressing "X" on the screen
 * Calibrate using the collected and expected coordinates
 *****************************************************************************/

void calibrateScreen(){
	ClearScreen();
	Point touch[8];
	int xLoc[4] = {80, 720, 720, 80};
	int yLoc[4] = {80, 80, 400, 400};

    // Display "X" on the screen for 8 times
	int i;
	for (i = 0; i < 8; i++){
		drawString("Tap the X", 400, 240, WHITE, BLACK, FALSE, 0, CONSOLAS_38PT, CENTER, CENTER);
		WriteALine(xLoc[i%4] - 20, yLoc[i%4] - 20, xLoc[i%4] + 20, yLoc[i%4] + 20, WHITE);
		WriteALine(xLoc[i%4] + 20, yLoc[i%4] - 20, xLoc[i%4] - 20, yLoc[i%4] + 20, WHITE);
		touch[i] = GetUncalibratedPress();
		WriteARectangle(xLoc[i%4]-21, xLoc[i%4]+21, yLoc[i%4]-21, yLoc[i%4]+21, BLACK);
		usleep(500000);
	}

    // Calculate the calibrated cordinates using the real cordinates
	left =  (touch[0].x + touch[3].x + touch[4].x + touch[7].x)/4;
	right = (touch[1].x + touch[2].x + touch[5].x + touch[6].x)/4;
	top = (touch[0].y + touch[1].y + touch[4].y + touch[5].y)/4;
	bottom = (touch[2].y + touch[3].y + touch[6].y + touch[7].y)/4;
	printf("Left: %d\nRight: %d\nTop: %d\nBottom: %d\n", left, right, top, bottom);

    // Store the calibration cofiguration to the file
	int fd;
	if (fileExists("cal.txt")){
		fd = openFile("cal.txt");
		printf("file exists\n");
	}
	else{
		printf("file doesnt exist...\n");
		fd = openNewFile("cal.txt");
	}

	printf("cal.txt's fd: %d\n", fd);

	char buf[10];
	sprintf(buf, "%d", left);
	writeLineToFile(fd, buf);
	sprintf(buf, "%d", right);
	writeLineToFile(fd, buf);
	sprintf(buf, "%d", top);
	writeLineToFile(fd, buf);
	sprintf(buf, "%d", bottom);
	writeLineToFile(fd, buf);
   	closeFile(fd);

}

/*****************************************************************************
 * Delay the program for a certain period of time
 * Parameter: millisconds that the caller intends to delay
 *****************************************************************************/
void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

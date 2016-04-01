#include "graphics.h"
#include "colours.h"
#include "fonts.h"
#include "touch_screen.h"
#include "constants.h"
#include <stdlib.h>
#include <time.h>

/*****************************************************************************
 * Clear screen by drawing black rectangule
 *****************************************************************************/
void ClearScreen(void){
	WriteARectangle(0,800,0,480, BLACK);
}

/*****************************************************************************
 * Display the standard page
 *****************************************************************************/
int displayMenu (int numElements, char* buttonNames[],  int font, int fontColour, int backgroundColour, int tFontColour, int tBackgroundColour, int withBack){

    // Clear Screen
	ClearScreen();


	int x1[numElements];
	int x2[numElements];
	int y1[numElements];
	int y2[numElements];

    // Draw numElents number of rectangular buttons and the align them vertically
	int i;
	for (i = 0; i < numElements; i++){
		y1[i] = 50 + i*150;
		y2[i] = 150 + i*150;
		x1[i] = 100;
		x2[i] = 700;
		drawButton (x1[i], x2[i], y1[i], y2[i], buttonNames[i], CONSOLAS_38PT, fontColour, backgroundColour);
	}

    // Draw go back button
	if (withBack){
		DrawHorizontalArrow(10, 80, 10, 80, backgroundColour);
	}

	int lastTouchTime = INT_MAX - 50;
	int buttonPressed = numElements+1, lastButtonPressed = numElements+1;
	int currentTime = 0;
	TouchEvent event;

    // Get the touchEvent on the screen
	while(1){
		currentTime = clock();

		if (ScreenTouched()){
			event = getTouchEventTimeout();
			if (event.type != EVENT_UNKNOWN){
				for (i = 0; i < numElements; i++){
                    // if the touchEvent occurs any location outside the buttons
					if (event.x >= x1[i] && event.x <= x2[i] && event.y >= y1[i] && event.y <= y2[i]){
						break;
					}
				}
				if(withBack){
					if(event.x <80 && event.y <80){
                        // if the touchEvent occurs on the go back button
						i = numElements;
					}
				}
				buttonPressed = i;
                // If the user releases at a different location rather than the initial touch
				if (buttonPressed != lastButtonPressed){
                    // When touch a regular button, change the button colour
					if (buttonPressed < numElements)
						drawButton(x1[buttonPressed], x2[buttonPressed], y1[buttonPressed], y2[buttonPressed], buttonNames[buttonPressed], CONSOLAS_38PT, tFontColour, tBackgroundColour);
                    // When release a regular button, change the button colour back
					if (lastButtonPressed < numElements)
						drawButton(x1[lastButtonPressed], x2[lastButtonPressed], y1[lastButtonPressed], y2[lastButtonPressed], buttonNames[lastButtonPressed], CONSOLAS_38PT, fontColour, backgroundColour);
					if(withBack){
                        // when touch the go back button, change its colour
						if(buttonPressed == numElements){
							DrawHorizontalArrow(10, 80, 10, 80, tBackgroundColour);
						}
                        // when release the go back button, change its colour back
						if(lastButtonPressed == numElements){
							DrawHorizontalArrow(10, 80, 10, 80, backgroundColour);
						}
					}
                    // update the last button pressed
					lastButtonPressed = buttonPressed;
				}
			}
            // update the last touch time
			lastTouchTime = currentTime;
		}

        // if a touch is released over 50 milli secondns
		else if (currentTime > lastTouchTime + 50){
            // Return the last button pressed
            return buttonPressed;
		}
	}
	return 0;
}

/*****************************************************************************
 * Display buttons with spacing
 *****************************************************************************/
void drawButtonSpace (int x1, int x2, int y1, int y2, char* str, int font, int spacing, int fontColour, int backgroundColour){
	WriteARectangle(x1, x2, y1, y2, backgroundColour);
	DrawUnfilledRectangle(x1, x2, y1, y2, fontColour);
	int x = (x1 + x2)/2;
	int y = (y1 + y2)/2;
	drawString(str, x, y, fontColour, backgroundColour, FALSE, spacing, font, CENTER, CENTER);
}

/*****************************************************************************
 * Draw basic rectangule buttons
 *****************************************************************************/
void drawButton (int x1, int x2, int y1, int y2, char* str, int font, int fontColour, int backgroundColour){
	drawButtonSpace(x1, x2, y1, y2, str, font, 2, fontColour, backgroundColour);
}

/*****************************************************************************
 *  Display Vertical Arrow
 *****************************************************************************/
void DrawVerticalArrow(int x1, int x2, int y1, int y2, int colour){
	int i;
	int half_y;
	int start_y;
	int end_y;

    // Determine the start/end/half of y
	if(y1<y2){ // if y1 is higher and y2 is lower
		half_y = y1+ (y2-y1)/2;
		start_y = y1;
		end_y = y2;
	}
	else{ // if y1 is lower and y2 is higher
		half_y = y2 + (y1-y2)/2;
		start_y = y2;
		end_y = y1;
	}
	int start_triangle_x;
	int start_rectangle_x;
	int end_rectangle_x;

    // Determine the start/end/half of x
    // Draw the arrow body
	if(x1<x2){ // if x1 is left and x2 is right
		start_triangle_x = x1+(x2-x1)/2;
		start_rectangle_x = x1+(x2-x1)/4;
		end_rectangle_x = x1+3*(x2-x1)/4;
		for(i=x1; i<x2; i++){
			WriteALine(start_triangle_x, start_y, i, half_y, colour); // connect x1 and x2
		}

		WriteARectangle(start_rectangle_x, end_rectangle_x, half_y, end_y, colour);
	}
	else{ // if x2 is left and x1 is right
		start_triangle_x = x2+(x1-x2)/2;
		start_rectangle_x = x2+(x1-x2)/4;
		end_rectangle_x = x2+3*(x1-x2)/4;
		for(i=x2; i<x1; i++){
			WriteALine(start_triangle_x, start_y, i, half_y, colour); // connect x1 and x2
		}

		WriteARectangle(start_rectangle_x, end_rectangle_x, half_y, end_y, colour);
	}
}


/*****************************************************************************
 *  Display Horizontal Arrow
 *****************************************************************************/
void DrawHorizontalArrow(int x1, int x2, int y1, int y2, int colour){

	int i;
	int third_x;
	int start_x;
	int end_x;

    // Determine the start/end/half of x
	if(x1<x2){
		third_x = x1+ (x2-x1)/2;
		start_x = x1;
		end_x = x2;
	}
	else{
		third_x = x2+(x1-x2)/2;
		start_x = x2;
		end_x = x1;
	}
	int start_triangle_y;
	int start_rectangle_y;
	int end_rectangle_y;

    // Determine the start/end/half of y
    // Draw the arrow body
	if(y1 < y2){ // if y1 is higher and y2 is lower
		start_triangle_y = y1+ (y2-y1)/2;
		start_rectangle_y = y1+ (y2-y1)/4;
		end_rectangle_y = y1+ 3*(y2-y1)/4;
		for(i = y1; i<y2; i++){
			WriteALine(start_x,start_triangle_y, third_x, i, colour); // Draw a filled rectangle
		}
		WriteARectangle(third_x, end_x, start_rectangle_y, end_rectangle_y, colour);
	}
	else{ // if y2 is higher and y1 is lower
		start_triangle_y = y2+ (y1-y2)/2;
		start_rectangle_y = y2+(y1-y2)/4;
		end_rectangle_y = y2+3*(y1-y2)/4;
		for( i = y2; i<y1; i++){
			WriteALine(start_x, start_triangle_y, third_x, i , colour);// Draw a filled rectangle
		}
		WriteARectangle(third_x, end_x, start_rectangle_y, end_rectangle_y, colour);
	}
}


/*****************************************************************************
 *  Display power sign: circle with a verticle bar on the top
 *****************************************************************************/
void DrawPowerSign(int x, int y, int colour){
	//since the screen has y axis 480 pixels, take up no more then 80 pixels
	DrawAFilledCircle(x,y,25,colour);
	DrawAFilledCircle(x,y,20,BLACK);
	WriteARectangle(x-10,x+10,y-25,y, BLACK);
	WriteARectangle(x-5,x+5,y-30,y-15, colour);
}


/*****************************************************************************
 *  Display settings sign: gear shape
 *****************************************************************************/

void DrawSettingsSign(int x, int y, int colour){
	DrawAFilledCircle(x,y,25,colour); // Draw outer white filled circle
    // draw little black filled circles on the outer circle
	DrawAFilledCircle(x-25,y,5,BLACK);
	DrawAFilledCircle(x+25, y, 5, BLACK);
	DrawAFilledCircle(x, y-25,5, BLACK);
	DrawAFilledCircle(x,y+25,5, BLACK);
	DrawAFilledCircle(x,y-25,5,BLACK);
	DrawAFilledCircle(x+18,y+18,5,BLACK);
	DrawAFilledCircle(x-18,y+18,5,BLACK);
	DrawAFilledCircle(x-18,y-18,5,BLACK);
	DrawAFilledCircle(x+18,y-18,5,BLACK);
	DrawAFilledCircle(x,y,12,BLACK); // Draw inner black filled circle
}


/*****************************************************************************
 *  Display rectangle filled with colour that's passed as parameter
 *****************************************************************************/

void DrawUnfilledRectangle(int x1, int x2, int y1, int y2, int colour){
	WriteAVerticalLine(x1, y1, y2, colour);
	WriteAVerticalLine(x2, y1, y2, colour);
	WriteAHorizontalLine(x1, y1, x2, colour);
	WriteAHorizontalLine(x1, y2, x2, colour);
}

/*****************************************************************************
 *  Display a unfilled triangle with border colour that's passed as parameter
 *****************************************************************************/
void DrawASimpleTriangle(int x1, int x2, int y1, int y2, int colour){
	int i;
	if(x1 < x2){
		for( i = x1; i<x2; i++){
			WriteALine((x1+x2)/2, y1,i, y2, colour);
		}
	}
	else{
		for(i = x2; i<x1; i++){
			WriteALine((x1+x2)/2, y1,i, y2, colour);
		}
	}
}


/*****************************************************************************
 *  Display a warning sign at a given location
 *****************************************************************************/
void DrawWarningSign(int x, int y){
	DrawASimpleTriangle(x-20, x+20, y-20, y+20, RED);
	DrawASimpleTriangle(x-15, x+15, y-13, y+17, WHITE);
	drawString("!", x+1, y+5, BLACK, WHITE, FALSE, 0, CONSOLAS_16PT, CENTER, CENTER);
}


/*******************************************************************************************
* This function writes a single pixel to the x,y coords specified using the specified colour
* Note colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
********************************************************************************************/

/*****************************************************************************
 *  Display a dot with colour that's passed as parameter
 *****************************************************************************/
void WriteAPixel(int x, int y, int Colour) {
	WAIT_FOR_GRAPHICS; // is graphics ready for new command
	GraphicsX1Reg = x; // write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour; // set pixel colour
	GraphicsCommandReg = PutAPixel; // give graphics "write pixel" command
}
/*****************************************************************************
 *  Display horizontal line with border colour that's passed as parameter
 *****************************************************************************/
void WriteAHorizontalLine(int x, int y, int x2, int Colour) {
	WAIT_FOR_GRAPHICS; // is graphics ready for new command

	if(x>x2){
		GraphicsX1Reg = x2; // write coords to x1, y1
		GraphicsX2Reg = x;
	}
	else{
		GraphicsX1Reg = x; // write coords to x1, y1
		GraphicsX2Reg = x2;
	}
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour; // set pixel colour
	GraphicsCommandReg = DrawHLine; // give graphics "draw horizontal line" command
}

/*****************************************************************************
 *  Display vertical line with border colour that's passed as parameter
 *****************************************************************************/
void WriteAVerticalLine(int x, int y, int y2, int Colour) {
	WAIT_FOR_GRAPHICS; // is graphics ready for new command

	GraphicsX1Reg = x; // write coords to x1, y1
	if(y>y2){
		GraphicsY1Reg = y2;
		GraphicsY2Reg = y;
	}
	else{
		GraphicsY1Reg = y;
		GraphicsY2Reg = y2;
	}
	GraphicsColourReg = Colour; // set pixel colour
	GraphicsCommandReg = DrawVLine; // give graphics "draw horizontal line" command
}

/*****************************************************************************
 *  Display a line with border colour that's passed as parameter
 *****************************************************************************/
void WriteALine(int x1, int y1, int x2, int y2, int Colour) {
	WAIT_FOR_GRAPHICS; // is graphics ready for new command
	GraphicsX1Reg = x1; // write coords to x1, y1
	GraphicsX2Reg = x2;
	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour; // set pixel colour
	GraphicsCommandReg = DrawLine; // give graphics "draw horizontal line" command
}

/*****************************************************************************
 *  Display unfilled rectangle with border colour that's passed as parameter
 *****************************************************************************/

void WriteARectangle(int x1, int x2, int y1, int y2, int Colour){
	WAIT_FOR_GRAPHICS;
	if(x1>x2){
		GraphicsX1Reg = x2;
		GraphicsX2Reg = x1;
	}
	else{
		GraphicsX1Reg = x1;
		GraphicsX2Reg = x2;
	}
	if(y1>y2){
		GraphicsY1Reg = y2;
		GraphicsY2Reg = y1;
	}
	else{
		GraphicsY1Reg = y1;
		GraphicsY2Reg = y2;
	}
	GraphicsColourReg = Colour; // set pixel colour
	GraphicsCommandReg = DrawRectangle; // give graphics "draw horizontal line" command
}

/*****************************************************************************
 *  Display unfilled rectangle with border colour that's passed as parameter
 *****************************************************************************/
void DrawACircle(int x, int y, int radius, int Colour){
	WAIT_FOR_GRAPHICS;
	GraphicsX1Reg = x;
	GraphicsX2Reg = radius;
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawCircle;
}

/*****************************************************************************
 *  Display circle filled with colour that's passed as parameter
 *****************************************************************************/

void DrawAFilledCircle(int x, int y, int radius, int Colour){
	WAIT_FOR_GRAPHICS;
	GraphicsX1Reg = x;
	GraphicsX2Reg = radius;
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawFilledCircle;
}

/*********************************************************************************************
* This function read a single pixel from the x,y coords specified and returns its colour
* Note returned colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
*********************************************************************************************/
int ReadAPixel(int x, int y) {
	WAIT_FOR_GRAPHICS; // is graphics ready for new command
	GraphicsX1Reg = x; // write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsCommandReg = GetAPixel; // give graphics a "get pixel" command
	WAIT_FOR_GRAPHICS; // is graphics done reading pixel
	return (int)(GraphicsColourReg) ; // return the palette number (colour)
}

/**********************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
**
************************************************************************************/
void ProgramPalette(int PaletteNumber, int RGB) {
	WAIT_FOR_GRAPHICS;
	GraphicsColourReg = PaletteNumber;
	GraphicsX1Reg = RGB >> 16 ; // program red value in ls.8 bit of X1 reg
	GraphicsY1Reg = RGB ; // program green and blue into ls 16 bit of Y1 reg
	GraphicsCommandReg = ProgramPaletteColour; // issue command
}

#include "graphics.h"
#include "fonts.h"
#include "constants.h"


/*************************************************************************************************
** This function draws a single ASCII character at the coord and colour specified
** it optionally eraseS the background colour pixels to the background colour
** This means you can use this to erase characters
**
** e.g. writing a space character with erase set to true will set all pixels in the
** character to the background colour
**
*************************************************************************************************/
void drawChar(int x, int y, int fontColour, int backgroundColour, int c, int erase, int font){
	switch (font) {
		case FONT_5x7:
			outGraphicsCharFont1(x, y, fontColour, backgroundColour, c, erase);
			break;
		case FONT_10x14:
			outGraphicsCharFont2(x, y, fontColour, backgroundColour, c, erase);
			break;
		case CONSOLAS_16PT:
			outGraphicsCharFont3(x, y, fontColour, backgroundColour, c, erase, 27, 2);
			break;
		case CONSOLAS_24PT:
			outGraphicsCharFont3(x, y, fontColour, backgroundColour, c, erase, 40, 3);
			break;
		case CONSOLAS_38PT:
			outGraphicsCharFont3(x, y, fontColour, backgroundColour, c, erase, 59, 4);
			break;
		default:
			printf("That's not a valid font.\n");
			break;
	}
}

// This function takes a string and prints each individual character,
// based on the inputted x and y location, as well as the vertical and
// horizontal orientation
void drawString(char* str, int x, int y, int fontColour, int backgroundColour, int erase, int spacing, int font, int verticalOrientation, int horizontalOrientation){
	int index = 0;
	int length = 0;
	// figure out the length of the string
	while (str[index] != '\0'){
		length++;
		index++;
	}

	// figure out the starting y location
	if (verticalOrientation == CENTER)
		y = y - fontHeights[font]/2;
	else if (verticalOrientation == BOTTOM)
		y = y - fontHeights[font];

	// figure out the starting x location
	if (horizontalOrientation == CENTER)
		x = x - (fontWidths[font]*length + spacing*(length-1))/2;
	else if (horizontalOrientation == RIGHT)
		x = x - (fontWidths[font]*length + spacing*(length-1));

	// loop through and print the characters
	index = 0;
	while (str[index] != '\0'){
		drawChar(x, y, fontColour, backgroundColour, (int)str[index], erase, font);
		x = x + fontWidths[font] + spacing;
		index++;
	}
}


void outGraphicsCharFont1(int x, int y, int fontColour, int backgroundColour, int c, int erase)
{
// using register variables (as opposed to stack based ones) may make execution faster
// depends on compiler and CPU

	register int row, column, theX = x, theY = y;
	register int pixels;
	register char theColour = fontColour;
	register int bitMask, theC = c;

// if x,y coord off edge of screen don't bother
// XRES and YRES are #defined to be 800 and 480 respectively
    if(((short)(x) > (short)(XRES-1)) || ((short)(y) > (short)(YRES-1)))
        return;


// if printable character subtract hex 20
	if(((short)(theC) >= (short)(' ')) && ((short)(theC) <= (short)('~'))) {
		theC = theC - 0x20;
		for(row = 0; (char)(row) < (char)(7); row ++)	{

// get the bit pattern for row 0 of the character from the software font
			pixels = Font5x7[theC][row];
			bitMask = 16;

			for(column = 0; (char)(column) < (char)(5); column ++)	{

// if a pixel in the character display it
				if((pixels & bitMask))
					WriteAPixel(theX+column, theY+row, theColour);

				else {
					if(erase == TRUE)

// if pixel is part of background (not part of character)
// erase the background to value of variable backgroundColour

						WriteAPixel(theX+column, theY+row, backgroundColour);
				}
				bitMask = bitMask >> 1;
			}
		}
	}
}
#define FONT2_XPIXELS	10				// width of Font2 characters in pixels (no spacing)
#define FONT2_YPIXELS	14				// height of Font2 characters in pixels (no spacing)


/******************************************************************************************************************************
** This function draws a single ASCII character at the coord specified using the colour specified
** OutGraphicsCharFont2(100,100, RED, 'A', TRUE, FALSE, 1, 1);	// display upper case 'A' in RED at coords 100,100, erase background
** no scroll, scale x,y= 1,1
**
******************************************************************************************************************************/
void outGraphicsCharFont2(int x, int y, int fontColour, int backgroundColour, int c, int erase)
{
	register int 	row, column, theX = x, theY = y;
	register int 	pixels;
	register char 	theColour = fontColour;
	register int 	bitMask, theCharacter = c, j, theRow, theColumn;
	register int charWidth, charHeight;


    if(((short)(x) > (short)(XRES-1)) || ((short)(y) > (short)(YRES-1)))  // if start off edge of screen don't bother
        return;

	if(((short)(theCharacter) >= (short)(' ')) && ((short)(theCharacter) <= (short)('~'))) {			// if printable character
		theCharacter -= 0x20;																			// subtract hex 20 to get index of first printable character (the space character)
		theRow = FONT2_YPIXELS;
		theColumn = FONT2_XPIXELS;

		for(row = 0; row < theRow; row ++) {
			pixels = Font10x14[theCharacter][row];		     								// get the pixels for row 0 of the character to be displayed
			bitMask = 512;							   											// set of hex 200 i.e. bit 7-0 = 0010 0000 0000
			for(column = 0; column < theColumn;) {
				if((pixels & bitMask))														// if valid pixel, then write it
					WriteAPixel(theX+column, theY+row, theColour);
				else {																		// if not a valid pixel, do we erase or leave it along (no erase)
					if(erase == TRUE)
						WriteAPixel(theX+column, theY+row, backgroundColour);
					// else leave it alone
				}
					column ++;
				bitMask = bitMask >> 1;
			}
		}
	}
}

void outGraphicsCharFont3 (int x, int y, int fontColour, int backgroundColour, int c, int erase, int height, int numSegs) {

	register int 	row, column, theX = x, theY = y;
	register char 	pixels;
	register char 	theColour = fontColour;
	register int 	bitMask, theCharacter = c, j;
	register int 	charHeight = height;
	register int 	segment;

	if(((short)(x) > (short)(XRES-1)) || ((short)(y) > (short)(YRES-1))) return; // if start off edge of screen don't bother


	// check if valid character, then subtract hex 20 to get index of character in font array
	if (((short)(theCharacter) >= (short)(' ')) && ((short)(theCharacter) <= (short)('~'))) {
		theCharacter -= 0x20;

		// loop through each row
		for (row = 0; (char)row < (char)charHeight; row++) {
			// loop through each segment of 8 pixels in a row
			for (segment = 0; segment < numSegs; segment++){
				// get the correct font data
				if (numSegs == 2)
					pixels = Font16x27[theCharacter*54 + row*numSegs + segment];
				else if (numSegs == 3)
					pixels = Font22x40[theCharacter*120 + row*numSegs + segment];
				else
					pixels = Font38x59[theCharacter*236 + row*numSegs + segment];

				bitMask = 128;

				// loop through each pixel in a segment, and draw it
				for (column = 0; column < 8; column++){
					if (pixels & bitMask){
						WriteAPixel(theX+column+segment*8, theY+row, theColour);
					}
					else if (erase){
						WriteAPixel(theX+column+segment*8, theY+row, backgroundColour);
					}
					bitMask = bitMask >> 1;
				}
			}
		}
	}
}

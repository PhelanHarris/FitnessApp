#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#define KEY_HEIGHT 96
#define KEY_WIDTH 80
#define PADDING 5

// each index represents a 1/2 key width by 1 key height space on the touch screen,
// and the value represents the id of the button at that location.
static int buttons[4][20] = {{	0, 	0, 	1, 	1, 	2, 	2, 	3, 	3, 	4, 	4, 	5,	5, 	6, 	6, 	7, 	7, 	8, 	8, 	9, 	9},		// first row
						  {	-1, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17,	18, 18, -1}, 	// second row
						  {	26, 26, 26, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 27, 27, 27}, 	// third row
						  { 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30}		// fourth row
						 };

static char *keys[] = 	{	"q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
					"a", "s", "d", "f", "g", "h", "j", "k", "l",
					/*shift*/"z", "x", "c", "v", "b", "n", "m"/*del*/};
					/*symbols*/ 	/*space*/ 	  /*enter*/

static char *shiftKeys[] = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
					"A", "S", "D", "F", "G", "H", "J", "K", "L",
					"Z", "X", "C", "V", "B", "N", "M"};

static char *symbols[] = {	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
					"-", "/", ":", ";", "(", ")", "&", "@", "\"",
					".", ",", "#", "*", "?", "!", "\'"};

enum {
	KEYS,
	SYMBOLS
};


void displayKeyboard(char* field, char* retString);



#endif

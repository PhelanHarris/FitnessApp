#include "keyboard.h"
#include "fonts.h"
#include "graphics.h"
#include "colours.h"
#include "touch_screen.h"
#include "constants.h"
#include <string.h>
#include <time.h>
#include <stdio.h>

void drawKeyboard();
void drawKey (int, int);
void blinkCursor(int);

int shifted;
int symbolMode;
int cursorLoc;
int cursorRow;
int cursorOn;


// function to display a fullscreen keyboard and return the string the user typed
void displayKeyboard(char* field, char* retString){


	shifted = FALSE;
	symbolMode = FALSE;


	// draw the keyboard
	ClearScreen();
	drawString(field, PADDING, KEY_HEIGHT/4, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, TOP, LEFT);
	drawString(retString, PADDING + fontWidths[CONSOLAS_16PT]*strlen(field), KEY_HEIGHT/4, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, TOP, LEFT);
	drawKeyboard();
	cursorRow = 0;

	int currentTime;
	cursorLoc = PADDING + fontWidths[CONSOLAS_16PT]*(strlen(field)+strlen(retString));
	int lastScreenTouchTime = INT_MAX-TOUCH_TIMEOUT;
	TouchEvent event;
	int buttonPressed = -1;
	int lastButtonPressed = -1;
	cursorOn = TRUE;
	// loop for touch input
	while (TRUE){
		currentTime = clock();

		// blink the cursor on and off
		if (currentTime % 1000 > 500 && !cursorOn){
			blinkCursor(TRUE);
			cursorOn = TRUE;
		}
		else if (currentTime % 1000 < 500 && cursorOn){
			blinkCursor(FALSE);
			cursorOn = FALSE;
		}

		// if there is touch data
		if (ScreenTouched()){
			event = getTouchEventTimeout();
			// if the touch event didnt timeout
			if (event.type != EVENT_UNKNOWN){
				// figure out which button was pressed
				int row = (event.y / KEY_HEIGHT) - 1;
				int column = (event.x / (KEY_WIDTH/2));
				if (row < 0 || row > 3 || column < 0 || column > 19){
					row = 1;
					column = 0;
				}
				buttonPressed = buttons[row][column];
				lastScreenTouchTime = currentTime;
				// if the user is touching a new button
				if (buttonPressed != lastButtonPressed){
				// draw the new button highlighted, and the old button unhighlighted
					drawKey(buttonPressed, TRUE);
					drawKey(lastButtonPressed, FALSE);
					lastButtonPressed = buttonPressed;
				}
			}
		}
		// if there hasn't been input in the last 50ms, assume touch release
		else if(currentTime > lastScreenTouchTime + TOUCH_TIMEOUT){
			lastScreenTouchTime = INT_MAX-TOUCH_TIMEOUT;
			lastButtonPressed = -1;
			//cursorLoc = PADDING + fontWidths[CONSOLAS_16PT]*(strlen((const char*)field) + strlen((const char*)retString));
			if (buttonPressed < 0){}
			// if a letter has been pressed
			else if (buttonPressed < 26){
				// draw the key unhighlighted
				drawKey(buttonPressed, FALSE);
				if (cursorLoc + fontWidths[CONSOLAS_16PT] <= XRES || cursorRow == 0){
					// figure out which key it is and add the character to the return string
					if (symbolMode){
						strcat(retString, symbols[buttonPressed]);
					}
					else if (shifted){
						strcat(retString, shiftKeys[buttonPressed]);
						shifted = FALSE;
						drawKeyboard();
					}
					else{
						strcat(retString, keys[buttonPressed]);
					}
					// erase the cursor, draw the character, and draw the cursor at the new location
					blinkCursor(FALSE);
					if (cursorLoc + fontWidths[CONSOLAS_16PT] >= XRES && cursorRow == 0){
						cursorRow = 1;
						cursorLoc = PADDING;
					}
					drawChar(cursorLoc, KEY_HEIGHT/4 + cursorRow*KEY_HEIGHT/3, WHITE, BLACK, (int)retString[strlen(retString)-1], FALSE, CONSOLAS_16PT);
					cursorLoc += fontWidths[CONSOLAS_16PT];
					blinkCursor(TRUE);
					printf("String: %s\n", retString);
				}
			} // shift key
			else if (buttonPressed == 26){
				shifted = !shifted;
				drawKeyboard();
			} // delete key
			else if (buttonPressed == 27){
				drawKey(buttonPressed, FALSE);

				if (strlen(retString) > 0){
					blinkCursor(FALSE);
					WriteARectangle(cursorLoc-fontWidths[CONSOLAS_16PT], cursorLoc, KEY_HEIGHT/4 + cursorRow*KEY_HEIGHT/3, KEY_HEIGHT/4 + (cursorRow+1)*KEY_HEIGHT/3, BLACK);
					if (cursorLoc - fontWidths[CONSOLAS_16PT] <= PADDING){
						cursorRow = 0;
						cursorLoc = PADDING + fontWidths[CONSOLAS_16PT]*(strlen(field) + strlen(retString));
					}
					retString[strlen(retString)-1] = '\0';
					cursorLoc -= fontWidths[CONSOLAS_16PT];

					blinkCursor(TRUE);
				}
			} // symbol key
			else if (buttonPressed == 28){
				symbolMode = !symbolMode;
				drawKeyboard();
			} // space bar
			else if (buttonPressed == 29){
				drawKey(buttonPressed, FALSE);
				if (cursorLoc + fontWidths[CONSOLAS_16PT] <= XRES || cursorRow == 0){
					blinkCursor(FALSE);
					if (cursorLoc + fontWidths[CONSOLAS_16PT] >= XRES && cursorRow == 0){
						cursorRow = 1;
						cursorLoc = PADDING;
					}
					strcat(retString, " ");
					cursorLoc += fontWidths[CONSOLAS_16PT];
					blinkCursor(TRUE);
					if (symbolMode){
						symbolMode = FALSE;
						drawKeyboard();
					}
				}
			} // return key
			else if (buttonPressed == 30){
				break;
			}
		}
	}

	printf("returning string: %s\n", retString);
}

// --------------------------------------------------- FUNCTIONS USED IF YOU DON'T WANT TO INTERRUPT AN OUTSIDE LOOP --------------------------------
// initialize the keyboard screen
void initKeyboard(char *retString){
	char field[10] = "Message: ";
	shifted = FALSE;
	symbolMode = FALSE;
	retString[0] = '\0';

	// draw the keyboard
	ClearScreen();
	drawString(field, PADDING, KEY_HEIGHT/4, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, TOP, LEFT);
	drawString(retString, PADDING + fontWidths[CONSOLAS_16PT]*strlen(field), KEY_HEIGHT/4, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, TOP, LEFT);
	drawKeyboard();
	cursorRow = 0;
	cursorLoc = PADDING + fontWidths[CONSOLAS_16PT]*(strlen(field)+strlen(retString));
}

// function called in an outside loop when the screen is touched and the keyboard is on screen
void screenTouchedKeyboard(TouchEvent event, int *buttonPressed, int *lastButtonPressed){
	int row = (event.y / KEY_HEIGHT) - 1;
	int column = (event.x / (KEY_WIDTH/2));
	if (row < 0 || row > 3 || column < 0 || column > 19){
		row = 1;
		column = 0;
	}
	*buttonPressed = buttons[row][column];
	// if the user is touching a new button
	if (*buttonPressed != *lastButtonPressed){
	// draw the new button highlighted, and the old button unhighlighted
		drawKey(*buttonPressed, TRUE);
		drawKey(*lastButtonPressed, FALSE);
		*lastButtonPressed = *buttonPressed;
	}
}

// function called in an outside loop when the screen is released and the keyboard in on screen
int buttonReleasedKeyboard(char *retString, int *buttonPressed, int *lastButtonPressed){
	*lastButtonPressed = -1;
	char field[10] = "Message: ";
	if (*buttonPressed < 0){}
	// if a letter has been pressed
	else if (*buttonPressed < 26){
		// draw the key unhighlighted
		drawKey(*buttonPressed, FALSE);
		if (cursorLoc + fontWidths[CONSOLAS_16PT] <= XRES || cursorRow == 0){
			// figure out which key it is and add the character to the return string
			if (symbolMode){
				strcat(retString, symbols[*buttonPressed]);
			}
			else if (shifted){
				strcat(retString, shiftKeys[*buttonPressed]);
				shifted = FALSE;
				drawKeyboard();
			}
			else{
				strcat(retString, keys[*buttonPressed]);
			}
			// erase the cursor, draw the character, and draw the cursor at the new location
			blinkCursor(FALSE);
			if (cursorLoc + fontWidths[CONSOLAS_16PT] >= XRES && cursorRow == 0){
				cursorRow = 1;
				cursorLoc = PADDING;
			}
			drawChar(cursorLoc, KEY_HEIGHT/4 + cursorRow*KEY_HEIGHT/3, WHITE, BLACK, (int)retString[strlen(retString)-1], FALSE, CONSOLAS_16PT);
			cursorLoc += fontWidths[CONSOLAS_16PT];
			blinkCursor(TRUE);
			printf("String: %s\n", retString);
		}
	} // shift key
	else if (*buttonPressed == 26){
		shifted = !shifted;
		drawKeyboard();
	} // delete key
	else if (*buttonPressed == 27){
		drawKey(*buttonPressed, FALSE);

		if (strlen(retString) > 0){
			blinkCursor(FALSE);
			WriteARectangle(cursorLoc-fontWidths[CONSOLAS_16PT], cursorLoc, KEY_HEIGHT/4 + cursorRow*KEY_HEIGHT/3, KEY_HEIGHT/4 + (cursorRow+1)*KEY_HEIGHT/3, BLACK);
			if (cursorLoc - fontWidths[CONSOLAS_16PT] <= PADDING){
				cursorRow = 0;
				cursorLoc = PADDING + fontWidths[CONSOLAS_16PT]*(strlen(field) + strlen(retString));
			}
			retString[strlen(retString)-1] = '\0';
			cursorLoc -= fontWidths[CONSOLAS_16PT];

			blinkCursor(TRUE);
		}
	} // symbol key
	else if (*buttonPressed == 28){
		symbolMode = !symbolMode;
		drawKeyboard();
	} // space bar
	else if (*buttonPressed == 29){
		drawKey(*buttonPressed, FALSE);
		if (cursorLoc + fontWidths[CONSOLAS_16PT] <= XRES || cursorRow == 0){
			blinkCursor(FALSE);
			if (cursorLoc + fontWidths[CONSOLAS_16PT] >= XRES && cursorRow == 0){
				cursorRow = 1;
				cursorLoc = PADDING;
			}
			strcat(retString, " ");
			cursorLoc += fontWidths[CONSOLAS_16PT];
			blinkCursor(TRUE);
			if (symbolMode){
				symbolMode = FALSE;
				drawKeyboard();
			}
		}
	} // return key
	else if (*buttonPressed == 30){
		return TRUE;
	}
	return FALSE;
}

// function called in an outside loop every loop, to update the blinking cursor
void refreshScreenKeyboard(int currentTime){
	// blink the cursor on and off
	if (currentTime % 1000 > 500 && !cursorOn){
		blinkCursor(TRUE);
		cursorOn = TRUE;
	}
	else if (currentTime % 1000 < 500 && cursorOn){
		blinkCursor(FALSE);
		cursorOn = FALSE;
	}
}


// draw a key given its ID, (highlighted or not)
void drawKey (int keyID, int pressed){

	// regular keys
	if (keyID < 26){
		char* key;
		int textColour;
		int backgroundColour;
		if (pressed){
			textColour = WHITE;
			backgroundColour = BLUE;
		}
		else{
			textColour = BLACK;
			backgroundColour = WHITE;
		}

		if (symbolMode)
			key = symbols[keyID];
		else if (shifted)
			key = shiftKeys[keyID];
		else
			key = keys[keyID];

		// first row
		if (keyID < 10){
			drawButton(keyID*KEY_WIDTH+PADDING, (keyID+1)*KEY_WIDTH-PADDING, KEY_HEIGHT+PADDING, 2*KEY_HEIGHT-PADDING, key, CONSOLAS_38PT, textColour, backgroundColour);
		} // second row
		else if (keyID < 19){
			drawButton(((keyID-10)+0.5)*KEY_WIDTH+PADDING, ((keyID-10)+1.5)*KEY_WIDTH-PADDING, 2*KEY_HEIGHT+PADDING, 3*KEY_HEIGHT-PADDING, key, CONSOLAS_38PT, textColour, backgroundColour);
		} // third row
		else {
			drawButton(((keyID-19)+1.5)*KEY_WIDTH+PADDING, ((keyID-19)+2.5)*KEY_WIDTH-PADDING, 3*KEY_HEIGHT+PADDING, 4*KEY_HEIGHT-PADDING, key, CONSOLAS_38PT, textColour, backgroundColour);
		}
	} // shift key
	else if (keyID == 26){
		if (pressed){
			WriteARectangle(PADDING, 1.5*KEY_WIDTH-PADDING, 3*KEY_HEIGHT+PADDING, 4*KEY_HEIGHT-PADDING, BLUE);
			DrawVerticalArrow(35, 1.5*KEY_WIDTH-35, 3*KEY_HEIGHT+23, 4*KEY_HEIGHT-23, WHITE);
		}
		else if (shifted){
			WriteARectangle(PADDING, 1.5*KEY_WIDTH-PADDING, 3*KEY_HEIGHT+PADDING, 4*KEY_HEIGHT-PADDING, WHITE);
			DrawVerticalArrow(35, 1.5*KEY_WIDTH-35, 3*KEY_HEIGHT+23, 4*KEY_HEIGHT-23, BLACK);
		}
		else{
			WriteARectangle(PADDING, 1.5*KEY_WIDTH-PADDING, 3*KEY_HEIGHT+PADDING, 4*KEY_HEIGHT-PADDING, GRAY);
			DrawVerticalArrow(35, 1.5*KEY_WIDTH-35, 3*KEY_HEIGHT+23, 4*KEY_HEIGHT-23, BLACK);
		}
	} // delete key
	else if (keyID == 27){
		if (pressed)
			drawButton(8.5*KEY_WIDTH+PADDING, 10*KEY_WIDTH-PADDING, 3*KEY_HEIGHT+PADDING, 4*KEY_HEIGHT-PADDING, "del", CONSOLAS_24PT, WHITE, BLUE);
		else
			drawButton(8.5*KEY_WIDTH+PADDING, 10*KEY_WIDTH-PADDING, 3*KEY_HEIGHT+PADDING, 4*KEY_HEIGHT-PADDING, "del", CONSOLAS_24PT, BLACK, GRAY);
	} // symbols button
	else if (keyID == 28){
		if (symbolMode){
			if (pressed)
				drawButton(PADDING, 2.5*KEY_WIDTH-PADDING, 4*KEY_HEIGHT+PADDING, 5*KEY_HEIGHT-PADDING, "ABC", CONSOLAS_24PT, WHITE, BLUE);
			else
				drawButton(PADDING, 2.5*KEY_WIDTH-PADDING, 4*KEY_HEIGHT+PADDING, 5*KEY_HEIGHT-PADDING, "ABC", CONSOLAS_24PT, BLACK, GRAY);
		}
		else {
			if (pressed)
				drawButton(PADDING, 2.5*KEY_WIDTH-PADDING, 4*KEY_HEIGHT+PADDING, 5*KEY_HEIGHT-PADDING, "123", CONSOLAS_24PT, WHITE, BLUE);
			else
				drawButton(PADDING, 2.5*KEY_WIDTH-PADDING, 4*KEY_HEIGHT+PADDING, 5*KEY_HEIGHT-PADDING, "123", CONSOLAS_24PT, BLACK, GRAY);
		}
	} // space bar
	else if (keyID == 29){
		if (pressed)
			drawButton(2.5*KEY_WIDTH+PADDING, 7.5*KEY_WIDTH-PADDING, 4*KEY_HEIGHT+PADDING, 5*KEY_HEIGHT-PADDING, "space", CONSOLAS_38PT, WHITE, BLUE);
		else
			drawButton(2.5*KEY_WIDTH+PADDING, 7.5*KEY_WIDTH-PADDING, 4*KEY_HEIGHT+PADDING, 5*KEY_HEIGHT-PADDING, "space", CONSOLAS_38PT, BLACK, WHITE);
	} // return button
	else if (keyID == 30){
		if (pressed)
			drawButton(7.5*KEY_WIDTH+PADDING, 10*KEY_WIDTH-PADDING, 4*KEY_HEIGHT+PADDING, 5*KEY_HEIGHT-PADDING, "return", CONSOLAS_24PT, WHITE, BLUE);
		else
			drawButton(7.5*KEY_WIDTH+PADDING, 10*KEY_WIDTH-PADDING, 4*KEY_HEIGHT+PADDING, 5*KEY_HEIGHT-PADDING, "return", CONSOLAS_24PT, BLACK, GRAY);
	}
}

// draw the cursor on or off, given the cursorLoc (a global var)
void blinkCursor(int on) {
	if (on)
		WriteAVerticalLine(cursorLoc, KEY_HEIGHT/4 + cursorRow*KEY_HEIGHT/3, KEY_HEIGHT/4 + (cursorRow+1)*KEY_HEIGHT/3, WHITE);
	else
		WriteAVerticalLine(cursorLoc, KEY_HEIGHT/4 + cursorRow*KEY_HEIGHT/3, KEY_HEIGHT/4 + (cursorRow+1)*KEY_HEIGHT/3, BLACK);
}

// draw every key
void drawKeyboard(){

	int i;
	for (i = 0; i < 31; i++){
		drawKey(i, FALSE);
	}
}

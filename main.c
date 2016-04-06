#include "types.h"
#include "constants.h"
#include "graphics.h"
#include "colours.h"
#include "fonts.h"
#include "touch_screen.h"
#include "GPS.h"
#include "sdcard.h"
#include "bluetooth.h"
#include "keyboard.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


int elementState[5] = {UNPRESSED, UNPRESSED, UNPRESSED, CONNECTED, 0};
int lastElementState[5] = {-1, -1, -1, -1, -1};
int flash = FALSE;
int lastFlash = FALSE;
int lastButtonPressed, buttonPressed;
int backButtonState = UNPRESSED, newMessageButtonState = UNPRESSED;
int lastBackButtonState = -1, lastNewMessageButtonState = -1;
char messageLines[100][50];
int numMessageLines = 0;
int firstMessage = 0;
int topMessageLine = 0;
int scrollingUpdated = FALSE;
int numNewMessages = 0;
int inMessenger = FALSE;
char curLocation[50];
int upScrollButtonState = UNPRESSED, downScrollButtonState = UNPRESSED;
int lastUpScrollButtonState = -1, lastDownScrollButtonState = -1;

const int x1[3] = {40, 40, 420};
const int x2[3] = {760, 390, 760};
const int y1[3] = {80, 300, 300};
const int y2[3] = {280, 440, 440};

void screenTouchedMain(TouchEvent event);
void screenTouchedMessenger(TouchEvent event);
void buttonReleasedMain(void);
void buttonReleasedMessenger(void);
void refreshScreenMain(void);
void refreshScreenMessenger(void);

int main() {
	ClearScreen();
	drawString("Initializing SD card...", 20, 40, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
	initSD();
	drawString("OK", 780, 40, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
	drawString("Initializing touch screen...", 20, 80, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
	initTouch();
	drawString("OK", 780, 80, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
	drawString("Initializing GPS...", 20, 120, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
	initGPS();
	drawString("OK", 780, 120, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
	drawString("Establishing bluetooth connection...", 20, 160, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
	initBlue();
	drawString("OK", 780, 160, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
	ClearScreen();

	refreshScreenMain();

	int currentTime = 0;
	lastButtonPressed = -1;
	buttonPressed = -1;
	int lastScreenTouchTime = INT_MAX - TOUCH_TIMEOUT;
	TouchEvent event;
	char retString[100];
	strcpy(messageLines[0], "1Test message");
	numMessageLines = 1;
	char *data = malloc(100);

	while(1){
		currentTime = clock();

		// update for elements that flash
		if (currentTime % 1000 > 500)
			flash = TRUE;
		else
			flash = FALSE;

		// always listen for bluetooth data, regardless of current screen
		if (BlueTestReceiveData()){
			char *curData = data;
			char *end;
			printf("Got bluetooth data.\n");
			if (!getMessage(data)){
				printf("False positive for data.\n");
			}
			else{
				// @ indicates a text message is about to be sent
				if (data[0] == '@'){
					// send acknowledge that we are ready to receive message
					sendMessage(";;;;;");
					// wait to receive message
					while(!BlueTestReceiveData()){}
					// loop until we get a message, sending X's to indicate missing message
					while (!getMessage(data) || strstr(data, "]") == NULL || strstr(data, "^") == NULL){
						sendMessage("XXXXX");
					}
					curData = strrchr(data, ']');
					curData += 1;
					end = strchr(curData, '^');
					end[0] = '\0';

					while(strlen(curData) > 30){
						strcpy(messageLines[numMessageLines], "1");
						strcat(messageLines[numMessageLines], curData);
						messageLines[numMessageLines][31] = '\0';
						curData = curData + 30;
						numMessageLines++;
					}
					strcpy(messageLines[numMessageLines], "1");
					strcat(messageLines[numMessageLines], curData);
					numMessageLines++;
					numNewMessages++;
					// update the messaging screen
					scrollingUpdated = TRUE;
					topMessageLine = numMessageLines - 10;
					lastElementState[2] = -1;
				}
				else{
					sendMessage("XXXXX");
				}
			}
		}

		// if the screen has received input
		if (ScreenTouched()){
			// get the touch event
			event = getTouchEventTimeout();
			// check that it wasn't a false positive for touch input
			if (event.type != EVENT_UNKNOWN){
				lastScreenTouchTime = currentTime;
				// depending on which screen we are on, call appropriate function
				if (!inMessenger)
					screenTouchedMain(event);
				else
					screenTouchedMessenger(event);
			}
		}
		// if it has been at least 50 ms since the last touch event,
		// assume there was a touch release.
		else if (currentTime > lastScreenTouchTime + TOUCH_TIMEOUT){
			lastScreenTouchTime = INT_MAX - TOUCH_TIMEOUT;
			// depending on which screen we are on, call the appropriate function
			if (!inMessenger)
				buttonReleasedMain();
			else
				buttonReleasedMessenger();
		}
		// refresh the screen (depending on which we are on)
		if (!inMessenger)
			refreshScreenMain();
		else
			refreshScreenMessenger();
	}
	return 0;
}

void screenTouchedMain(TouchEvent event){

	int i;
	for (i = 0; i < 3; i++){
		if (event.x >= x1[i] && event.x <= x2[i] && event.y >= y1[i] && event.y <= y2[i]){
			break;
		}
	}
	buttonPressed = i;
	if (i == 3)
		buttonPressed = -1;
	if (buttonPressed != lastButtonPressed){
		if (buttonPressed >= 0)
			elementState[buttonPressed] = PRESSED;
		if (lastButtonPressed >= 0)
			elementState[lastButtonPressed] = UNPRESSED;
		lastButtonPressed = buttonPressed;
	}

}

void screenTouchedMessenger(TouchEvent event){
	// back button
	if (event.x < 80 && event.y < 60){
		buttonPressed = 0;
		backButtonState = PRESSED;
	} // new message button
	else if (event.y > 400){
		buttonPressed = 1;
		newMessageButtonState = PRESSED;
	} // up scroll button
	else if (event.y < 80 && event.x > 720){
		buttonPressed = 2;
		upScrollButtonState = PRESSED;
	} // down scroll button
	else if (event.y > 320 && event.x > 720){
		buttonPressed = 3;
		downScrollButtonState = PRESSED;
	}
	else{
		buttonPressed = -1;
	}
	if (buttonPressed != lastButtonPressed){
		if (lastButtonPressed == 0)
			backButtonState = UNPRESSED;
		else if (lastButtonPressed == 1)
			newMessageButtonState = UNPRESSED;
		else if (lastButtonPressed == 2)
			upScrollButtonState = UNPRESSED;
		else if (lastButtonPressed == 3)
			downScrollButtonState = UNPRESSED;

		lastButtonPressed = buttonPressed;
	}
}

void buttonReleasedMain(){
	switch(buttonPressed) {
		case 0:
			if (elementState[3] == ALARM_SOUNDED || elementState[3] == HELP_COMING){
				//sendMessageWithAck()
				elementState[3] = CONNECTED;
			}
			else {
				get_coor(curLocation);
				sendMessageWithAck(curLocation, "!!!!!", '~');
				elementState[3] = ALARM_SOUNDED;
			}
			elementState[0] = UNPRESSED;
			lastElementState[0] = -1;
			break;
		case 1:
			if (elementState[3])
			elementState[1] = UNPRESSED;
			break;
		case 2:
			inMessenger = TRUE;
			lastBackButtonState = -1;
			lastNewMessageButtonState = -1;
			lastUpScrollButtonState = -1;
			lastDownScrollButtonState = -1;
			scrollingUpdated = TRUE;
			topMessageLine = numMessageLines - 10;
			ClearScreen();
			break;
	}
	if (buttonPressed >= 0 && buttonPressed <= 2)
		elementState[buttonPressed] = UNPRESSED;
}

void buttonReleasedMessenger(){
	char retString[100];
	switch(buttonPressed) {
		case 0:
			inMessenger = FALSE;
			lastElementState[0] = -1;
			lastElementState[1] = -1;
			lastElementState[2] = -1;
			lastElementState[3] = -1;
			ClearScreen();
			backButtonState = UNPRESSED;
			break;
		case 1:
			retString[0] = '\0';
			displayKeyboard("Message: ", retString);
			if (strlen(retString) > 0){
				sendMessageWithAck(retString, "#####", ')');
				char *curRetString = retString;
				while (strlen(curRetString) >= 30){
					strcpy(messageLines[numMessageLines], "0");
					strcat(messageLines[numMessageLines], curRetString);
					messageLines[numMessageLines][31] = '\0';
					printf("message line %d: %s\n", numMessageLines, messageLines[numMessageLines]);
					curRetString += 30;
					numMessageLines++;
				}
				strcpy(messageLines[numMessageLines], "0");
				strcat(messageLines[numMessageLines], curRetString);
				printf("message line %d: %s\n", numMessageLines, messageLines[numMessageLines]);
				numMessageLines++;
				numNewMessages++;
			}
			newMessageButtonState = UNPRESSED;
			lastBackButtonState = -1;
			lastNewMessageButtonState = -1;
			lastUpScrollButtonState = -1;
			lastDownScrollButtonState = -1;
			scrollingUpdated = TRUE;
			topMessageLine = numMessageLines - 10;
			ClearScreen();
			break;
		case 2:
			if (topMessageLine > 0){
				topMessageLine--;
				scrollingUpdated = TRUE;
			}
			upScrollButtonState = UNPRESSED;
			break;
		case 3:
			if (topMessageLine + 10 < numMessageLines){
				topMessageLine++;
				scrollingUpdated = TRUE;
			}
			downScrollButtonState = UNPRESSED;
			break;
	}
}

void refreshScreenMain(){
	// Element 0: Help button
	if (elementState[0] != lastElementState[0]){
		if (elementState[3] == ALARM_SOUNDED || elementState[3] == HELP_COMING){
			if (elementState[0] == UNPRESSED){
				WriteARectangle(x1[0], x2[0], y1[0], y2[0], DARKRED);
				WriteARectangle(x1[0]+10, x2[0]-10, y1[0]+10, y2[0]-10, BLACK);
				drawString("CANCEL", (x1[0]+x2[0])/2, (y1[0]+y2[0])/2, DARKRED, BLACK, FALSE, 0, CONSOLAS_38PT, CENTER, CENTER);
			}
			else {
				WriteARectangle(x1[0], x2[0], y1[0], y2[0], RED);
				WriteARectangle(x1[0]+10, x2[0]-10, y1[0]+10, y2[0]-10, BLACK);
				drawString("CANCEL", (x1[0]+x2[0])/2, (y1[0]+y2[0])/2, RED, BLACK, FALSE, 0, CONSOLAS_38PT, CENTER, CENTER);
			}
		}
		else {
			if (elementState[0] == UNPRESSED){
				WriteARectangle(x1[0], x2[0], y1[0], y2[0], DARKRED);
				WriteARectangle(x1[0]+10, x2[0]-10, y1[0]+10, y2[0]-10, BLACK);
				drawString("HELP", (x1[0]+x2[0])/2, (y1[0]+y2[0])/2, DARKRED, BLACK, FALSE, 0, CONSOLAS_38PT, CENTER, CENTER);
			}
			else {
				WriteARectangle(x1[0], x2[0], y1[0], y2[0], RED);
				WriteARectangle(x1[0]+10, x2[0]-10, y1[0]+10, y2[0]-10, BLACK);
				drawString("HELP", (x1[0]+x2[0])/2, (y1[0]+y2[0])/2, RED, BLACK, FALSE, 0, CONSOLAS_38PT, CENTER, CENTER);
			}
		}
		lastElementState[0] = elementState[0];
	}

	// Element 1: Pair Bluetooth Button
	if (elementState[1] != lastElementState[1]){
		if (elementState[1] == UNPRESSED){
			WriteARectangle(x1[1], x2[1], y1[1], y2[1], LIGHT_GRAY);
			WriteARectangle(x1[1]+10, x2[1]-10, y1[1]+10, y2[1]-10, BLACK);
			drawString("Pair Bluetooth", (x1[1]+x2[1])/2, (y1[1]+y2[1])/2, LIGHT_GRAY, BLACK, FALSE, 0, CONSOLAS_24PT, CENTER, CENTER);
		}
		else{
			WriteARectangle(x1[1], x2[1], y1[1], y2[1], WHITE);
			WriteARectangle(x1[1]+10, x2[1]-10, y1[1]+10, y2[1]-10, BLACK);
			drawString("Pair Bluetooth", (x1[1]+x2[1])/2, (y1[1]+y2[1])/2, WHITE, BLACK, FALSE, 0, CONSOLAS_24PT, CENTER, CENTER);
		}
		lastElementState[1] = elementState[1];
	}

	// Element 2: Send Message Button
	if (elementState[2] != lastElementState[2]){
		if (elementState[2] == UNPRESSED){
			WriteARectangle(x1[2], x2[2], y1[2], y2[2], LIGHT_GRAY);
			WriteARectangle(x1[2]+10, x2[2]-10, y1[2]+10, y2[2]-10, BLACK);
			drawString("Messages", (x1[2]+x2[2])/2, (y1[2]+y2[2])/2, LIGHT_GRAY, BLACK, FALSE, 0, CONSOLAS_24PT, CENTER, CENTER);
		}
		else{
			WriteARectangle(x1[2], x2[2], y1[2], y2[2], WHITE);
			WriteARectangle(x1[2]+10, x2[2]-10, y1[2]+10, y2[2]-10, BLACK);
			drawString("Messages", (x1[2]+x2[2])/2, (y1[2]+y2[2])/2, WHITE, BLACK, FALSE, 0, CONSOLAS_24PT, CENTER, CENTER);
		}
		if (numNewMessages > 0){
			DrawAFilledCircle(734, 325, 15, RED);
			char newMessagesString[10];
			sprintf(newMessagesString, "%d", numNewMessages);
			drawString(newMessagesString, 735, 325, WHITE, RED, FALSE, 0, CONSOLAS_16PT, CENTER, CENTER);
		}
		lastElementState[2] = elementState[2];
	}

	// Element 3: Status string
	if (elementState[3] != lastElementState[3]){
		WriteARectangle(0, 400, 0, 80, BLACK);
		if (elementState[3] == NOT_CONNECTED){
			drawString("Bluetooth device not connected.", 40, 40, RED, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
		}
		else if (elementState[3] == CONNECTED){
			drawString("Bluetooth connected.", 40, 40, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
		}
		else if (elementState[3] == ALARM_SOUNDED){
			DrawWarningSign(40, 40);
			drawString("Alarm sounded!", 70, 40, RED, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
		}
		else if (elementState[3] == HELP_COMING){
			// TODO: read in data on how far away they are
			drawString("Help is on the way!", 40, 40, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
		}
		lastElementState[3] = elementState[3];
	}
	else if (flash != lastFlash && elementState[3] == ALARM_SOUNDED){
		if (flash)
			DrawWarningSign(40, 40);
		else
			WriteARectangle(15, 65, 15, 65, BLACK);
		lastFlash = flash;
	}
}


void refreshScreenMessenger(){
	if (scrollingUpdated){
		WriteARectangle(720, 800, 80, 320, BLACK);
		if (numMessageLines <= 10){
			WriteARectangle(720, 800, 280, 320, SILVER);
		}
		else{
			WriteARectangle(720, 800, 80 + 200*topMessageLine/(numMessageLines-10), 120 + 200*topMessageLine/(numMessageLines-10), SILVER);
		}
		scrollingUpdated = FALSE;
		numNewMessages = 1;
	}
	if (numNewMessages > 0){
		WriteARectangle(80, 720, 0, 400, BLACK);
		int i;
		for (i = 0; i + topMessageLine < numMessageLines && i < 10; i++){
			if (i + topMessageLine < 0)
				i = 0 - topMessageLine;
			if (messageLines[i+topMessageLine][0] == '1'){
				drawString(messageLines[i+topMessageLine]+1, 100, 20 + 40*i, WHITE, WHITE, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
			}
			else if (messageLines[i+topMessageLine][0] == '0'){
				drawString(messageLines[i+topMessageLine]+1, 700, 20 + 40*i, GRAY, WHITE, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
			}
		}
		numNewMessages = 0;
	}
	if (backButtonState != lastBackButtonState){
		if (backButtonState == PRESSED)
			DrawHorizontalArrow(10, 80, 10, 80, TURQUOISE);
		else
			DrawHorizontalArrow(10, 80, 10, 80, BLUE);
		lastBackButtonState = backButtonState;
	}

	if (newMessageButtonState != lastNewMessageButtonState){
		if (newMessageButtonState == PRESSED){
			drawButton (0, 800, 400, 480, "Send new message", CONSOLAS_38PT, BLACK, TURQUOISE);
		}
		else{
			drawButton (0, 800, 400, 480, "Send new message", CONSOLAS_38PT, BLACK, BLUE);
		}
		lastNewMessageButtonState = newMessageButtonState;
	}

	if (upScrollButtonState != lastUpScrollButtonState){
		if (upScrollButtonState == PRESSED){
			WriteARectangle(720, 800, 0, 80, LIGHT_GRAY);
			DrawASimpleTriangle(730, 790, 10, 70, DARK_GRAY);
		}
		else{
			WriteARectangle(720, 800, 0, 80, DARK_GRAY);
			DrawASimpleTriangle(730, 790, 10, 70, LIGHT_GRAY);
		}
		lastUpScrollButtonState = upScrollButtonState;
	}

	if (downScrollButtonState != lastDownScrollButtonState){
		if (downScrollButtonState == PRESSED){
			WriteARectangle(720, 800, 320, 400, LIGHT_GRAY);
			DrawASimpleTriangle(730, 790, 390, 330, DARK_GRAY);
		}
		else{
			WriteARectangle(720, 800, 320, 400, DARK_GRAY);
			DrawASimpleTriangle(730, 790, 390, 330, LIGHT_GRAY);
		}
		lastDownScrollButtonState = downScrollButtonState;
	}
}

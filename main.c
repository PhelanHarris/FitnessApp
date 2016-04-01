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
const int x1[3] = {40, 40, 420};
const int x2[3] = {760, 390, 760};
const int y1[3] = {80, 300, 300};
const int y2[3] = {280, 440, 440};

void refreshScreen(void);

int main() {
	ClearScreen();
	drawString("Initializing SD card...", 20, 40, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
	initSD();
	drawString("OK", 780, 40, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
	drawString("Initializing touch screen...", 20, 80, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
	initTouch();
	drawString("OK", 780, 80, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
	drawString("Initializing GPS...", 20, 120, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
	//initGPS();
	drawString("OK", 780, 120, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
	drawString("Establishing bluetooth connection...", 20, 160, WHITE, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, LEFT);
	//initBlue();
	drawString("OK", 780, 160, GREEN, BLACK, FALSE, 0, CONSOLAS_16PT, CENTER, RIGHT);
	ClearScreen();

	refreshScreen();

	int currentTime = 0, lastButtonPressed = 3, buttonPressed = 3;
	int lastScreenTouchTime = INT_MAX - 50;
	TouchEvent event;
	char retString[100];

	while(1){
		currentTime = clock();

		flash = (currentTime % 1000 > 500);

		if (BlueTestReceiveData()){
			char data[100];
			printf("Got bluetooth data.\n");
			if (!getMessage(data)){
				printf("False positive for data.\n");
			}
			else{
				sendMessage("Message received!");
			}
		}

		if (ScreenTouched()){
			event = getTouchEventTimeout();
			if (event.type != EVENT_UNKNOWN){
				lastScreenTouchTime = currentTime;
				int i;
				for (i = 0; i < 3; i++){
					if (event.x >= x1[i] && event.x <= x2[i] && event.y >= y1[i] && event.y <= y2[i]){
						break;
					}
				}
				buttonPressed = i;
				if (i > 2)
					buttonPressed = -1;
				if (buttonPressed != lastButtonPressed){
					if (buttonPressed >= 0)
						elementState[buttonPressed] = PRESSED;
					if (lastButtonPressed >= 0)
						elementState[lastButtonPressed] = UNPRESSED;
					lastButtonPressed = buttonPressed;
					refreshScreen();
				}
			}
		}
		else if (currentTime > lastScreenTouchTime + 50){
			lastScreenTouchTime = INT_MAX - 50;
			switch(buttonPressed) {
				case 0:
					if (elementState[3] == ALARM_SOUNDED || elementState[3] == HELP_COMING){
						sendMessage("Help request cancelled.");
						elementState[3] = CONNECTED;
					}
					else {
						sendMessage("ALERT! HELP BUTTON PRESSED!");
						elementState[3] = ALARM_SOUNDED;
					}
					break;
				case 1:
					// TODO pair bluetooth device
					break;
				case 2:
					retString[0] = '\0';
					displayKeyboard("Message: ", retString);
					if (strlen(retString) > 0)
						sendMessageWithAck(retString);
					ClearScreen();
					lastElementState[0] = -1;
					lastElementState[1] = -1;
					lastElementState[2] = -1;
					lastElementState[3] = -1;
					break;
 			}
			if (buttonPressed >= 0 && buttonPressed <= 2)
				elementState[buttonPressed] = UNPRESSED;

			refreshScreen();
		}
	}

	return 0;
}

void refreshScreen(){
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
		lastElementState[3] = elementState[1];
	}
	else if (flash != lastFlash && elementState[3] == ALARM_SOUNDED){
		if (flash)
			DrawWarningSign(40, 40);
		else
			WriteARectangle(15, 65, 15, 65, BLACK);
		lastFlash = flash;
	}
}

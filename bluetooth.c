#include "bluetooth.h"
#include "constants.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

// method for reading in a string from the bluetooth dongle
int getMessage(char *retString) {
	int cur;
	int i;
	for (i = 0; i < 1000; i++) {
		cur = getcharBlueTimeout();
		// if the read timed out, return false
		if (cur == -1) {
			return FALSE;
		// if the character is a '\r', we're at the end of a message
		} else if (cur == '\r') {
			// read one more character to get rid of the '\n'
			cur = getcharBlueTimeout();
			break;
		} else {
			retString[i] = cur;
		}
	}
	// null terminate the string
	retString[i] = '\0';
	printf("This is the string received: %s \n", retString);
	return TRUE;
}

// method to send a string over bluetooth
void sendMessage(char *message) {
	printf("Sending string over bluetooth: %s\n", message);
	int i;
	for (i = 0; i < strlen(message); i++) {
			putcharBlue(message[i]);
	}
	putcharBlue('\r');
}

// method to send a string over bluetooth, using an initial indicator and acknowledge
int sendMessageWithAck(char *message, char *indicator, char ack) {
	// send '^' character to indicate we want to send a message
	sendMessage(indicator);

	// wait for acknowledgement from Android device
	char retString[8] = "";
	int initialTime = clock();
	int currentTime = initialTime;
	// loop until we receive the acknoledgement
	while(retString[0] != ack && currentTime < initialTime + BLUETOOTH_TIMEOUT){
		getMessage(retString);
		currentTime = clock();
	}

	// If ack received, send message
	if (retString[0] == ack){
		// pad the message so the first/last characters aren't lost
		char paddedMessage[110] = "]]]]]";
		strcat(paddedMessage, message);
		strcat(paddedMessage, "^^^^^");
		sendMessage(paddedMessage);
		printf("Finished sending process using ACK.\n");
		return TRUE;
	}
	printf("Failed to send message.\n");
	return FALSE;
}

// send a single character over bluetooth
int putcharBlue(int c) {
	while ((Blue_Status & 0x02) != 0x02) {
	}
	Blue_TxData = c;
	return c;
}
// receive a single character over bluetooth
int getcharBlue(void) {
	while ((Blue_Status & 0x01) != 0x01) {
	}
	int now = Blue_RxData;
	return now;

}
// receive a single character over bluetooth, but return -1 after a timeout
// if there isn't one.
int getcharBlueTimeout(void) {
	int startTime = clock();
	while ((Blue_Status & 0x01) != 0x01) {
		if (clock() > startTime + BLUETOOTH_TIMEOUT)
			return -1;
	}
	int now = Blue_RxData;
	return now;

}
// initialize the bluetooth device
void initBlue(void) {
	Blue_Control = 0x15;
	Blue_Baud = 0x01;
	//pairDevice();
}

// function to send/receive a simple message, to know that we are paired
void pairDevice(){
	char retString[8] = "";
	while(TRUE){
		while(!BlueTestReceiveData()){}
		getMessage(retString);
		if (strcmp(retString, "Hello") == 0){
			break;
		}
	}
	sendMessage("*****");
}

// function to test if there is any data to read in
int BlueTestReceiveData(void) {
	if ((Blue_Status & 0x01) != 0x01) {
		return 0;
	} else {
		return 1;
	}
}

// function to loop, waiting for data to be available (until it reaches a timeout)
int waitForBTData(int timeout){
	int startTime = clock();
	while(!BlueTestReceiveData()){
		if (startTime + timeout < clock()){
			return FALSE;
		}
	}
	return TRUE;
}

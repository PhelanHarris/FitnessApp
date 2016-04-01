#include "bluetooth.h"
#include "constants.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

void getData(void) {
	int write_flag = 0;
	int i;
	while (1) {

		if (write_flag == 0) {
			while (BlueTestReceiveData() != 1) {
			}
			char testing[1000];
			char now;
			for (i = 0; i < 1000; i++) {
				now = getcharBlue();
				if (now == '\r') {
					break;
				} else {
					testing[i] = now;
				}
			}
			testing[i] = '\0';
			printf("this is the string received %s \n", testing);
			if (strcmp(testing, "hello") == 0) {
				write_flag = 1;
			}
			write_flag = 1;
		}
		if (write_flag == 1) {
			printf("now writing\n");
			char to_send[] = "message";
			char copy_send[strlen(to_send)];
			for (i = 0; i < strlen(to_send); i++) {
				copy_send[i] = putcharBlue(to_send[i]);
			}
			printf("this is the copy %s \n", to_send);
			printf("finished");
			write_flag = 0;
		}
	}
}

int getMessage(char *retString) {
	int cur;
	int i;
	for (i = 0; i < 1000; i++) {
		cur = getcharBlueTimeout();
		if (cur == -1) {
			return FALSE;
		} else if (cur == '\r') {
			break;
		} else {
			retString[i] = cur;
		}
	}
	retString[i] = '\0';
	printf("This is the string received: %s \n", retString);
	return TRUE;
}

void sendMessage(char *message) {
	printf("Sending string over bluetooth: %s\n", message);
	int i;
	for (i = 0; i < strlen(message); i++) {
			putcharBlue(message[i]);
	}
}


int sendMessageWithAck(char *message) {
	// send '^' character to indicate we want to send a message
	sendMessage("^^^^^");

	// wait for acknowledgement from Android device
	char retString[8] = "";
	int initialTime = clock();
	int currentTime = initialTime;
	while(strcmp(retString, "&ACK") != 0 && currentTime < initialTime + BLUETOOTH_TIMEOUT){
		getMessage(retString);
		currentTime = clock();
	}

	// If ack received, send message
	if (strcmp(retString, "&ACK") == 0){
		sendMessage(message);
		// substring to indicate end of message
		sendMessage("&END");
		printf("Finished sending process using ACK.\n");
		return TRUE;
	}
	printf("Failed to send message.\n");
	return FALSE;
}

int putcharBlue(int c) {
	while ((Blue_Status & 0x02) != 0x02) {
	}
	Blue_TxData = c;
	return c;
}
int getcharBlue(void) {
	while ((Blue_Status & 0x01) != 0x01) {
	}
	int now = Blue_RxData;
	return now;

}
int getcharBlueTimeout(void) {
	int startTime = clock();
	while ((Blue_Status & 0x01) != 0x01) {
		if (clock() > startTime + BLUETOOTH_TIMEOUT)
			return -1;
	}
	int now = Blue_RxData;
	return now;

}
void initBlue(void) {
	Blue_Control = 0x15;
	Blue_Baud = 0x01;
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

int BlueTestReceiveData(void) {
	if ((Blue_Status & 0x01) != 0x01) {
		return 0;
	} else {
		return 1;
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>
#include "sdcard.h"

// function to init sd card
int initSD(void){
	alt_up_sd_card_dev *device_reference = NULL;
	int connected = 0;

	// open the SDCard
	printf("Opening SDCard\n");
	if((device_reference = alt_up_sd_card_open_dev("/dev/Altera_UP_SD_Card_Avalon_Interface_0")) == NULL)
	{
		printf("SDCard Open FAILED\n");
		return 0 ;
	}
	else
		printf("SDCard Open PASSED\n");

	// make sure SDCard opened properly
	if (device_reference != NULL ) {
		// loop until SDCard is connected
		while(1) {
			if ((connected == 0) && (alt_up_sd_card_is_Present())){
				printf("Card connected.\n");
				// check that the SDCard has the proper format
				if (alt_up_sd_card_is_FAT16()) {
					printf("FAT16 file system detected.\n");
					break;
				}
				else {
					printf("Unknown file system.\n");
				}
				connected = 1;
			} else if((connected == 1) && (alt_up_sd_card_is_Present() == false)){
				printf("Card disconnected.\n");
				connected =0;
			}
		}
	}
	else
		printf("Can't open device\n");

	return 0;
}

// open a file and return the file descriptor (or -1 on error)
int openFile(char* filename){
	short int fd;
	// make sure sd card is present
	if(alt_up_sd_card_is_Present() && alt_up_sd_card_is_FAT16()) {
		// open the file and return the file descriptor
		if((fd = alt_up_sd_card_fopen(filename, false)) > -1) {
			return fd;
		}
		// error!
		else{
			printf("%s could not be opened.\n", filename);
			return fd;
		}
	}
	printf("Could not find FAT-16 formatted SD card.\n");
	return -1;
}

// create a new file and open it, returning the file descriptor
int openNewFile(char* filename){
	short int fd;
	// make sure sd card set up properly
	if (alt_up_sd_card_is_Present() && alt_up_sd_card_is_FAT16()){
		// create file and return fd
		if ((fd = alt_up_sd_card_fopen(filename, true)) > -1){
			return fd;
		}
		// error!
		else{
			printf("Could not create new file\n");
			return fd;
		}
	}
	printf("Could not find FAT-16 formatted SD card.\n");
	return -1;
}

// close a file
int closeFile(int fd){
	// make sure SD card is set up properly, then close file
	if (alt_up_sd_card_is_Present() && alt_up_sd_card_is_FAT16()) {
		return alt_up_sd_card_fclose(fd);
	}
	printf("failed to close file\n");
	return -1;
}

// write a string to a file
int writeToFile(int fd, char* string){
	// loop through each character in the string
	int i = 0;
	while (string[i] != '\0'){
		// write the character to the file
		printf("Writing char: %c\n", string[i]);
		if (alt_up_sd_card_write(fd, string[i]) == false){
			printf("error writing to file\n");
			return false;
		}
		i++;
	}
	return true;
}

// write a string to a file and add a '\n' character at the end
int writeLineToFile(int fd, char* string){
	if (writeToFile(fd, string) == false)
		return false;
	return alt_up_sd_card_write(fd, '\n');
}

// read a line from a file
int readLineFromFile(int fd, char* buf){
	// repeatedly loop through reading characters
	int i = 0;
	short int ret = alt_up_sd_card_read(fd);
	while(ret >= 0 && ret <= 255){
		// end line character
		if (ret == 10){
			buf[i] = '\0';
			return 1;
		}
		buf[i] = (char)ret;
		ret = alt_up_sd_card_read(fd);
		i++;
	}
	buf[i] = '\0';
	return ret;
}

// read a single character from a file
short int readCharFromFile(int fd){
	return alt_up_sd_card_read(fd);
}

// check if a file exists
int fileExists(char* filename){
	// try to open the file
	int fd = openFile(filename);
	// if no error, close file and return true
	if (fd >= 0){
		closeFile(fd);
		return 1;
	}
	// if error, return false
	return 0;
}


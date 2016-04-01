#ifndef SDCARD_H_
#define SDCARD_H_

int initSD(void);
int openFile(char* filename);
int openNewFile(char* filename);
int closeFile(int fd);
int writeToFile(int fd, char* string);
int writeLineToFile(int fd, char* string);
int readLineFromFile(int fd, char* buf);
short int readCharFromFile(int fd);
int fileExists(char* filename);

#endif /* SDCARD_H_ */

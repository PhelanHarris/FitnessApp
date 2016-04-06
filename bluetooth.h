#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define Blue_Control (* (volatile unsigned char *)(0x84000220))
#define Blue_Status (*(volatile unsigned char *)(0x84000220))
#define Blue_TxData (*(volatile unsigned char *)(0x84000222))
#define Blue_RxData (*(volatile unsigned char*)(0x84000222))
#define Blue_Baud (*(volatile unsigned char *)(0x84000224))


void initBlue(void);
void pairDevice(void);
int BlueTestReceiveData(void);
int getcharBlue(void);
int putcharBlue(int c);
void sendMessage(char *message);
int sendMessageWithAck(char *message, char *indicator, char ack);
int getMessage(char *retString);


#endif /* BLUETOOTH_H_ */

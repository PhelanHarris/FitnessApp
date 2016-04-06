#ifndef GPS_H_
#define GPS_H_


#define GPS_Control (* (volatile unsigned char *)(0x84000210))
#define GPS_Status (*(volatile unsigned char *)(0x84000210))
#define GPS_TxData (*(volatile unsigned char *)(0x84000212))
#define GPS_RxData (*(volatile unsigned char*)(0x84000212))
#define GPS_Baud (*(volatile unsigned char *)(0x84000214))

int indicator;


char *get_location(void);
void initGPS(void);
void init_gps_interrupt(void);
void start_gps(void);
char *get_total_time(void);
char *get_total_distance(void);
char *get_string_speed(void);
void stop_gps(void);
void get_coor(char *);
char * get_calories(void);

#endif

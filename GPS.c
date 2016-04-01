#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "system.h"
#include "altera_avalon_timer_regs.h"
#include "sys/alt_irq.h"
#include "GPS.h"
#include "graphics.h"

double getMass(void);

double total_time;
double old_time;
double time_dif;
double time_seconds;
double latitude;
double longitude;
double total_distance;
double distance_change;
double old_long;
double old_lat;
double inst_speed;
double total_cal;
double mass_runner;
int count;


//init the gps
void initGPS(void){
    //9600 baud
	GPS_Baud = 0x05;
    //8 bit parity 1 stop bit
	GPS_Control = 0x15;
}
//check to see if any data is being recieved from the gps
//if bit 0 is 1 return 1, else return 0
int GPSTestRecieveData(void){
	if((GPS_Status & 0x01) != 0x01){
		return 0;
	}
	else{
		return 1;
	}
}
//sending data to gps
int putcharGPS(int c){
    //wait for tx bit to be 1
	while((GPS_Status & 0x02) != 0x02){
	}
	GPS_TxData = c;
	return c;
}
//reading data from gps
int getcharGPS(void){
    //wait for rx bit to be 1
	while((GPS_Status & 0x01) != 0x01){
	}
	int now = GPS_RxData;
	return now;
}

//puts together longitude, latitude and appropriate directions to return coordinates string
char *one_string(double longitude, double latitude, char N_S[], char E_W[]){
	char *str = malloc(100);
	char lng[10];
    //copy longitude to string
	sprintf(lng, "%f", longitude);
	strcpy(str, lng);
    //add longitude direction to string
	strcat(str, N_S);
    //add latitude direction to string
	sprintf(lng, "%f", latitude);
	strcat(str, lng);
    //add latitude direction to string
	strcat(str, E_W);
	printf("%s \n", str);
	return str;
}
//convert string with gps latitude to decimal degree latitude.
//returns decimal degree latitude
 float printlat(char array[]){
	//parse out the two most significant dd terms.
	char degree[10];
	int a;
	for(a=0; a<2; a++){
		degree[a]=array[a];
	}
	degree[a] = '\0';
	float degree_float = atof(degree);
	//parse the minutes out.
	char m[100];
	int b=0;
	while(a < strlen(array)){
		m[b]=array[a];
		b++;
		a++;
	}
	m[b] = '\0';
	float m_float = atof(m);
    //create the decimal degree
	float lat = degree_float + (m_float/60.0);
	printf("latitude is %f ", lat);
	return lat;
}
//converts string with gpd longitude to decimal degree longitude
//return decimal degree longitude
float printlong(char array[]){
	char degree[10];
	int a;
    //parse out the three most significant ddd terms.
	for(a=0; a<3; a++){
		degree[a]= array[a];
	}
	degree[a] = '\0';
	float degree_float = atof(degree);
    //parse the minutes out
	char m[100];
	int b=0;
	while(a < strlen(array)){
		m[b]=array[a];
		b++;
		a++;
	}
	m[b] = '\0';
	float m_float = atof(m);
    //crete the decimal degree
	float lng = degree_float+ (m_float/ 60.0);
	printf("longitude is %f ", lng );

	return lng;
}
//converts the gps time string, into a seconds time
//returns the time in seconds
double to_seconds(char time[]){

	char hours_string[10];
	char minutes_string[10];
	char seconds_string[10];
	double hours;
	double minutes;
	double seconds;
	double total_time;
	int i,b;
    //gets the hours
	for(i=0; i<2; i++){
		hours_string[i] = time[i];
	}
	hours_string[i] = '\0';
    //gets the minutes
	for(b=0; b<2; b++){
		minutes_string[b] = time[i];
		i++;
	}
    //gets the seconds
	minutes_string[b] = '\0';
	for(b=0; b<2; b++){
		seconds_string[b] = time[i];
		i++;
	}
	seconds_string[b] = '\0';
	hours = atof(hours_string);
	minutes = atof(minutes_string);
	seconds = atof(seconds_string);
    //calculates time in seconds
	total_time = (3600.00)*hours+(60.00)*minutes+seconds;
	return total_time;
}

//initalize values at the start of the run
//time=distance=calories=speed=0
void start_gps(void){
	old_time = time_seconds;
	total_time = 0;
	time_dif = 0;
	total_distance = 0;
	inst_speed = 0;
	distance_change = 0;
	total_cal = 0;
	old_long = longitude;
	old_lat = latitude;
    //gets the mass of the runner for Cal calculations
	mass_runner = getMass();
	count = 1;
}
void set_total_time(void){
	total_time = 0;
	time_dif = 0;
	old_time = time_seconds+1;
}
//get the total calories burned
void get_calories_burned(void){
    /**
     * Kcal/Min ~= respiratoryExchangeRatio*massKg*VO2/1000
     * respiratoryExchangeRatio = 4.86
     * VO2 = 2.209+3.1633*kph
     *
     * From this we know that total_cal= total_cal+(Kcal/Min)*(dtime)
     * where dtime, is the change in time measured in minutes
     */
	//convert to km/hr
	double kph = inst_speed*3.6;
	double VO2 = 2.209+3.1633*kph;
    //total calories per minute
	double cal_min = 4.86*mass_runner*VO2/1000.0;
	double time_change_min = time_dif/60.0;
    //calculate the total calories burned
	double cal = cal_min*time_change_min;
	total_cal = total_cal+cal;
}
//find the distance between two coordintes, using the haversine formula.
void get_distance_between(void){
    /**
     * distance = 2*(Eradius)arcsin(sqrt(sin^2((lat2-lat1)/2))+cos(lat1)*cos(lat2)*sin^2((long1-long2)/2))
     * Eradius = Earth radius = 6373
     * all angles must be in radians.
     */
    //lat2 and long2
	double new_lat = latitude;
	double new_long = longitude;
	double distance_lat = new_lat - old_lat;
	double distance_long = new_long - old_long;
    //convert to radians
	double rad_lat = distance_lat*3.14159/180.0;
	double rad_long = distance_long*3.14159/180.0;
	double sin_lat = sin(rad_lat/2.0);
	double sin_long = sin(rad_long/2.0);
	double exp_lat = pow(sin_lat,2);
	double exp_long = pow(sin_long,2);
    //ready to calculate the distance between
	double hav_dr = exp_lat+cos(new_lat*3.14159/180.0)*cos(old_lat*3.14159/180.0)*exp_long;;
	double distance = 6373.0*2.0*asin(sqrt(hav_dr));
	distance_change = distance*1000;
	total_distance = total_distance+distance_change;
    //the current values of latitude and longitude become lat1 and long1 to be used for next calculation
	old_lat = new_lat;
	old_long = new_long;
}
//find the instantaneous speed
void get_speed(void){
    //change in distance/change in time
	inst_speed = distance_change/time_dif;
}
//find the change in time and update total time
void get_time_dif(double time){
    //change in time is current time- the previously measured time
	time_dif = time - old_time;
    //add change in time to current time
	total_time = total_time+time_dif;
    //update old_time to current time, for next measurement
	old_time = time;
}
//get string recieved from gps in form of "GPGGA" etc...
//parse string from $ to *
char *getString(void){
	char now;
	int i=0;
	char message[1000];
	printf("trying to start \n");
    //wait for gps to send data
	while((GPSTestRecieveData()) != 1){
	}
    //pass the $ character
	while(1){
		now = getcharGPS();
		if(now == '$'){
			printf("starting string");
			break;
		}
	}
    //start writing values to return string
	while(1){
		now = getcharGPS();
        //this is the end of the string
		if(now == '*'){
			break;
		}
        //add the character to string
		else{
			message[i] = now;
			i++;
		}
	}
	message[i] = '\0';
	char *ptr = message;
	return ptr;
}

//gps function used to update the run status screen
char *get_location(void){

	char *message;
	int i;
	int place = 0;
	char now;
	char header[1000];
	char time[1000];
	char lat[1000];
	char N_S[1000];
	char lng[1000];
	char E_W[1000];
	char *location;

	while(1){
		place = 0;
		i =0;
        // get the string
		message = getString();
		printf("the message is %s \n", message);
		while(1){
			now = message[i];
			if(now == ','){
				break;
			}
			else{
				header[i] = now;
				i++;
			}
		}
		header[i] = '\0';
        //make sure that string starts with GPGGA, if does not try again
		printf("header : %s \n", header);
		if(strcmp(header, "GPGGA") == 0){
            //the string is GPGGA
			i++;
			printf("passed \n");
            //parse the time
			while(1){
				now = message[i];
				if(now == ','){
					time[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					time[place] = now;
					place++;
					i++;
				}
			}
			printf("this is time %s \n", time);
            //parse the latitude
			while(1){
				now = message[i];
				if(now == ','){
					lat[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					lat[place] = now;
					place++;
					i++;
				}
			}
			printf("this is the lat: %s \n", lat);
            //parse the latitude direction
			while(1){
				now = message[i];
				if(now == ','){
					N_S[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					N_S[place] = now;
					place++;
					i++;
				}
			}
			printf("this is the lat direction: %s \n", N_S);
            //parse the longitude
			while(1){
				now = message[i];
				if(now == ','){
					lng[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					lng[place] = now;
					place++;
					i++;
				}
			}
			printf("this is the lng: %s \n", lng);
            //parse the longitude direction
			while(1){
				now = message[i];
				if(now == ','){
					E_W[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					E_W[place] = now;
					place++;
					i++;
				}
			}
			printf("this is the lng direction: %s \n", E_W);
            //convert latitude and longitude to float values
			latitude = printlat(lat);
			longitude = printlong(lng);
            //prepare the coordinates in one strin
			location = one_string(longitude, latitude,N_S, E_W);
            //get time
			time_seconds = to_seconds(time);
			//every time the location is gotten, the time, distance, calories and speed is recalculated
			get_time_dif(time_seconds);
			get_distance_between();
			get_speed();
			get_calories_burned();
			printf("the total time in seconds taken is %f \n", time_seconds);
			printf("the location is %s \n", location);
			printf("the total total time is %f \n", total_time);
			printf("the dif in time is %f \n", time_dif);
			printf("the total distance traveled is %f \n", total_distance);
			printf("the dif in distance is %f \n",distance_change);
			printf("the inst speed is %f \n", inst_speed);
			break;
		}
	}
	printf("finished");
    //increment count --> this is used for graphing results
	count++;
	printf("5 seconds is up \n");
    //reset timer
	IOWR_16DIRECT(TIMER_2_BASE, 0, 0);
	printf("interuppt over \n ");
    //set indicator to 1, showing that the values have been updated thus run screen should be updated
	indicator = 1;
	return location;

}

//get the coordinates
//unlike get_location does not update time, speed etc...
//meant to be used only in the fitness application
char *get_coor(void){
	char *message;
	int i;
	int place = 0;
	char now;
	char header[1000];
	char time[1000];
	char lat[1000];
	char N_S[1000];
	char lng[1000];
	char E_W[1000];
	char *location;

	while(1){
		place = 0;
		i =0;
		message = getString();
		printf("the message is %s \n", message);
        //get header message
		while(1){
			now = message[i];
			if(now == ','){
				break;
			}
			else{
				header[i] = now;
				i++;
			}
		}
		header[i] = '\0';
		printf("header : %s \n", header);
        //make sure that head is GPGGA
		if(strcmp(header, "GPGGA") == 0){
			i++;
			printf("passed \n");
            //parse time
			while(1){
				now = message[i];
				if(now == ','){
					time[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					time[place] = now;
					place++;
					i++;
				}
			}
			printf("this is time %s \n", time);
            //parse latitude
			while(1){
				now = message[i];
				if(now == ','){
					lat[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					lat[place] = now;
					place++;
					i++;
				}
			}
			printf("this is the lat: %s \n", lat);
            //parse latitude direction
			while(1){
				now = message[i];
				if(now == ','){
					N_S[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					N_S[place] = now;
					place++;
					i++;
				}
			}
			printf("this is the lat direction: %s \n", N_S);
            //parse longitude
			while(1){
				now = message[i];
				if(now == ','){
					lng[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					lng[place] = now;
					place++;
					i++;
				}
			}
			printf("this is the lng: %s \n", lng);
            //parse longitude direction
			while(1){
				now = message[i];
				if(now == ','){
					E_W[place] = '\0';
					i++;
					place = 0;
					break;
				}
				else{
					E_W[place] = now;
					place++;
					i++;
				}
			}
			printf("this is the lng direction: %s \n", E_W);
            //convert to float for latitude and longitude
			latitude = printlat(lat);
			longitude = printlong(lng);
            //add the coordinated together in one string
			location = one_string(longitude, latitude,E_W, N_S);
			printf("the location is %s \n", location);
			break;
		}
	}
	printf("finished");
	return location;
}
/*
 * puts total time into a string
 * fitness.c uses this function, to help print time to status screen.
 * returns total time in mm:ss form
 **/
char* get_total_time(void){
    //get the total minutes
	int minutes = total_time/60;
    //whatever is left will be presented in seconds form
	int seconds = ((int)total_time)%60;

	char *message = malloc(10);
    //concate message so mm:ss
	sprintf(message, "%d:%02d", minutes, seconds);
	return message;
}
/*
 * puts total distance into a string
 * fitness.c uses this function, to help print distance to the status screen
 * return distance in x.xxx km form
 */
char* get_total_distance(void){
    //convert distance to km
	double km = total_distance/1000.0;
	char *message = malloc(10);
	sprintf(message, "%.3f", km);
	return message;
}
/*
 * puts instantaneous spped into a string
 * fitness.c uses this function to help print speed to the status screen
 * returns speed in x.xxx km/he
 */
char* get_string_speed(){
    //convert to km/her
	double kph = inst_speed*3.6;
	char *message = malloc(10);
	sprintf(message, "%.3f", inst_speed);
	return message;
}
/*
 * puts total calories into string
 * fitness.c uses this function to prind the calories to status screen
 */
char * get_calories(){
	char *message = malloc(10);
	sprintf(message, "%.3f", total_cal);
	return message;
}
//return the count, which is the total number updates during run in string
void get_count(char *ret_count){
	sprintf(ret_count, "%d", count);
}
//initialize interrupts for gps
void init_gps_interrupt(void){
    //ISR is get_location
    //happens every 5 seconds
	//setting up the interrupt
	alt_irq_register(TIMER_2_IRQ, NULL, (void*)get_location);

	printf(" Hardware-Only Timer\n");
	printf(" Setting timer period to 5 seconds.\n");
	int timer_period = 5 * 50000000;
	IOWR_16DIRECT(TIMER_2_BASE, 8, timer_period & 0xFFFF);
	IOWR_16DIRECT(TIMER_2_BASE, 12, timer_period >> 16);

	printf(" Stopping Timer\n");
	IOWR_16DIRECT(TIMER_2_BASE, 4, (1<<3)|(1<<1)|(1<<0));
	//set timeout bit to 0
	IOWR_16DIRECT(TIMER_2_BASE,0,0);

	alt_irq_enable(TIMER_2_IRQ);

	printf(" Starting Timer\n");
	IOWR_16DIRECT(TIMER_2_BASE, 4, (1<<2)|(1<<1)|(1<<0));
	printf(" Waiting for timer to expire...\n");
}
//stop the gps interrupts
void stop_gps(void){
	alt_irq_disable(TIMER_2_IRQ);
}
//get the mass of user from SD card
double getMass(void){
    //if file exists then use user's mass
    //default mass is 50kg.
	int fd;
	char name[20];
	char mass[20];
	char age[4];
	char gender[3];
	double mass_kg;
	if(fileExists("user.txt")){
		fd = openFile("user.txt");
		readLineFromFile(fd, name);
		readLineFromFile(fd, mass);
		readLineFromFile(fd, age);
		readLineFromFile(fd, gender);
		closeFile(fd);
		mass_kg = atof(mass);
	}
	else{
		mass_kg = 50.0;
	}
	return mass_kg;
}

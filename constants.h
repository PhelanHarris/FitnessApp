#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// status string states
typedef enum {
	NOT_CONNECTED,
	CONNECTED,
	ALARM_SOUNDED,
	HELP_COMING
} StringState;

#define UNPRESSED 0
#define PRESSED 1

#define TOUCH_TIMEOUT 50
#define BLUETOOTH_TIMEOUT 500

// Touche events
#define EVENT_PRESS 0
#define EVENT_RELEASE 1
#define EVENT_UNKNOWN 2

#define FALSE 0
#define TRUE 1

// Possible text orientations
typedef enum {
	RIGHT,
	LEFT,
	CENTER,
	TOP,
	BOTTOM
} Orientation;

// Fonts
typedef enum {
	FONT_5x7,
	FONT_10x14,
	CONSOLAS_16PT,
	CONSOLAS_24PT,
	CONSOLAS_38PT
} Font;

static const int fontWidths[] = {5, 10, 16, 22, 32};
static const int fontHeights[] = {7, 14, 27, 40, 59};

// screen bounds
#define XRES 800
#define YRES 480


#endif /* CONSTANTS_H_ */

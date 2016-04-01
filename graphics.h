#ifndef GRAPHICS_H_
#define GRAPHICS_H_

// graphics registers all address begin with '8' so as to by pass data cache on NIOS
#define GraphicsCommandReg (*(volatile unsigned short int *)(0x84000000))
#define GraphicsStatusReg (*(volatile unsigned short int *)(0x84000000))
#define GraphicsX1Reg (*(volatile unsigned short int *)(0x84000002))
#define GraphicsY1Reg (*(volatile unsigned short int *)(0x84000004))
#define GraphicsX2Reg (*(volatile unsigned short int *)(0x84000006))
#define GraphicsY2Reg (*(volatile unsigned short int *)(0x84000008))
#define GraphicsColourReg (*(volatile unsigned short int *)(0x8400000E))
#define GraphicsBackGroundColourReg (*(volatile unsigned short int *)(0x84000010))

/************************************************************************************************
** This macro pauses until the graphics chip status register indicates that it is idle
***********************************************************************************************/
#define WAIT_FOR_GRAPHICS while((GraphicsStatusReg & 0x0001) != 0x0001);

// Command Constants
#define DrawHLine 1
#define DrawVLine 2
#define DrawLine 3
#define DrawRectangle 4
#define DrawCircle 5
#define DrawFilledCircle 6
#define PutAPixel 0xA
#define GetAPixel 0xB
#define ProgramPaletteColour 0x10


// Public functions
void ClearScreen(void);
void DrawLines(void);
void DrawRectangles(void);
void drawButton (int x1, int x2, int y1, int y2, char* str, int font, int fontColour, int backgroundColour);
void drawButtonSpace (int x1, int x2, int y1, int y2, char* str, int font, int spacing, int fontColour, int backgroundColour);
void WriteAPixel(int x, int y, int Colour);
void WriteAHorizontalLine(int x, int y, int length, int Colour);
void WriteAVerticalLine(int x, int y1, int y2, int Colour);
void WriteARectangle(int x1, int x2, int y1, int y2, int Colour);
void WriteALine(int x1, int y1, int x2, int y2, int Colour);
int ReadAPixel(int x, int y);
void ProgramPalette(int PaletteNumber, int RGB);
void DrawAFilledCircle(int x, int y, int radius, int Colour);
void DrawACircle(int x, int y, int radius, int Colour);
void DrawSettingsSign(int x, int y, int colour);
void DrawPowerSign(int x, int y, int colour);
int displayMenu (int numElements, char* buttonNames[],  int font, int fontColour, int backgroundColour, int tFontColour, int tBackgroundColour, int withBack);
void DrawHorizontalArrow(int x1, int x2, int y1, int y2, int colour);
void DrawVerticalArrow(int x1, int x2, int y1, int y2, int colour);
void DrawUnfilledRectangle(int x1, int x2, int y1, int y2, int colour);
void DrawASimpleTriangle(int x1, int x2, int y1, int y2, int colour);
void DrawWarningSign(int x, int y);

#endif /* GRAPHICS_H_ */

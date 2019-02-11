#ifndef __INNOSTEC_PRINTER_API_h__
#define  __INNOSTEC_PRINTER_API_h__

/*! \file printer.h
 * \brief Api for controlling the printer in the Vault Machine.
 * Author: Harold L Marzan, MMC.
 * Requested By: Metaconxept, Dopel.
*/
//!Opens the Printer and returns it Handle
int prnPrintOpen();

//!Opens the Printer and returns it Handle
int prnPrintInit(); //!same as prnPrintOpen

//!Closes the Printer referred by its Handle
int prnPrintClose(int prnHandle);

//!Closes the Printer referred by its Handle
int prnPrintDeInit(int prnHandle); //!same as prnPrintClose

//!Prepares the Printer for printing
int prnPrintStart(int prnHandle);

//!Unprepares the Printer
int prnPrintEnd(int prnHandle);


#define PRN_PRINTER_FONT_SMALLEST 	0
#define PRN_PRINTER_FONT_SMALL 		1
#define PRN_PRINTER_FONT_MEDIUM 	2
#define PRN_PRINTER_FONT_LARGE 		3
//!If the printer support this below
#define PRN_PRINTER_FONT_BIG 		4
//!The default font is small
#define PRN_PRINTER_FONT_DEFAULT	PRN_PRINTER_FONT_SMALL


//!Set the font to use for printing
int prnSetFont(int font);

//!Get the current font used for printing
int prnGetCurrentFont();

//!Sends a text to the printer and print it with the current font
int prnPrintText(const char* bufText);

//!Sends a text to the printer and print it with the current font
int prnPrintLine(const char* bufLine); //!same as prnPrintText

//!Sends a formatted text to the printer with the current font
int prnPrintf(const char* format, ...);

//!Gets the last error code from the printer
int prnGetLastError();

//!Get the last error message from the printer
char* prnGetLastErrorMessage();

//!Sends a monochrome bitmap of the logo to print from a buffer 
int prnPrintLogo(char* bufBitmapLogo);

//!Sends blank lines, or simply step down the sheet the next lines specified in step
int prnSetStep(int step);

//!Printer Status
#define PRN_PRINTER_STATUS_NO_ERROR 	0
#define PRN_PRINTER_STATUS_NO_PAPER 	1
#define PRN_PRINTER_STATUS_PAPER_JAM 	2
#define PRN_PRINTER_STATUS_DEAD			4
#define PRN_PRINTER_STATUS_NO_COMM		5
#define PRN_PRINTER_STATUS_NO_NETWORK	5

//!Retrieves the printer status
int prnGetStatus();

//!Retrieves the printer status from the specified print handle.
int prnGetStatusEx(int prnHandle);


//!Set the font to use for printing from the specified print handle.
int prnSetFontEx(int prnHandle, int font);

//!Get the current font used for printing from the specified print handle.
int prnGetCurrentFontEx(int prnHandle);

//!Sends a text to the printer and print it with the current font from the specified print handle.
int prnPrintTextEx(int prnHandle, const char* bufText);

//!Sends a text to the printer and print it with the current font
int prnPrintLineEx(int prnHandle, const char* bufLine); //!same as prnPrintTextEx 

//!Sends a formatted text to the printer with the current font from the specified print handle.
int prnPrintfEx(int prnHandle, const char* format, ...);

//!Gets the last error code from the printer from the specified print handle.
int prnGetLastErrorEx(int prnHandle);

//!Get the last error message from the printer from the specified print handle.
char* prnGetLastErrorMessageEx(int prnHandle);

//!Sends a monochrome bitmap of the logo to print from a buffer from the specified print handle.
int prnPrintLogoEx(int prnHandle, char* bufBitmapLogo);

//!Sends blank lines, or simply step down the sheet the next lines specified in step from the specified print handle.
int prnSetStepEx(int prnHandle, int step);


#endif //__INNOSTEC_PRINTER_API_h__


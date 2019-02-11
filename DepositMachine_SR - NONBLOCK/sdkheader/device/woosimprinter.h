#ifndef __WOOSIMPRINTER_H__
#define __WOOSIMPRINTER_H__

#ifdef __cplusplus
 extern "C" {
#endif


//typedef int bool;
//#define true 1
//#define false 0

#define TRUE  1
#define FALSE 0
//////////////////////////////////////////////////////////////////////
// ERROR CODE DEFINE
//
#define WSP_REV_SUCCESS          0
#define WSP_REV_ERROR           -1
#define WSP_TIME_OUT            -5

#define WSP_OPEN_SUCCESS        0
#define WSP_NO_MEMORY           -1
#define WSP_NOT_OPENED	    	-2                                         
#define WSP_NO_POLL             -3
#define WSP_NO_DEVICE           -4

#define WSP_NO_DESCRIPTOR       -5
                             
extern int     error_no;

//////////////////////////////////////////////////////////////////////
//const define
// 
#define PROG_ID             	 0x11
#define FONT_ID              	 0x22

enum Barcode
{
	UPCA=65,
	UPCE,
	EAN13,
	EAN8,
	CODE39,
	ITF,
	CODABAR,
	CODE93,
	CODE128
};

enum WIDTH
{
	_1INCH=192,
	_2INCH=384,
	_3INCH=576,
	_4INCH=768
};

enum MCU
{
	M16C,
	ARM,
	RX
};

enum TEXTWIDTH
{
	TEXTWIDTH01 = 0x00,
	TEXTWIDTH02,
	TEXTWIDTH03,
	TEXTWIDTH04,
	TEXTWIDTH05,
	TEXTWIDTH06,
	TEXTWIDTH07,
	TEXTWIDTH08
};

enum TEXTHIGHT
{
	TEXTHIGHT01 = 0x00,
	TEXTHIGHT02,
	TEXTHIGHT03,
	TEXTHIGHT04,
	TEXTHIGHT05,
	TEXTHIGHT06,
	TEXTHIGHT07,
	TEXTHIGHT08
};

typedef struct woosimlib_ woosimlib;
struct woosimlib_
{
	int   poll_ndx;
	int   fd;
	int   type;                                     
	int   tag;
	void  (*on_poll_in )( woosimlib *);
	void  (*on_poll_out)( woosimlib *);
	void  (*on_poll_err)( woosimlib *);
	void  (*on_poll_hup)( woosimlib *);

	char  devname[20];                              
	int   baud;  
	int   databit;
	int   stopbit;
	int   parity;


	void  (*on_read    )( woosimlib *);
	void  (*on_writable)( woosimlib *);
	void  (*on_error   )( woosimlib *, int);
};

extern void       poll_init( void);
extern int        poll_loop( int);

extern woosimlib *woosimlib_open( char *devname, int baud, int databit, int parity, int stopbit);
extern int        woosimlib_write( woosimlib *sock, char *buf, int buf_size);
extern int        woosimlib_read ( woosimlib *sock, char *buf, int buf_size);
extern void       woosimlib_close( woosimlib *sock);

/*****************************************************
* woosim library function
*****************************************************/
extern int ControlCommand(unsigned char* Cmds, int iLen);
extern int PrintSpool(woosimlib *sock, bool bDelete_Spool);
extern int BmpSaveSpool(woosimlib *sock,char *bmpFilePath);
extern void LoadLogoSaveSpool(int n);
extern void TextSaveSpool(char *textData);
extern void CutPaper(int mode);
extern void PrintData();
extern void PrintDotFeed(int dots);
extern void PrintLineFeed(int lines);
extern void InitPrinterStatus();
extern void InitLineSpace();
extern void InitPageMode(int iXPos, int iYPos, int width, int height);
extern void SetPageMode();
extern void Page_Print_StandardMode();
extern void Page_Print();
extern void Page_SetStandardMode();
extern void Page_ClearCurrentData();
extern void Page_SetArea(int iXPos,int iYPos, int width, int height);
extern void Page_SetDirection(int n);
extern void Page_SetPosition(int iXPos, int iYPos);
extern void Page_DrawBox(int iXPos, int iYPos, int width, int height, int thickness);
extern void Page_Newline();
extern void Page_DotFeed(int dots);
extern void Page_LineFeed(int lines);
extern void SetLineSpace(int n);
extern void SetCharSpace(int n);
extern void SetCharCodeTable(int n, int MCU);
extern void SetCharCodeTableEX(int n);
extern void SetFontSize(int n);
extern void SetTextStyle(int underline, bool emphasize, int width, int height, bool reverse);
extern void SetUpsideDown(bool set);
extern void SetTextAlignment(int n);
extern void SetAbsPosition(int distance);
extern void Set_PosDataAlignment(int n);
extern void SetPositionFromMark(int distance);
extern void FeedToMark();
extern void OneDimensionBarcodeSaveSpool(unsigned char ucBarcodeType, int ucBarWidth, int ucBarHeight, bool HRI_On, char* barcodeData);
extern void DataMatrixSaveSpool(int width, int height, int module, char *barcodeData);
extern void QRCodeSaveSpool(int version, char level, int module, char *barcodeData);
extern void MaxicodeSaveSpool(int mode, char *barcodeData);
extern void GS1DatabarSaveSpool(int type, int n, char * barcodeData);
extern void PDF417SaveSpool(int width, int column, int level, int ratio, char *barcodeData, bool HRI);
extern void TruncatedPDF417SaveSpool(int width, int column, int level, int ratio, char *barcodeData, bool HRI);
extern void MicroPDF417SaveSpool(int width, int column, int row, int ratio, char *barcodeData, bool HRI);
extern void ClearSpool();
extern int EnterMSRMode(woosimlib *sock,int n,int iTimeoutMsec);
extern void CancelMSRMode();
extern void Page_DrawLine(int x, int y, int x2, int y2, int thickness);
extern void Page_DrawEllipse(int x, int y, int A_length, int B_length, int thickness);
extern int GetPrinterModelName(woosimlib *sock, int iTimeout_msec);
extern int GetFirmwareVersion(woosimlib *sock, int iTimeout_msec);
extern int GetPrinterStatus(woosimlib *sock, int iTimeout_msec);


#ifdef __cplusplus
}
#endif

#endif

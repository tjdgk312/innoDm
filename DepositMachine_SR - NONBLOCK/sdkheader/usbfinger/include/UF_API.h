#ifndef _SFM_API_H_
#define _SFM_API_H_

#include "SFM_Type.h"
#include "SFM_Error.h"
#include "SFM_Module.h"
#include "SFM_Enroll.h"
#include "SFM_GPIO.h"
#include "SFM_Image.h"

#ifdef __cplusplus
extern "C"
{
#endif

//
// Initialize serial communication
//
void UF_SetSerialWriteCallback( void (*Callback)( int, int ) );
void UF_SetSerialReadCallback( void (*Callback)( int, int ) );
UF_RET_CODE UF_InitCommPort(const char* commPort, int baudrate, BOOL asciiMode);
UF_RET_CODE UF_CloseCommPort(void);
void UF_Reconnect(void);
UF_RET_CODE UF_SetBaudrate(int baudrate);
void UF_SetAsciiMode( BOOL asciiMode );
int UF_GetBaudrate(void);

//
// Basic packet interface
//
UF_RET_CODE UF_SendPacket( BYTE command, UINT32 param, UINT32 size, BYTE flag, int timeout );
UF_RET_CODE UF_ReceivePacket(BYTE* packet, int timeout );
UF_RET_CODE UF_ReceiveDataPacket(BYTE command, BYTE* buf, UINT32 dataSize);
UF_RET_CODE UF_SendRawData(BYTE* buf, UINT32 size, int timeout);
UF_RET_CODE UF_ReceiveRawData( BYTE* buf, UINT32 size, int timeout, BOOL checkEndCode );
UF_RET_CODE UF_SendDataPacket( BYTE command, BYTE* buf, UINT32 dataSize, UINT32 dataPacketSize );
UF_RET_CODE UF_SendNetworkPacket( BYTE command, USHORT terminalID, UINT32 param, UINT32 size, BYTE flag, int timeout );
UF_RET_CODE UF_ReceiveNetworkPacket( BYTE* packet, int timeout );
void UF_SetDefaultPacketSize( int defaultSize );
int UF_GetDefaultPacketSize(void);

// Callback
void UF_SetSendPacketCallback( void (*Callback)( BYTE* ) );
void UF_SetReceivePacketCallback( void (*Callback)( BYTE* ) );
void UF_SetSendDataPacketCallback( void (*Callback)( int, int ) );
void UF_SetReceiveDataPacketCallback( void (*Callback)( int, int ) );
void UF_SetSendRawDataCallback( void (*Callback)( int, int ) );
void UF_SetReceiveRawDataCallback( void (*Callback)( int, int ) );

//
// Generic command interface
//
void UF_SetProtocol( UF_PROTOCOL protocol, UINT32 moduleID );
UF_PROTOCOL UF_GetProtocol(void);
UINT32 UF_GetModuleID(void);

UF_RET_CODE UF_Command(BYTE command, UINT32* param, UINT32* size, BYTE* flag );
UF_RET_CODE UF_CommandEx( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BOOL (*msgCallback)(BYTE) );
UF_RET_CODE UF_CommandSendData( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BYTE* data, UINT32 dataSize );
UF_RET_CODE UF_CommandSendDataEx( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BYTE* data, UINT32 dataSize, BOOL (*msgCallback)(BYTE), BOOL waitUserInput );
UF_RET_CODE UF_Cancel(BOOL receivePacket);

int UF_CalculateTimeout( int dataSize );
void UF_SetGenericCommandTimeout( int timeout );
void UF_SetInputCommandTimeout( int timeout );
int UF_GetGenericCommandTimeout(void);
int UF_GetInputCommandTimeout(void);
void UF_SetNetworkDelay( int delay );
int UF_GetNetworkDelay(void);

//
// Module information
//
UF_RET_CODE UF_Reset(void);

//
// System parameters
//
UF_RET_CODE UF_GetSysParameter( UF_SYS_PARAM parameter, UINT32* value );
UF_RET_CODE UF_SetSysParameter( UF_SYS_PARAM parameter, UINT32 value );
UF_RET_CODE UF_GetMultiSysParameter( int parameterCount, UF_SYS_PARAM* parameters, UINT32* values );
UF_RET_CODE UF_SetMultiSysParameter( int parameterCount, UF_SYS_PARAM* parameters, UINT32* values );
UF_RET_CODE UF_Save(void);
UF_RET_CODE UF_SaveConfiguration( const char* filename, const char* description, int numOfComponent, UFConfigComponentHeader* componentHeader, void** componentData );
UF_RET_CODE UF_ReadConfigurationHeader( const char* filename, UFConfigFileHeader* header );
UF_RET_CODE UF_LoadConfiguration( const char* filename, int numOfComponent, UF_CONFIG_TYPE* type );
UF_RET_CODE UF_MakeParameterConfiguration( UFConfigComponentHeader* configHeader, BYTE* configData );
UF_RET_CODE UF_ResetSystemConfiguration(void);

//
// Enroll
//
void UF_SetEnrollCallback( void (*Callback)( BYTE errCode, UF_ENROLL_MODE enrollMode, int numOfSuccess ) );
UF_RET_CODE UF_Enroll( UINT32 userID, UF_ENROLL_OPTION option, UINT32* enrollID, UINT32* imageQuality );
UF_RET_CODE UF_GetEnrollMode(void);
UF_RET_CODE UF_EnrollContinue(  UINT32 userID, UINT32* enrollID, UINT32* imageQuality );
UF_RET_CODE UF_EnrollAfterVerification( UINT32 userID, UF_ENROLL_OPTION option, UINT32* enrollID, UINT32* imageQuality );
UF_RET_CODE UF_EnrollTemplate( UINT32 userID, UF_ENROLL_OPTION option, UINT32 templateSize, BYTE* templateData, UINT32* enrollID );
UF_RET_CODE UF_EnrollMultipleTemplates( UINT32 userID, UF_ENROLL_OPTION option, int numOfTemplate, UINT32 templateSize, BYTE* templateData, UINT32* enrollID );
UF_RET_CODE UF_EnrollMultipleTemplatesEx( UINT32 userID, UF_ENROLL_OPTION option, int numOfTemplate, int numOfEnroll,  UINT32 templateSize, BYTE* templateData, UINT32* enrollID );
UF_RET_CODE UF_EnrollImage( UINT32 userID, UF_ENROLL_OPTION option, UINT32 imageSize, BYTE* imageData, UINT32* enrollID, UINT32* imageQuality );

SFM_RET_CODE SFM_Enroll_Scan( UINT32 userID, UF_ENROLL_OPTION option, UINT32* enrollID, UINT32* imageQuality, int enroll_mode );
SFM_RET_CODE SFM_Enroll_Template(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 templateSize, BYTE* templateData, UINT32 *enrollID, UINT32 *Number_Of_Feature);
SFM_RET_CODE SFM_Enroll_ImageEx(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 imageSize, BYTE* imageData, UINT32 *enrollID, UINT32 *Image_Quality);
SFM_RET_CODE SFM_Enroll_TemplateEx(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 templateSize, BYTE* templateData, UINT32 *enrollID, UINT32 *Number_Of_Feature);

//
// Identify
//
void UF_SetIdentifyCallback( void (*Callback)( BYTE ) );
UF_RET_CODE UF_Identify( UINT32* userID, BYTE* subID );
UF_RET_CODE UF_IdentifyTemplate( UINT32 templateSize, BYTE* templateData,  UINT32* userID, BYTE* subID );
UF_RET_CODE UF_IdentifyImage( UINT32 imageSize, BYTE* imageData,  UINT32* userID, BYTE* subID );

SFM_RET_CODE SFM_IdentifyImage( UINT32 imageSize, BYTE* imageData,  UINT32* userID, BYTE* subID );
SFM_RET_CODE SFM_IdentifyImageEx(UINT32 imageSize, BYTE* imageData, UINT32* userID, UINT32* subID);

//
// Verify (SFM_Verify.h)
//
void UF_SetVerifyCallback( void (*Callback)( BYTE ) );
UF_RET_CODE UF_Verify( UINT32 userID, BYTE* subID );
UF_RET_CODE UF_VerifyImage( UINT32 imageSize, BYTE* imageData,  UINT32 userID, BYTE* subID );
UF_RET_CODE UF_VerifyTemplate( UINT32 templateSize, BYTE* templateData,  UINT32 userID, BYTE* subID );
UF_RET_CODE UF_VerifyHostTemplate( UINT32 numOfTemplate, UINT32 templateSize, BYTE* templateData );

SFM_RET_CODE SFM_VerifyImage( UINT32 imageSize, BYTE* imageData,  UINT32 userID, BYTE* subID );
SFM_RET_CODE SFM_VerifyImageEx(UINT32 imageSize, BYTE* imageData, UINT32 userID, BYTE* subID );

//
// Image (SFM_Image.h)
//
UF_RET_CODE UF_ReadImage( UFImage* image );
UF_RET_CODE UF_ReadImageEx( UFImage* image, UF_IMAGE_TYPE type, int wsqBitRate );
UF_RET_CODE UF_ScanImage( UFImage* image );

SFM_RET_CODE SFM_ReadImage(unsigned char* image);
SFM_RET_CODE SFM_ReadImageEx(unsigned char* image, UF_IMAGE_TYPE type, int wsqBitRate);
SFM_RET_CODE SFM_ScanImage(unsigned char* image, UINT32* size);
SFM_RET_CODE SFM_ScanImageEx( unsigned char* image, UF_IMAGE_TYPE type, int wsqBitRate );

//
// GPIO (SFM_GPIO.h)
//
UF_RET_CODE UF_GetGPIOConfiguration( UF_GPIO_PORT port, UF_GPIO_MODE* mode, int* numOfData, UFGPIOData* data );
UF_RET_CODE UF_SetInputGPIO( UF_GPIO_PORT port, UFGPIOInputData data );
UF_RET_CODE UF_SetOutputGPIO( UF_GPIO_PORT port, int numOfData, UFGPIOOutputData* data );
UF_RET_CODE UF_SetBuzzerGPIO( UF_GPIO_PORT port, int numOfData, UFGPIOOutputData* data );
UF_RET_CODE UF_SetSharedGPIO( UF_GPIO_PORT port, UFGPIOInputData inputData, int numOfOutputData, UFGPIOOutputData* outputData );
UF_RET_CODE UF_DisableGPIO( UF_GPIO_PORT port );
UF_RET_CODE UF_ClearAllGPIO();
UF_RET_CODE UF_SetDefaultGPIO();
UF_RET_CODE UF_MakeGPIOConfiguration( UFConfigComponentHeader* configHeader, BYTE* configData );

//
// Delete (SFM_Delete.h)
//
void UF_SetDeleteCallback( void (*Callback)( BYTE ) );
UF_RET_CODE UF_Delete( UINT32 userID );
UF_RET_CODE UF_DeleteOneTemplate(UINT32 userID, int subID);
UF_RET_CODE UF_DeleteMultipleTemplates(UINT32 startUserID, UINT32 lastUserID, int* deletedUserID );
UF_RET_CODE UF_DeleteAll(void);
UF_RET_CODE UF_DeleteByScan(UINT32* userID);
UF_RET_CODE UF_DeleteAllAfterVerification(void);

//
// Upgrade
//
UF_RET_CODE UF_Upgrade( const char* firmwareFilename, int dataPacketSize );

//
// Security (SFM_Security.h)
//
SFM_RET_CODE SFM_SetEncryptionKey(BYTE* enc_keys);

// Templates & Database (SFM_Template.h)
UF_RET_CODE UF_GetNumOfTemplate( UINT32* numOfTemplate );
UF_RET_CODE UF_GetMaxNumOfTemplate( UINT32* maxNumOfTemplate );
UF_RET_CODE UF_GetAllUserInfo( UFUserInfo* userInfo, UINT32* numOfUser, UINT32* numOfTemplate );
UF_RET_CODE UF_GetAllUserInfoEx( UFUserInfoEx* userInfo, UINT32* numOfUser, UINT32* numOfTemplate );

UF_RET_CODE UF_SetAdminLevel( UINT32 userID, UF_ADMIN_LEVEL adminLevel );
UF_RET_CODE UF_GetAdminLevel( UINT32 userID, UF_ADMIN_LEVEL* adminLevel );
UF_RET_CODE UF_ClearAllAdminLevel(void);
UF_RET_CODE UF_ScanTemplate(BYTE* templateData, UINT32* templateSize, UINT32* imageQuality);
UF_RET_CODE UF_ReadOneTemplate( UINT32 userID, int subID, BYTE* templateData );
UF_RET_CODE UF_ReadTemplate( UINT32 userID, UINT32* numOfTemplate, BYTE* templateData );
UF_RET_CODE UF_CheckTemplate( UINT32 userID, UINT32* numOfTemplate );
UF_RET_CODE UF_FixProvisionalTemplate(void);
UF_RET_CODE UF_SetSecurityLevel( UINT32 userID, UF_USER_SECURITY_LEVEL securityLevel );
UF_RET_CODE UF_GetSecurityLevel( UINT32 userID, UF_USER_SECURITY_LEVEL* securityLevel );
UF_RET_CODE UF_FormatUserDatabase(void);

UF_RET_CODE UF_SaveDB( const char* fileName );
UF_RET_CODE UF_LoadDB( const char* fileName );
#ifdef __cplusplus
}
#endif

#endif

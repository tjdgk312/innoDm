
/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _SFM_COMMAND_H_
#define _SFM_COMMAND_H_

#include "SFM_RS232.h"
#include "SFM_Command.h"
#include "SFM_Error.h"
#include "SFM_Serial.h"

//
// Command definitions
//
typedef enum {
    SFM_COM_SW  = 0x01, // System Configuration
    SFM_COM_SF  = 0x02,
    SFM_COM_SR  = 0x03,
    SFM_COM_CS  = 0x1A,
    SFM_COM_SS  = 0x04,
    SFM_COM_CA  = 0x60,
    SFM_COM_ID  = 0x85,
    SFM_COM_UG  = 0x62,
    SFM_COM_AW  = 0x65,
    SFM_COM_AR  = 0x66,
    SFM_COM_AC  = 0x67,
    SFM_COM_RS  = 0xD0,
    SFM_COM_LM  = 0xB1,
    SFM_COM_UM  = 0xB0,
    SFM_COM_MP  = 0xB2,
    SFM_COM_OFF = 0xD2,
    SFM_COM_ES  = 0x05, // Enroll
    SFM_COM_ESA = 0x70,
    SFM_COM_EI  = 0x06,
    SFM_COM_EIX = 0x80,
    SFM_COM_ET  = 0x07,
    SFM_COM_ETX = 0x87,
    SFM_COM_EW  = 0x1C,
    SFM_COM_EWA = 0x71,
    SFM_COM_VS  = 0x08, // Verify
    SFM_COM_VI  = 0x09,
    SFM_COM_VIX = 0x82,
    SFM_COM_VT  = 0x10,
    SFM_COM_VW  = 0x1D,
    SFM_COM_VH  = 0x22,
    SFM_COM_WSL = 0x6B,
    SFM_COM_RSL = 0x6C,
    SFM_COM_IS  = 0x11, // Identify
    SFM_COM_II  = 0x12,
    SFM_COM_IIX = 0x81,
    SFM_COM_IT  = 0x13,
    SFM_COM_DA  = 0x17, // Delete
    SFM_COM_DAA = 0x74,
    SFM_COM_DT  = 0x16,
    SFM_COM_DS  = 0x1E,
    SFM_COM_DSA = 0x72,
    SFM_COM_DW  = 0x1F,
    SFM_COM_DWA = 0x73,
    SFM_COM_LT  = 0x18, // Template management
    SFM_COM_LTX = 0x86,
    SFM_COM_CT  = 0x19,
    SFM_COM_FP  = 0x23,
    SFM_COM_DP  = 0x24,
    SFM_COM_RI  = 0x20, // Get image and template
    SFM_COM_RIX = 0x84,
    SFM_COM_SI  = 0x15,
    SFM_COM_SIX = 0x83,
    SFM_COM_RT  = 0x14,
    SFM_COM_RTX = 0x89,
    SFM_COM_ST  = 0x21,
    SFM_COM_KS  = 0x35,
    SFM_COM_KW  = 0x34,
    SFM_COM_ML  = 0x31, // User Memoey management
    SFM_COM_MW  = 0x32,
    SFM_COM_MR  = 0x33,
    SFM_COM_TW  = 0x3A, // Time and log management
    SFM_COM_TR  = 0x3B,
    SFM_COM_LN  = 0x3C,
    SFM_COM_LR  = 0x3D,
    SFM_COM_LD  = 0x3E,
    SFM_COM_UW  = 0xA3,
    SFM_COM_UR  = 0xA4,
    SFM_COM_UC  = 0xA5,
    SFM_COM_UL  = 0xA6,
    SFM_COM_LC  = 0x3F,
    SFM_COM_RCL = 0xEC,
    SFM_COM_CCL = 0xEB,
   	SFM_COM_GR	= 0x36,
	SFM_COM_GW	= 0x37,
	SFM_COM_GC	= 0x38,
	SFM_COM_GD	= 0x39,
    SFM_COM_FR  = 0xFA,
    SFM_COM_FF  = 0xFF,
    SFM_FTR_BSN = 0xE0,
    SFM_FTR_FIB = 0xE1,
    SFM_FTR_PXY = 0xE2,
    SFM_FTR_SXY = 0xE3,
    SFM_FTR_EXP = 0xE4,
	SFM_COM_WME	= 0xF0,
	SFM_COM_RME	= 0xF1,
    SFM_COM_CME = 0xF2,
} SFM_COMMAND;

#ifdef __cplusplus
extern "C"
{
#endif

void UF_SetProtocol( UF_PROTOCOL protocol, UINT32 moduleID );
UF_PROTOCOL UF_GetProtocol(void);
UINT32 UF_GetModuleID(void);

UF_RET_CODE UF_Command(BYTE command, UINT32* param, UINT32* size, BYTE* flag );
UF_RET_CODE UF_CommandEx( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BOOL (*msgCallback)(BYTE) );
UF_RET_CODE UF_CommandSendData( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BYTE* data, UINT32 dataSize );
UF_RET_CODE UF_CommandSendDataEx( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BYTE* data, UINT32 dataSize, BOOL (*msgCallback)(BYTE), BOOL waitUserInput );
UF_RET_CODE UF_Cancel( BOOL receivePacket );

int UF_CalculateTimeout( int dataSize );
void UF_SetGenericCommandTimeout( int timeout );
void UF_SetInputCommandTimeout( int timeout );

int UF_GetGenericCommandTimeout(void);
int UF_GetInputCommandTimeout();
void UF_SetNetworkDelay( int delay );
int UF_GetNetworkDelay();

#ifdef __cplusplus
}
#endif

#endif
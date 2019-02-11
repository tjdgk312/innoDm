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

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SFM_Command.h"
#include "SFM_Packet.h"
#include "SFM_Error.h"
#include "SFM_Flag.h"
#include "SFM_SysParameter.h"

SFM_RET_CODE SFM_Facoty_command(void)
{
    int ret;
    int nMenu;
    char strMenu[100];
    ret = system("clear");

    printf("----------------------------------\n");
    printf("Factory Command\n");
    printf("----------------------------------\n");

    printf("1. Board Serial Number (Read only)\n");
    printf("2. Factory Info 1 & 2 (Read only)\n");
    printf("3. Position X & Y (Read)\n");
    printf("4. Scale X & Y (Read)\n");
    printf("5. Exposure - Normal high (Read)\n");

    printf(">>");
    fflush(stdin);
    gets(strMenu);
    nMenu = atoi(strMenu);

    switch(nMenu) {
        case 1:
            printf("[SFM Demo] Board Serial number : %ld\n", SFM_Facoty_BSN());
            break;
        case 2:
            SFM_Facoty_FIB();
            break;
        case 3:
            SFM_Facoty_Read_PosXY();
            break;
        case 4:
            SFM_Facoty_SXY();
            break;
        case 5:
            SFM_Facoty_EXP();
            break;
        default:
            printf("Please, try again!\n");
            getchar();
            break;
    }
}

int SFM_Facoty_BSN(void)
{
    int i, ret;
    int tryCount = 0;
    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = 0;
    BYTE packet[SFM_PACKET_LEN];
    int timeout;
    int bytes_avail = 0;
    BYTE end_code;

    UINT32 Board_Serial_Number;

    UF_SendPacket( SFM_FTR_BSN , param, size, flag, 10000 );

    UF_ReceivePacket(packet, 10000);

    Board_Serial_Number = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);

    return Board_Serial_Number;
}

void SFM_Facoty_FIB(void)
{
    int i, ret;
    int tryCount = 0;
    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = 0;
    BYTE packet[SFM_PACKET_LEN];
    int timeout;
    int bytes_avail = 0;
    BYTE end_code;


    UINT32 factory_info_1;
    UINT32 factory_info_2;

    UF_SendPacket( SFM_FTR_FIB , param, size, flag, 10000 );

    UF_ReceivePacket(packet, 10000);
    factory_info_1 = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);

    UF_ReceivePacket(packet, 10000);
    factory_info_2 = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);

    printf("[SFM Demo] factory_info_1 : %d, factory_info_2 : %d\n", factory_info_1, factory_info_2);
}

void SFM_Facoty_Read_PosXY(void)
{
    int i, ret;
    int tryCount = 0;
    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = 0;
    BYTE packet[SFM_PACKET_LEN];
    int timeout;
    int bytes_avail = 0;
    BYTE end_code;

    UINT32 input_x;
    UINT32 input_y;

    UF_SendPacket( SFM_FTR_PXY , param, size, flag, 10000 );

    UF_ReceivePacket(packet, 10000);
    input_x = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);

    UF_ReceivePacket(packet, 10000);
    input_y = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);

     printf("[SFM Demo] input x : %d, input y : %d \n", input_x, input_y);

}

void SFM_Facoty_SXY(void)
{
    int i, ret;
    int tryCount = 0;
    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = 0;
    BYTE packet[SFM_PACKET_LEN];
    int timeout;
    int bytes_avail = 0;
    BYTE end_code;

    UINT32 scale_factor_x;
    UINT32 scale_factor_y;

    UF_SendPacket( SFM_FTR_SXY , param, size, flag, 10000 );

    UF_ReceivePacket(packet, 10000);
    scale_factor_x = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);
    UF_ReceivePacket(packet, 10000);
    scale_factor_y = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);

    printf("[SFM Demo] scale_factor_x : %d, scale_factor_y : %d\n", scale_factor_x, scale_factor_y);

}

void SFM_Facoty_EXP(void)
{
    int i, ret;
    int tryCount = 0;
    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = 0;
    BYTE packet[SFM_PACKET_LEN];
    int timeout;
    int bytes_avail = 0;
    BYTE end_code;
    
    UINT32 Exposure;

    UF_SendPacket( SFM_FTR_EXP , param, size, flag, 10000 );

    UF_ReceivePacket(packet, 10000);
    Exposure =  SFM_GetPacketValue(SFM_PACKET_PARAM, packet);

    printf("[SFM Demo] Exposure : %d\n", Exposure);
}
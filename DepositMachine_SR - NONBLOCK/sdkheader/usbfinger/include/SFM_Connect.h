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

#ifndef _SFM_CONNECT_H_
#define _SFM_CONNECT_H_

#include "SFM_SysParameter.h"

extern int g_nPort;
extern int g_nProtocol;
extern int g_nModuleID;
extern int g_bConnectionStatus;

/* System Parameter */
extern SysParameter *sysParameter;

void CommunicationSetup(void);
void CurrentCummunicationSetup(void);
void CommunicationSetupMenu();

#endif
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

#ifndef _SFM_FLAG_H_
#define _SFM_FLAG_H_

//
// Flag definitions
//
typedef enum {
    FLAG_CHECK_ID                = 0x70,
    FLAG_ADD_NEW                 = 0x71,
    FLAG_CONTINUE                = 0x74,
    FLAG_AUTO_ID                 = 0x79,
    FLAG_CHECK_FINGER            = 0x84,
    FLAG_CHECK_FINGER_AUTO_ID    = 0x85,
    FLAG_ADD_DURESS              = 0x92,     // SFM3500, SFM5500 only
} SFM_FLAG;

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif
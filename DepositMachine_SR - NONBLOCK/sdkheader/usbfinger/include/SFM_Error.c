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

#include "SFM_Error.h"

SFM_RET_CODE UF_GetErrorCode(SFM_PROTOCOL_RET_CODE retCode)
{
	switch( retCode ) {
		case SFM_PROTO_RET_SCAN_FAIL:
			return SFM_ERR_SCAN_FAIL;

		case SFM_PROTO_RET_NOT_FOUND:
			return SFM_ERR_NOT_FOUND;

		case SFM_PROTO_RET_NOT_MATCH:
			return SFM_ERR_NOT_MATCH;

		case SFM_PROTO_RET_TRY_AGAIN:
			return SFM_ERR_TRY_AGAIN;

		case SFM_PROTO_RET_TIME_OUT:
			return SFM_ERR_TIME_OUT;

		case SFM_PROTO_RET_MEM_FULL:
			return SFM_ERR_MEM_FULL;

		case SFM_PROTO_RET_EXIST_ID:
			return SFM_ERR_EXIST_ID;

		case SFM_PROTO_RET_FINGER_LIMIT:
			return SFM_ERR_FINGER_LIMIT;

		case SFM_PROTO_RET_UNSUPPORTED:
			return SFM_ERR_UNSUPPORTED;

		case SFM_PROTO_RET_INVALID_ID:
			return SFM_ERR_INVALID_ID;

		case SFM_PROTO_RET_TIMEOUT_MATCH:
			return SFM_ERR_TIMEOUT_MATCH;

		case SFM_PROTO_RET_BUSY:
			return SFM_ERR_BUSY;

		case SFM_PROTO_RET_CANCELED:
			return SFM_ERR_CANCELED;

		case SFM_PROTO_RET_DATA_ERROR:
			return SFM_ERR_DATA_ERROR;

		case SFM_PROTO_RET_EXIST_FINGER:
			return SFM_ERR_EXIST_FINGER;

		case SFM_PROTO_RET_DURESS_FINGER:
			return SFM_ERR_DURESS_FINGER;

		case SFM_PROTO_RET_CARD_ERROR:
			return SFM_ERR_CARD_ERROR;

		case SFM_PROTO_RET_LOCKED:
			return SFM_ERR_LOCKED;

		case SFM_PROTO_RET_ACCESS_NOT_GRANTED:
			return SFM_ERR_ACCESS_NOT_GRANTED;

		case SFM_PROTO_RET_EXCEED_ENTRANCE_LIMIT:
			return SFM_ERR_EXCEED_ENTRANCE_LIMIT;

		case SFM_PROTO_RET_REJECTED_ID:
			return SFM_ERR_REJECTED_ID;

		case SFM_PROTO_FAKE_DETECTED:
			return SFM_ERR_FAKE_DETECTED;

		default:
			return SFM_ERR_UNKNOWN;
	}
}
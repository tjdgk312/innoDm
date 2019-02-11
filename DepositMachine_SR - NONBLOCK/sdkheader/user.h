#ifndef __INNOSTEC_USER_API_h__
#define  __INNOSTEC_USER_API_h__
#include <time.h>

//#define SDK_DATABASE

/*! \file user.h
 * \brief Api for controlling the user's credentials in the Vault Machine.
 * Author: Harold L Marzan, MMC.
 * Requested By: Metaconxept, Dopel.
*/

#ifdef SDK_DATABASE

//!User Creation Errors
#define USER_CREATE_STATUS_NO_ERROR					0
#define USER_CREATE_STATUS_SUCCESS					USER_CREATE_STATUS_NO_ERROR
#define USER_CREATE_STATUS_USER_ALREADY_EXISTS		1
#define USER_CREATE_STATUS_INVALID_ROLES			2
#define USER_CREATE_STATUS_USER_RECORDS_FULL		3                                           //  Discuss limit count     now = 300;
#define USER_CREATE_STATUS_DATABASE_ERROR			4

#define USER_RECORDS_LIMIT 300

//!User Roles
#define USER_ROLES_ALLOW_LOGIN						1
#define USER_ROLES_ALLOW_QUERY						2
#define USER_ROLES_ALLOW_DEPOSIT					4
#define USER_ROLES_ALLOW_LOGOFF						8
#define USER_ROLES_ALLOW_CHANGE_PASSWORD			0x10
#define USER_ROLES_ALLOW_PRINTING					0x20
#define USER_ROLES_ALLOW_SET_ALARM					0x40
#define USER_ROLES_ALLOW_SET_FINGERPRINT			0x80
#define USER_ROLES_ALLOW_OPEN_SCROW					0x100
#define USER_ROLES_ALLOW_OPEN_FRONT_GATE			0x200
#define USER_ROLES_ALLOW_FINGERPRINT_LOGIN			0x400


typedef enum __tagHIGH_LEVEL_USER_ROLES {
	USER_ROLES_ALLOW_DEPOSITANT_USER	=		(USER_ROLES_ALLOW_LOGIN 				|
													USER_ROLES_ALLOW_DEPOSIT			|
													USER_ROLES_ALLOW_LOGOFF),
													
	USER_ROLES_ALLOW_TRIPULANT_USER =			(USER_ROLES_ALLOW_LOGIN					|
													 USER_ROLES_ALLOW_OPEN_FRONT_GATE   |
													 USER_ROLES_ALLOW_LOGOFF),
													
	USER_ROLES_ALLOW_ADMIN_ALL		=			(USER_ROLES_ALLOW_LOGIN 				|
													 USER_ROLES_ALLOW_QUERY 			|
													 USER_ROLES_ALLOW_DEPOSIT 			|
													 USER_ROLES_ALLOW_LOGOFF			|
													 USER_ROLES_ALLOW_CHANGE_PASSWORD	|
													 USER_ROLES_ALLOW_PRINTING			|
													 USER_ROLES_ALLOW_SET_ALARM			|
													 USER_ROLES_ALLOW_SET_FINGERPRINT	|
													 USER_ROLES_ALLOW_OPEN_SCROW		|
													 USER_ROLES_ALLOW_OPEN_FRONT_GATE	|
													 USER_ROLES_ALLOW_FINGERPRINT_LOGIN),
}HIGH_LEVEL_USER_ROLES;



//!Create a user in the vault System, with the capabilities/functions specified in roles. 
//!The user does not expires.
//!
int usrCreateUser(const char* clientName, //!< The client (commercial/company) name that the user belongs to                    // DEFINE      in database   UserId is primary key
                  const char* userFullName, //!< The user's full name
                  const char* userName, 	//!< The user name
                  const char* password, 	//!< The user's password/secret or PIN
				  int roles, 				//!< The roles assigned to the user
				  int UserId				//!< A provided external system's user Id
				  );

//!Create a user in the vault System, with the capabilities/functions specified in roles with an expiration date and time.
//!The user account will block after that expiration date and time.
//!
int usrCreateUserEx(const char* clientName, //!< The client (commercial/company) name that the user belongs to                  // DEFINE
                      const char* userFullName, //!< The user's full name
                      const char* userName, 	//!< The user name
                      const char* password, 	//!< The user's password/secret or PIN
					  int roles, 				//!< The roles assigned to the user
					  int UserId,				//!< A provided external system's user Idconst char* clientName
					  time_t expiresDateTime	//!< The user's credentials expiration date and time 
					);


//!User Operation Status
#define  USER_STATUS_OK								0
#define  USER_STATUS_AUTHENTICATED					1
#define  USER_STATUS_BLOCKED						2
#define  USER_STATUS_NO_LOGIN						3
#define  USER_STATUS_NO_ROLES						4

//!Allows users to login to the vault machine using their user credentials
int usrLogin(const char* userName, const char* password);               // DEFINE

//!Logs off the current user
int usrLogoff();                                                        // DEFINE

//!Blocks a user in the vault system, from its user Id
int usrBlockUser(int userId);                                           // DEFINE

//!Unblocks a user that was previously locked/blocked to use the vault
int usrUnblockUser(int userId);                                         // DEFINE

//!Get the userId from the specified user name
int usrGetUserId(const char *userName);                                 // DEFINE

//!Get the user's full name from the user Id
char* usrGetUserFullName(int userId);                                   // DEFINE

//!Get the user's client name from the user Id
char* usrGetUserClientName(int userId);                                 // DEFINE

//!Allows to reset the password for the specified user
int usrResetPasswordOrPIN(int userId, char* password);                  // DEFINE  this function is change user roles???

//!Requests the user to change his/her password
int usrRequestUserChangePasswordOrPIN(int userId, char* oldpassword, char* password);   // DEFINE   -but I do not know what's different from usrResetPasswordOrPIN();


#define USER_AUTH_USER_NAME_LEN 64
#define USER_AUTH_FULL_NAME_LEN 256
#define USER_AUTH_CLIENT_NAME_LEN 512
//!Holds user information regarding a user
typedef struct __tagUSER_INFO
{
	int UserId;									//!< The Id of the user we are requesting this info
	char clientName[USER_AUTH_CLIENT_NAME_LEN + 1];	//!< The user's client name (may be used in the voucher printing) 
	char fullName[USER_AUTH_FULL_NAME_LEN + 1];	//!< The user's full name (may be used in the voucher printing)	
	char userName[USER_AUTH_USER_NAME_LEN + 1]; //!< The user name
	int Status;									//!< The status of this user's account
	int Roles;									//!< Get the roles assigned to this user's account

	time_t CreationDateTime;					//!< The creation date and time of this user
	time_t LastDepositDateTime;					//!< The last deposit date and time performed by this user
	
	time_t ExpirationDateTime;					//!< The date and time of the expiration of this user's account
}USER_INFO, *LPUSER_INFO;

//!Get User information from the userId
LPUSER_INFO usrGetUserInfo(int userId);         // DEFINE

//!Get the roles assigned to the user
int usrGetUserRoles(int userId);                // DEFINE

//!Set the roles to the specified user
int usrSetRoles(int userId, int roles);         // DEFINE

//!Set User information as batch from a user info structure.
int usrSetUserInfo(LPUSER_INFO user_info_ptr);                      // DEFINE   -  LastDepositDateTime also trans?
#endif

#endif //__INNOSTEC_USER_API_h__


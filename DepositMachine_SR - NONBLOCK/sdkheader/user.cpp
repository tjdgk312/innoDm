#include "user.h"
#include "sdk_global.h"

//#define SDK_DATABASE


#ifdef SDK_DATABASE         // SDK 데이터베이스사용  - PC와의 통신작업에 있어 번거로움이 있음
int usrLogin(const char *userName, const char *password)
{
    QList<QVariant> bufList;
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    int result = pMain->pDatabase->SelectRecord("USER","password,roles,UserId,expiresDateTime,Status",QString("userName = '%1'").arg(userName),&bufList);
//    qDebug() << "login USER DB" << buf[0] << buf[1] << buf[2] << buf[3] << buf[4];
    if(!result){                                                            // 존재확인
        qDebug() << "NOT EXIST NO_LOGIN" << result;
        return USER_STATUS_NO_LOGIN;
    }
    if(bufList.at(4).toBool()){                                                    // 블록확인
        qDebug() << "USER BLOCKED NO_LOGIN" << bufList.at(4);
        return USER_STATUS_BLOCKED;
    }
    if((bufList.at(1).toInt() & USER_ROLES_ALLOW_LOGIN) != USER_ROLES_ALLOW_LOGIN){                     // 권한확인
        qDebug() << "NO ROLE NO_LOGIN";
        return USER_STATUS_NO_ROLES;
    }
    if(bufList.at(0).toString() != password){                      // 비번확인
        qDebug() << "PASSWORD NO_LOGIN";
        return USER_STATUS_NO_LOGIN;
    }
    if(bufList.at(3).toInt() != 0){                                            // 시간제한유저인지 확인
        if(bufList.at(3).toInt() < QDateTime::currentDateTime().toTime_t() ){  // 시간종료유저
            qDebug() << "limit expired BLOCKED" << bufList.at(3);
            return USER_STATUS_BLOCKED;
        }
        else{                                                           // 시간남은유저
            qDebug() << "limit not expired AUTHENTICATED";
            pMain->loginState = true;
            pMain->currentUserId = bufList.at(2).toInt();
            pMain->currentLogUser= userName;
            pMain->currentUserRole = bufList.at(1).toInt();
            return USER_STATUS_AUTHENTICATED;
        }
    }
    else{
//         bufList.at(2).toString().toStdString().c_str();
        qDebug() << "AUTHENTICATED";
        pMain->loginState = true;
        pMain->currentUserId = bufList.at(2).toInt();
        pMain->currentLogUser= userName;
        pMain->currentUserRole = bufList.at(1).toInt();
        return USER_STATUS_AUTHENTICATED;
    }

}

int usrCreateUser(const char *clientName, const char *userFullName, const char *userName, const char *password, int roles, int UserId)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    if((roles != USER_ROLES_ALLOW_DEPOSITANT_USER) && (roles != USER_ROLES_ALLOW_TRIPULANT_USER) && (roles != USER_ROLES_ALLOW_ADMIN_ALL)){
        return USER_CREATE_STATUS_INVALID_ROLES;
    }
    // test
    int cntidx = pMain->pDatabase->SelectMinMax("*","USER",3);

    if(USER_RECORDS_LIMIT  <= pMain->pDatabase->SelectMinMax("*","USER",3)){
        return USER_CREATE_STATUS_USER_RECORDS_FULL;
    }

    QList<QVariant> bufList;
    int idx = pMain->pDatabase->SelectRecord("USER","UserId",QString("UserId = %1").arg(UserId),&bufList);
    if(idx == 0){          // NOT EXIST
        bool result = pMain->pDatabase->InsertRecord("USER", QString("'%1', '%2', '%3', '%4', %5, %6, 0, 0, 0, 0").arg(clientName).arg(userFullName).arg(userName).arg(password).arg(roles).arg(UserId));
        if(result){
            return USER_CREATE_STATUS_SUCCESS;
        }
        else{
            return USER_CREATE_STATUS_DATABASE_ERROR;
        }
    }
    else{                                 // EXIST
        qDebug() << "select record EXIST" << bufList.at(0);
        return USER_CREATE_STATUS_USER_ALREADY_EXISTS;
    }
}

int usrCreateUserEx(const char *clientName, const char *userFullName, const char *userName, const char *password, int roles, int UserId, time_t expiresDateTime)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    // 기계 켤때 Ex 유저 읽어서 Delete 처리해야 함
    if((roles != USER_ROLES_ALLOW_DEPOSITANT_USER) && (roles != USER_ROLES_ALLOW_TRIPULANT_USER) && (roles != USER_ROLES_ALLOW_ADMIN_ALL)){
        return USER_CREATE_STATUS_INVALID_ROLES;
    }
    if(USER_RECORDS_LIMIT  <= pMain->pDatabase->SelectMinMax("","USER",3)){
        return USER_CREATE_STATUS_USER_RECORDS_FULL;
    }

    QList<QVariant> bufList;
    pMain->pDatabase->SelectRecord("USER","UserId",QString("UserId = %1").arg(UserId),&bufList);
    if(!bufList.at(0).isValid()){          // NOT EXIST
        bool result = pMain->pDatabase->InsertRecord("USER", QString("'%1', '%2', '%3', '%4', %5, %6, %7, 0, 0, 0").arg(clientName).arg(userFullName).arg(userName).arg(password).arg(roles).arg(UserId).arg(expiresDateTime));
        if(result){
            return USER_CREATE_STATUS_SUCCESS;
        }
        else{
            return USER_CREATE_STATUS_DATABASE_ERROR;
        }
    }
    else{                                 // EXIST
        qDebug() << "select record EXIST" << bufList.at(0);
        return USER_CREATE_STATUS_USER_ALREADY_EXISTS;
    }
}

int usrLogoff()
{
    pMain->loginState = false;
//    pMain->currentLogId = buf[2].toString().toUtf8().data();
//    pMain->currentLogUser= userName;
//    pMain->currentUserRole = buf[1].toInt();
}

int usrBlockUser(int userId)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    bool result = pMain->pDatabase->UpdateRecord("USER","Status=1",QString("UserId = %1").arg(userId));
    if(result) return USER_STATUS_OK;
}

int usrUnblockUser(int userId)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    bool result = pMain->pDatabase->UpdateRecord("USER","Status=0",QString("UserId = %1").arg(userId));
    if(result) return USER_STATUS_OK;
}

int usrGetUserId(const char *userName)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    QList<QVariant> bufList;
    int idx = pMain->pDatabase->SelectRecord("USER","UserId",QString("userName = '%1'").arg(userName),&bufList);
    return bufList.at(0).toInt();
}

char *usrGetUserFullName(int userId)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    QList<QVariant> bufList;
    int idx = pMain->pDatabase->SelectRecord("USER","userFullName",QString("UserId = %1").arg(userId),&bufList);
    return bufList.at(0).toString().toUtf8().data();
}

char *usrGetUserClientName(int userId)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    QList<QVariant> bufList;
    int idx = pMain->pDatabase->SelectRecord("USER","clientName",QString("UserId = %1").arg(userId),&bufList);
    return bufList.at(0).toString().toUtf8().data();
}

int usrResetPasswordOrPIN(int userId, char *password)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    bool result = pMain->pDatabase->UpdateRecord("USER",QString("password='%1'").arg(password),QString("UserId = %1").arg(userId));
    if(result) return USER_STATUS_OK;
}

int usrRequestUserChangePasswordOrPIN(int userId, char *oldpassword, char *password)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    if((pMain->currentUserRole & USER_ROLES_ALLOW_CHANGE_PASSWORD) != USER_ROLES_ALLOW_CHANGE_PASSWORD){
//        return USER_STATUS_ERROR;
    }
    bool result = pMain->pDatabase->UpdateRecord("USER",QString("password='%1'").arg(password),QString("UserId = %1").arg(userId));
    if(result) return USER_STATUS_OK;
}

LPUSER_INFO usrGetUserInfo(int userId)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    USER_INFO strInfo;
    LPUSER_INFO userInfo = &strInfo;

    QList<QVariant> bufList;
    int idx = pMain->pDatabase->SelectRecord("USER","clientName,userFullName,userName,Status,roles,expiresDateTime,creationDateTime,lastDepositDateTime",QString("UserId = %1").arg(userId),&bufList);
    userInfo->UserId = userId;
    memcpy(userInfo->clientName,bufList.at(0).toString().toUtf8().data(),bufList.at(0).toString().size());
    memcpy(userInfo->fullName,bufList.at(1).toString().toUtf8().data(),bufList.at(1).toString().size());
    memcpy(userInfo->userName,bufList.at(2).toString().toUtf8().data(),bufList.at(2).toString().size());
    userInfo->Status = bufList.at(3).toInt();
    userInfo->Roles = bufList.at(4).toInt();
    userInfo->ExpirationDateTime = bufList.at(5).toInt();
    userInfo->CreationDateTime = bufList.at(6).toInt();
    userInfo->LastDepositDateTime = bufList.at(7).toInt();

    return userInfo;
}

int usrGetUserRoles(int userId)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    QList<QVariant> bufList;
    int idx = pMain->pDatabase->SelectRecord("USER","roles",QString("UserId = %1").arg(userId),&bufList);
    return bufList.at(0).toInt();
}

int usrSetRoles(int userId, int roles)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    bool result = pMain->pDatabase->UpdateRecord("USER",QString("roles= %1").arg(roles),QString("UserId = %1").arg(userId));
    if(result) return USER_STATUS_OK;
}

// "clientName,userFullName,userName,Status,roles,expiresDateTime,creationDateTime,lastDepositDateTime"
int usrSetUserInfo(LPUSER_INFO user_info_ptr)
{
    pMain->pDatabase->db.setDatabaseName(PATH_USER);
    pMain->pDatabase->db.open();
    QString setupQuery;
    setupQuery.append(QString("UserId = %1,").arg(user_info_ptr->UserId));
    setupQuery.append(QString("clientName = '%1',").arg(user_info_ptr->clientName));
    setupQuery.append(QString("userFullName = '%1',").arg(user_info_ptr->fullName));
    setupQuery.append(QString("userName = '%1',").arg(user_info_ptr->userName));
    setupQuery.append(QString("Status = %1,").arg(user_info_ptr->Status));
    setupQuery.append(QString("roles = %1,").arg(user_info_ptr->Roles));
    setupQuery.append(QString("expiresDateTime = %1,").arg(user_info_ptr->ExpirationDateTime));
    setupQuery.append(QString("creationDateTime = %1,").arg(user_info_ptr->CreationDateTime));
    setupQuery.append(QString("lastDepositDateTime = %1").arg(user_info_ptr->LastDepositDateTime));
    bool result = pMain->pDatabase->UpdateRecord("USER",setupQuery,QString("UserId = %1").arg(user_info_ptr->UserId));
    if(result) return USER_STATUS_OK;
}
#endif

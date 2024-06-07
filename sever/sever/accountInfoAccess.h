/* 이름: accountInfoAccess.h
 *  내용: 계정 정보 저장 및 참조 함수들의 선언
 */

#ifndef __ACCACCESS_H__
#define __ACCACCESS_H__
#include <WinSock2.h>
#include "accountInfo.h"

int AddaccInfo(char* id, char* password, char* nickname);
accInfo* GetAccPtrByid(char* id);
int RegistAccstomer(SOCKET clntSock);
int LoginAccstomer(SOCKET clntSock);
int IsRegistid(char* id);
accInfo* GetAccPtrBypassword(char* password);
accInfo* GetAccPtrBynick(char* nick);

int IsRegistnickname(char* nickname);

void StoreacclistToFile(void);
void LoadacclistFromFile(void);

#endif

/* end of file */#pragma once

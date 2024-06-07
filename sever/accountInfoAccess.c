/* 이름: accountInfoAccess.c
 *  내용: 계정 정보 저장 및 참조 함수들의 정의
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<WinSock2.h>
#include "accountInfo.h"
#include "accountInfoAccess.h"

#define MAX_ACCOUNT  100
#define ACC_BAKCUP_FILE   "accInfo.txt"

static accInfo * acclist[MAX_ACCOUNT];
static int numOfAccount = 0;
int pass = -1;

/* 함    수: int AddaccInfo (char * id, char * password, char * nickname)
 * 기    능: accInfo 할당 및 저장.
 * 반    환: 성공 시 '등록된 정보의 개수', 실패 시 0을 반환.
 *
 */
int AddaccInfo(char* id, char* password, char* nickname)
{
	accInfo* pAcc;

	if (numOfAccount >= MAX_ACCOUNT)
		return 0;  // 입력 실패

	pAcc = (accInfo*)malloc(sizeof(accInfo));
	strcpy(pAcc->id, id);
	strcpy(pAcc->password, password);
	strcpy(pAcc->nickname, nickname);

	acclist[numOfAccount++] = pAcc;

	/* 파일에 데이터 저장 */
	StoreacclistToFile();

	return numOfAccount;   // 입력 성공
}


/* 함    수: accInfo * GetAccPtrByid(char * id)
 * 기    능: 해당 id의 정보를 담고 있는 변수의 포인터 반환
 * 반    환: 등록 안된 id의 경우 NULL 포인터 반환.
 *
 */
accInfo* GetAccPtrByid(char* id)
{
	int i;

	for (i = 0; i < numOfAccount; i++)
	{
		if (!strcmp(acclist[i]->id, id))
			return acclist[i];
	}

	return (accInfo*)0;
}

accInfo* GetAccPtrBypassword(char* password)
{
	int i;

	for (i = 0; i < numOfAccount; i++)
	{
		if (!strcmp(acclist[i]->password, password))
			pass = i;
		return acclist[i];

	}

	return (accInfo*)0;
}

accInfo* GetAccPtrBynick(char* nick)
{
	int i;

	for (i = 0; i < numOfAccount; i++)
	{
		if (!strcmp(acclist[i]->nickname, nick))
			return acclist[i];
	}

	return (accInfo*)0;
}




int RegistAccstomer(SOCKET clntSock)
{
	char id[id_LEN];
	char password[pass_LEN];
	char nickname[nick_LEN];
	char approval[2] = { 1,1 };

	recv(clntSock, id, sizeof(id), 0);

	if (IsRegistid(id))
	{
		puts("이미 사용중인 ID로 계정 생성에 접근했습니다.");
		send(clntSock, approval, sizeof(approval), 0);
		return 0;
	}
	recv(clntSock, password, sizeof(password), 0);

	recv(clntSock, nickname, sizeof(nickname), 0);

	if (IsRegistnickname(nickname))
	{
		puts("이미 사용중인 NICKNAME으로 계정 생성에 접근했습니다.");
		send(clntSock, approval, sizeof(approval), 0);
		return 0;
	}

	if (!AddaccInfo(id, password, nickname))
	{
		puts("정상적인 데이터 저장에 실패하였습니다.");
		send(clntSock, approval, sizeof(approval), 0);
		return 0;
	}

	puts("가입이 완료되었습니다.");

	return 1;
}

int LoginAccstomer(SOCKET clntSock)
{
	char id[id_LEN];
	char password[pass_LEN];
	char nickname[nick_LEN];
	char approval[2] = { 1,1 };
	int temp;


	while (1)
	{
		temp = 0;
		recv(clntSock, id, sizeof(id), 0);
		if (IsRegistid(id))
		{
			temp++;
		}
		recv(clntSock, password, sizeof(password), 0);
		if (IsRegistpassword(password))
		{
			temp++;
		}
		if (temp == 2) {
			strcpy(nickname, acclist[pass]->nickname);
			send(clntSock, nickname, sizeof(nickname), 0);
			return 1;
		}
	}
}




/* 함    수: int IsRegistid(char * id)
 * 기    능: 가입 된 계정인지 확인.
 * 반    환: 가입 되었으면 1, 아니면 0 반환.
 *
 */
int IsRegistid(char* id)
{
	accInfo* pAcc = GetAccPtrByid(id);

	if (pAcc == 0)
		return 0;  // 가입 안된 경우.
	else
		return 1;  // 가입 된 경우.
}

/*함    수 : int IsRegistpassword(char* password)
* 기    능 : 가입 된 별명인지 확인.
* 반    환 : 가입 되었으면 1, 아니면 0 반환.
*
*/
int IsRegistpassword(char* password)
{
	accInfo* pAcc = GetAccPtrBynick(password);

	if (pAcc == 0)
		return 0;  // 가입 안된 경우.
	else
		return 1;  // 가입 된 경우.
}

/* 함    수: int IsRegistnickname(char* nickname)
 * 기    능: 가입 된 별명인지 확인.
 * 반    환: 가입 되었으면 1, 아니면 0 반환.
 *
 */
int IsRegistnickname(char* nick)
{
	accInfo* pAcc = GetAccPtrBynick(nick);

	if (pAcc == 0)
		return 0;  // 가입 안된 경우.
	else
		return 1;  // 가입 된 경우.
}


/* 함    수: void StoreacclistToFile(void)
 * 기    능: 고객 정보 파일에 저장
 * 반    환: void.
 *
 */
void StoreacclistToFile(void)
{
	int i;


	FILE* fp = fopen(ACC_BAKCUP_FILE, "wb");
	if (fp == NULL)
		return;

	fwrite(&numOfAccount, sizeof(int), 1, fp);

	for (i = 0; i < numOfAccount; i++)
		fwrite(acclist[i], sizeof(accInfo), 1, fp);

	fclose(fp);
}

/* 함    수: void LoadacclistFromFile(void)
 * 기    능: 고객 정보 파일로부터 복원
 * 반    환: void.
 *
 */
void LoadacclistFromFile(void)
{
	int i;

	FILE* fp = fopen(ACC_BAKCUP_FILE, "rb");
	if (fp == NULL)
		return;

	fread(&numOfAccount, sizeof(int), 1, fp);

	for (i = 0; i < numOfAccount; i++)
	{
		acclist[i] = (accInfo*)malloc(sizeof(accInfo));
		fread(acclist[i], sizeof(accInfo), 1, fp);
	}

	fclose(fp);
}

/* end of file */
/* �̸�: accountInfoAccess.c
 *  ����: ���� ���� ���� �� ���� �Լ����� ����
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

/* ��    ��: int AddaccInfo (char * id, char * password, char * nickname)
 * ��    ��: accInfo �Ҵ� �� ����.
 * ��    ȯ: ���� �� '��ϵ� ������ ����', ���� �� 0�� ��ȯ.
 *
 */
int AddaccInfo(char* id, char* password, char* nickname)
{
	accInfo* pAcc;

	if (numOfAccount >= MAX_ACCOUNT)
		return 0;  // �Է� ����

	pAcc = (accInfo*)malloc(sizeof(accInfo));
	strcpy(pAcc->id, id);
	strcpy(pAcc->password, password);
	strcpy(pAcc->nickname, nickname);

	acclist[numOfAccount++] = pAcc;

	/* ���Ͽ� ������ ���� */
	StoreacclistToFile();

	return numOfAccount;   // �Է� ����
}


/* ��    ��: accInfo * GetAccPtrByid(char * id)
 * ��    ��: �ش� id�� ������ ��� �ִ� ������ ������ ��ȯ
 * ��    ȯ: ��� �ȵ� id�� ��� NULL ������ ��ȯ.
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
		puts("�̹� ������� ID�� ���� ������ �����߽��ϴ�.");
		send(clntSock, approval, sizeof(approval), 0);
		return 0;
	}
	recv(clntSock, password, sizeof(password), 0);

	recv(clntSock, nickname, sizeof(nickname), 0);

	if (IsRegistnickname(nickname))
	{
		puts("�̹� ������� NICKNAME���� ���� ������ �����߽��ϴ�.");
		send(clntSock, approval, sizeof(approval), 0);
		return 0;
	}

	if (!AddaccInfo(id, password, nickname))
	{
		puts("�������� ������ ���忡 �����Ͽ����ϴ�.");
		send(clntSock, approval, sizeof(approval), 0);
		return 0;
	}

	puts("������ �Ϸ�Ǿ����ϴ�.");

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




/* ��    ��: int IsRegistid(char * id)
 * ��    ��: ���� �� �������� Ȯ��.
 * ��    ȯ: ���� �Ǿ����� 1, �ƴϸ� 0 ��ȯ.
 *
 */
int IsRegistid(char* id)
{
	accInfo* pAcc = GetAccPtrByid(id);

	if (pAcc == 0)
		return 0;  // ���� �ȵ� ���.
	else
		return 1;  // ���� �� ���.
}

/*��    �� : int IsRegistpassword(char* password)
* ��    �� : ���� �� �������� Ȯ��.
* ��    ȯ : ���� �Ǿ����� 1, �ƴϸ� 0 ��ȯ.
*
*/
int IsRegistpassword(char* password)
{
	accInfo* pAcc = GetAccPtrBynick(password);

	if (pAcc == 0)
		return 0;  // ���� �ȵ� ���.
	else
		return 1;  // ���� �� ���.
}

/* ��    ��: int IsRegistnickname(char* nickname)
 * ��    ��: ���� �� �������� Ȯ��.
 * ��    ȯ: ���� �Ǿ����� 1, �ƴϸ� 0 ��ȯ.
 *
 */
int IsRegistnickname(char* nick)
{
	accInfo* pAcc = GetAccPtrBynick(nick);

	if (pAcc == 0)
		return 0;  // ���� �ȵ� ���.
	else
		return 1;  // ���� �� ���.
}


/* ��    ��: void StoreacclistToFile(void)
 * ��    ��: �� ���� ���Ͽ� ����
 * ��    ȯ: void.
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

/* ��    ��: void LoadacclistFromFile(void)
 * ��    ��: �� ���� ���Ϸκ��� ����
 * ��    ȯ: void.
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
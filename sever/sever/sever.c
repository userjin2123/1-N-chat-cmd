#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <process.h>
#include "accountInfo.h"
#include "accountInfoAccess.h"
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)


void ErrorHandling(char* msg);
unsigned int WINAPI HandleClient(void* arg);
void SendMsg(char* msg, int len);
int clntCnt = 0;
HANDLE hMutex;
SOCKET clntSocks[10];


#define BUFSIZE 1024				//ä�� ����
#define QUITREQ		-1



int main(int argc, char* argv[])
{
	//SOCKET ����
	WSADATA wsaData;
	SOCKET servSock, clntSock;
	SOCKADDR_IN servAddr, clntAddr;


	//�޽��� �޴� ���ڿ�
	char port[8];					//��Ʈ �迭
	int clntAddrSize;					//Ŭ���̾�Ʈ �ּ� ������

	HANDLE hThread;
	HANDLE hThread2;// ������ �ڵ�
	DWORD dwThreadID;
	DWORD dwThreadID2;	// ������ ID


	char welcome[1024] = "\n������ ���ӵǾ����ϴ�. 'q' �� �Է��Ͻø� �������ᰡ �˴ϴ�.\n";




	//��Ʈ�� �Է¹޴´�. (argc , argv[] �� �̿��ؼ� ���� �˴ϴ�.)
	printf("������ ���� PORT �� �Է��ϼ���. :");
	fgets(port, sizeof(port), stdin);

	//socket ���̺귯�� �ʱ�ȭ ����Ȯ�� 

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");
	//hMutex �ڵ����
	if ((hMutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
		ErrorHandling("CreateMutex() error.");
	//socket ����
	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	//sever socket �ʱ�ȭ
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(port));

	//sever ip, port �Ҵ�
	if (bind(servSock, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	//listen �Լ� ȣ��
	if (listen(servSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");



	while (1)
	{
		clntAddrSize = sizeof(clntAddr);
		// Ŭ���̾�Ʈ�� ������ �����Ѵ�.
		if ((clntSock = accept(servSock, (SOCKADDR*)& clntAddr, &clntAddrSize)) == INVALID_SOCKET) {
			ErrorHandling("accept() error.");
		}
		LoadacclistFromFile();


		WaitForSingleObject(hMutex, INFINITE);
		{
			clntSocks[clntCnt++] = clntSock;

		}
		ReleaseMutex(hMutex);

		// ������ Ŭ���̾�Ʈ�� ������ �˸���.
		printf("[�˸�] ���ο� Ŭ���̾�Ʈ�� �����Ͽ����ϴ�. \n");

		// HandleClient �����带 ���۽�Ų��.
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, (void*)clntSock, 0, (unsigned*)& dwThreadID);

		// �����尡 ���������� �������� �ʴ� ���.
		if (hThread == 0) {
			ErrorHandling("_beginthreadex() error.");
		}
	}




	// closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();
	return 0;
}

// ��� : Ŭ���̾�Ʈ�� �ڵ鸵�Ѵ�.
// ���� : void *arg - ������ ����� ���� (clntSock)
unsigned WINAPI HandleClient(void* arg) {
	SOCKET clntSock = (SOCKET*)arg; //�Ű������ι��� Ŭ���̾�Ʈ ������ ����
	int strLen1, strLen2 = 0, i;
	char j[2];
	char msg[BUFSIZE];

	while ((strLen1 = recv(clntSock, j, 2, 0)) != 0)
	{
		if (strLen1 == 1 || strLen1 == 2) {
			break;
		}
		printf("�α��� ����� \n");
	}

	if (strLen1 == 1) {
		while (1)
		{
			if (RegistAccstomer(clntSock))
			{
				char approval[1] = { 1 };
				//for (i = 0; i < 9; i++)
				send(clntSock, approval, sizeof(approval), 0);
				break;
			}
		}
	}
	if (strLen1 == 2) {
		while (1)
		{
			if (LoginAccstomer(clntSock))
			{
				char approval[1] = { 1 };
				//for (i = 0; i < 9; i++)
				send(clntSock, approval, sizeof(approval), 0);
				break;
			}
		}
	}

	while ((strLen2 = recv(clntSock, msg, sizeof(msg), 0)) != 0)
	{
		if (!strcmp(msg, "q")) {
			send(clntSock, "q", 1, 0);
			break;
		}
		SendMsg(msg, strLen2);
	}

	printf("�������� �������ϴ�.\n");
	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clntCnt; i++)
	{
		if (clntSock == clntSocks[i])
		{
			while (i++ < clntCnt - 1)
				clntSocks[i] = clntSocks[i + 1];
			break;
		}
	}
}


void SendMsg(char* msg, int len) { //�޽����� ��� Ŭ���̾�Ʈ���� ������.
	int i;
	WaitForSingleObject(hMutex, INFINITE);//���ؽ� ����
	for (i = 0; i < clntCnt; i++)//Ŭ���̾�Ʈ ������ŭ
		send(clntSocks[i], msg, len, 0);//Ŭ���̾�Ʈ�鿡�� �޽����� �����Ѵ�.
	ReleaseMutex(hMutex);//���ؽ� ����
}

void ErrorHandling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

void ErrorHandling(char* message);
unsigned int WINAPI Sender(void* arg);
unsigned int WINAPI Receiver(void* arg);
int Regist(SOCKET hServSock);

#define BUFSIZE		1024	// �޽��� ���� ������
#define NAMESIZE	30		// ��ȭ��
char msg[BUFSIZE];
char name[NAMESIZE] = "[DEFAULT]";

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	HANDLE hThread1, hThread2;		// ������ �ڵ�
	DWORD dwThreadID1, dwThreadID2; // ������ ID

	int strLen;
	int num;
	char port[8];
	char ipaddr[16];
	char i[2];
	char yes[2] = { 49, NULL };
	char no[3] = { 50, 50 , NULL };


	printf("������ ������ IP �� �Է��ϼ���. :");
	fgets(ipaddr, sizeof(ipaddr), stdin);
	printf("������ ������ PORT �� �Է��ϼ���. :");
	fgets(port, sizeof(port), stdin);


	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");



	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ipaddr);
	//	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(atoi(port));

	if (connect(hServSock, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("conncet() error");

	while (1) {

		printf("���������� 1 , �α����� 22�� �Է��ϼ���.\n");
		scanf("%s", i);
		send(hServSock, i, strlen(i), 0);
		getchar();
		if (i[1] == yes[1]) {
			Regist(hServSock);
			break;
		}
		else if (!(strcmp(no, i))) {
			Login(hServSock);
			break;
		}
		else
		{
			printf("�߸��� ���� �Է��߽��ϴ�.\n");
		}
	}


	hThread1 = (HANDLE)_beginthreadex(NULL, 0, Sender, (void*)hServSock, 0, (unsigned*)& dwThreadID1);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, Receiver, (void*)hServSock, 0, (unsigned*)& dwThreadID2);

	if (hThread1 == 0 || hThread2 == 0) {
		ErrorHandling("_beginthreadex() error.");
	}

	// Sender, Receiver �����尡 �۾��� ������ ������ ����Ѵ�.
	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);


	closesocket(hServSock);
	WSACleanup();

	return 0;
}

unsigned int WINAPI Sender(void* arg)
{
	SOCKET hServSock = (SOCKET)arg;
	char nameMsg[NAMESIZE + BUFSIZE];
	while (1) {//�ݺ�
		fgets(msg, BUFSIZE, stdin);//�Է��� �޴´�.
		if (!strcmp(msg, "q\n")) {//q�� �Է��ϸ� �����Ѵ�.
			send(hServSock, "q", 1, 0);//nameMsg�� �������� �����Ѵ�.
		}
		sprintf(nameMsg, "%s %s", name, msg);//nameMsg�� �޽����� �����Ѵ�.
		send(hServSock, nameMsg, strlen(nameMsg), 0);//nameMsg�� �������� �����Ѵ�.
	}
	return 0;
}

// ��� : �����κ��� �ٸ� ������� �޽����� �����Ѵ�.
// ���� : void *arg - ������ ����� ���� (hServSock)
unsigned int WINAPI Receiver(void* arg)
{
	SOCKET hServSock = (SOCKET)arg;
	char nameMsg[NAMESIZE + BUFSIZE];
	int strLen;
	while (1) {//�ݺ�
		strLen = recv(hServSock, nameMsg, NAMESIZE + BUFSIZE - 1, 0);//�����κ��� �޽����� �����Ѵ�.
		if (strLen == -1)
			return -1;
		nameMsg[strLen] = 0;//���ڿ��� ���� �˸��� ���� ����
		if (!strcmp(nameMsg, "q")) {
			printf("left the chat\n");
			closesocket(hServSock);
			exit(0);
		}
		fputs(nameMsg, stdout);//�ڽ��� �ֿܼ� ���� �޽����� ����Ѵ�.
	}
	return 0;
}

int Regist(SOCKET hServSock)
{
	char id[10];
	char password[30];
	char nickname[30];
	char approval[2];
	int test;
	int i;

	while (1) {

		printf("�������� ����� ���̵� �Է��ϼ���. : ");
		fgets(id, sizeof(id), stdin);
		id[strlen(id) - 1] = '\0';
		send(hServSock, id, strlen(id), 0);

		printf("�������� ����� ��й�ȣ�� �Է��ϼ���. : ");
		fgets(password, sizeof(password), stdin);
		password[strlen(password) - 1] = '\0';
		send(hServSock, password, strlen(password), 0);

		printf("�������� ����� �г����� �Է��ϼ���. : ");
		fgets(nickname, sizeof(nickname), stdin);
		nickname[strlen(nickname) - 1] = '\0';
		send(hServSock, nickname, strlen(nickname), 0);

		while (recv(hServSock, approval, sizeof(approval), 0) != 1);
		test = 1;

		if (test == 1)
			break;

	}
	sprintf(name, "[%s]", nickname);
	return 1;
}

int Login(SOCKET hServSock)
{
	char id[10];
	char password[30];
	char nickname[30];
	char approval[2];
	int test;


	while (1) {
		printf("���̵� �Է��ϼ���. : ");
		fgets(id, sizeof(id), stdin);
		id[strlen(id) - 1] = '\0';
		send(hServSock, id, strlen(id), 0);

		printf("��й�ȣ�� �Է��ϼ���. : ");
		fgets(password, sizeof(password), stdin);
		password[strlen(password) - 1] = '\0';
		send(hServSock, password, strlen(password), 0);

		recv(hServSock, nickname, sizeof(nickname), 0);

		while (recv(hServSock, approval, sizeof(approval), 0) != 1);
		test = 1;

		if (test == 1)
			break;

	}
	sprintf(name, "[%s]", nickname);
	return 1;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
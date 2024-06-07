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

#define BUFSIZE		1024	// 메시지 버퍼 사이즈
#define NAMESIZE	30		// 대화명
char msg[BUFSIZE];
char name[NAMESIZE] = "[DEFAULT]";

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	HANDLE hThread1, hThread2;		// 스레드 핸들
	DWORD dwThreadID1, dwThreadID2; // 스레드 ID

	int strLen;
	int num;
	char port[8];
	char ipaddr[16];
	char i[2];
	char yes[2] = { 49, NULL };
	char no[3] = { 50, 50 , NULL };


	printf("접속할 서버의 IP 를 입력하세요. :");
	fgets(ipaddr, sizeof(ipaddr), stdin);
	printf("접속할 서버의 PORT 를 입력하세요. :");
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

		printf("계정생성은 1 , 로그인은 22를 입력하세요.\n");
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
			printf("잘못된 값을 입력했습니다.\n");
		}
	}


	hThread1 = (HANDLE)_beginthreadex(NULL, 0, Sender, (void*)hServSock, 0, (unsigned*)& dwThreadID1);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, Receiver, (void*)hServSock, 0, (unsigned*)& dwThreadID2);

	if (hThread1 == 0 || hThread2 == 0) {
		ErrorHandling("_beginthreadex() error.");
	}

	// Sender, Receiver 스레드가 작업을 종료할 때까지 대기한다.
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
	while (1) {//반복
		fgets(msg, BUFSIZE, stdin);//입력을 받는다.
		if (!strcmp(msg, "q\n")) {//q를 입력하면 종료한다.
			send(hServSock, "q", 1, 0);//nameMsg를 서버에게 전송한다.
		}
		sprintf(nameMsg, "%s %s", name, msg);//nameMsg에 메시지를 전달한다.
		send(hServSock, nameMsg, strlen(nameMsg), 0);//nameMsg를 서버에게 전송한다.
	}
	return 0;
}

// 요약 : 서버로부터 다른 사용자의 메시지를 수신한다.
// 인자 : void *arg - 서버와 통신할 소켓 (hServSock)
unsigned int WINAPI Receiver(void* arg)
{
	SOCKET hServSock = (SOCKET)arg;
	char nameMsg[NAMESIZE + BUFSIZE];
	int strLen;
	while (1) {//반복
		strLen = recv(hServSock, nameMsg, NAMESIZE + BUFSIZE - 1, 0);//서버로부터 메시지를 수신한다.
		if (strLen == -1)
			return -1;
		nameMsg[strLen] = 0;//문자열의 끝을 알리기 위해 설정
		if (!strcmp(nameMsg, "q")) {
			printf("left the chat\n");
			closesocket(hServSock);
			exit(0);
		}
		fputs(nameMsg, stdout);//자신의 콘솔에 받은 메시지를 출력한다.
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

		printf("서버에서 사용할 아이디를 입력하세요. : ");
		fgets(id, sizeof(id), stdin);
		id[strlen(id) - 1] = '\0';
		send(hServSock, id, strlen(id), 0);

		printf("서버에서 사용할 비밀번호을 입력하세요. : ");
		fgets(password, sizeof(password), stdin);
		password[strlen(password) - 1] = '\0';
		send(hServSock, password, strlen(password), 0);

		printf("서버에서 사용할 닉네임을 입력하세요. : ");
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
		printf("아이디를 입력하세요. : ");
		fgets(id, sizeof(id), stdin);
		id[strlen(id) - 1] = '\0';
		send(hServSock, id, strlen(id), 0);

		printf("비밀번호을 입력하세요. : ");
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
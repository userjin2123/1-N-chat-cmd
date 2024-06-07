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


#define BUFSIZE 1024				//채팅 버퍼
#define QUITREQ		-1



int main(int argc, char* argv[])
{
	//SOCKET 선언
	WSADATA wsaData;
	SOCKET servSock, clntSock;
	SOCKADDR_IN servAddr, clntAddr;


	//메시지 받는 문자열
	char port[8];					//포트 배열
	int clntAddrSize;					//클라이언트 주소 사이즈

	HANDLE hThread;
	HANDLE hThread2;// 스레드 핸들
	DWORD dwThreadID;
	DWORD dwThreadID2;	// 스레드 ID


	char welcome[1024] = "\n서버에 접속되었습니다. 'q' 만 입력하시면 접속종료가 됩니다.\n";




	//포트를 입력받는다. (argc , argv[] 로 이용해서 만들어도 됩니다.)
	printf("서버의 시작 PORT 를 입력하세요. :");
	fgets(port, sizeof(port), stdin);

	//socket 라이브러리 초기화 버전확인 

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");
	//hMutex 핸들생성
	if ((hMutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
		ErrorHandling("CreateMutex() error.");
	//socket 생성
	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	//sever socket 초기화
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(port));

	//sever ip, port 할당
	if (bind(servSock, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	//listen 함수 호출
	if (listen(servSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");



	while (1)
	{
		clntAddrSize = sizeof(clntAddr);
		// 클라이언트의 연결을 수락한다.
		if ((clntSock = accept(servSock, (SOCKADDR*)& clntAddr, &clntAddrSize)) == INVALID_SOCKET) {
			ErrorHandling("accept() error.");
		}
		LoadacclistFromFile();


		WaitForSingleObject(hMutex, INFINITE);
		{
			clntSocks[clntCnt++] = clntSock;

		}
		ReleaseMutex(hMutex);

		// 서버에 클라이언트의 접속을 알린다.
		printf("[알림] 새로운 클라이언트가 접속하였습니다. \n");

		// HandleClient 스레드를 동작시킨다.
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, (void*)clntSock, 0, (unsigned*)& dwThreadID);

		// 스레드가 정상적으로 생성되지 않는 경우.
		if (hThread == 0) {
			ErrorHandling("_beginthreadex() error.");
		}
	}




	// closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();
	return 0;
}

// 요약 : 클라이언트를 핸들링한다.
// 인자 : void *arg - 서버와 통신할 소켓 (clntSock)
unsigned WINAPI HandleClient(void* arg) {
	SOCKET clntSock = (SOCKET*)arg; //매개변수로받은 클라이언트 소켓을 전달
	int strLen1, strLen2 = 0, i;
	char j[2];
	char msg[BUFSIZE];

	while ((strLen1 = recv(clntSock, j, 2, 0)) != 0)
	{
		if (strLen1 == 1 || strLen1 == 2) {
			break;
		}
		printf("로그인 대기중 \n");
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

	printf("서버에서 나갔습니다.\n");
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


void SendMsg(char* msg, int len) { //메시지를 모든 클라이언트에게 보낸다.
	int i;
	WaitForSingleObject(hMutex, INFINITE);//뮤텍스 실행
	for (i = 0; i < clntCnt; i++)//클라이언트 개수만큼
		send(clntSocks[i], msg, len, 0);//클라이언트들에게 메시지를 전달한다.
	ReleaseMutex(hMutex);//뮤텍스 중지
}

void ErrorHandling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
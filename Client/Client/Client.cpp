#include <iostream>
#include <map>
#include <WinSock2.h>// 윈도우 C++환경에서 소켓 프로그래밍을 하기 위해 헤더 포함
#include <windows.h>// Win32 API 함수를 사용하기 위해
#include <thread>
#include <charconv>
#include <WS2tcpip.h>

#include "flatbuffers/flatbuffers.h"
#include "C2S_CHATECHO_REQ_generated.h"
#include "C2S_PID_REQ_generated.h"
#include "C2S_ROOM_ENTER_REQ_generated.h"
#include "S2C_CHATECHO_ACK_generated.h"
#include "S2C_CHATECHO_NTY_generated.h"
#include "S2C_PID_ACK_generated.h"
#include "S2C_ROOM_ENTER_ACK_generated.h"
#include "S2C_ROOM_ENTER_NTY_generated.h"

#pragma comment(lib, "ws2_32.lib")//#include <WinSock2.h> 사용하기 위한  ws2_32.lib 추가, 이게 있어야 윈도우에 소켓을 사용 가능
using namespace std;

void MenuSelection();
void ChattingEcho();
void ChattingRoom();
void RecvCharEcho(char* buffer);
void RecvCharRoom(char* buffer);
void RecvCharPid(char* buffer);
void RecvCharValidRoomNo(char* buffer);
void Char_Recv();
void CommonFunc();

static CHAR IP[] = "127.0.0.1";
static CHAR Port[] = "3587";
#define BUF_SIZE 1024

SOCKET hSocket; // 소켓 생성하는 함수
bool threadStop = true;
bool mainLoop = true;
bool mainFinish = true;

int pid;

int MenuNum = 0;

enum Code
{
	NO_CHOICE,
	CHAT_ECHO,
	CHAT_ROOM,
	PID,
	VALID_ROOM_NO
};

map<int, void(*)(char*)> callbackMap =
{
	{1, RecvCharEcho},
	{2, RecvCharRoom},
	{3, RecvCharPid},
	{4, RecvCharValidRoomNo}
};

INT main(int argc, char* argv[])
{
	CommonFunc();
	threadStop = true;
	thread th1(Char_Recv); // 함수를 이용해 스레드 객체 생성
	while (mainFinish)
	{
		while (mainLoop)
		{
			MenuSelection();
			cin >> MenuNum;
			cin.ignore();// 입력 버퍼 초기화

			if (MenuNum == 1) // 채팅 에코
			{
				ChattingEcho();
			}
			else if (MenuNum == 2) // 채팅룸 입장
			{
				mainLoop = false;
				ChattingRoom();
			}
			else if (MenuNum == 0) // 프로그램종료
			{
				mainLoop = false;
				mainFinish = false;
			}
			else
			{
				cout << "다시 입력해 주세요!" << endl;
			}
		}
	}
	threadStop = false;
	cout << "[ACK] 클라이언트 종료" << endl;
	closesocket(hSocket);// 해당 소켓 닫아준다
	WSACleanup();
	th1.join(); // 해당 쓰레드가 실행을 종료하면 리턴
	return 0;
}

void MenuSelection()
{
	cout << "메뉴를 선택해 주세요!" << endl;
	cout << "1. 채팅 에코 메시지 전송" << endl;
	cout << "2. 채팅 룸 입장" << endl;
	cout << "0. 프로그램 종료" << endl;
}

void ChattingEcho()
{
	flatbuffers::FlatBufferBuilder builder;
	threadStop = true;
	cout << "\n채팅 에코 프로그램  " << endl;
	cout << "\n문자를 입력해주세요: (-1 입력 시 종료) ";
	while (1)
	{
		char input[BUF_SIZE] = { 0, };
		memset(input, 0, BUF_SIZE);
		cin.getline(input, BUF_SIZE, '\n');
		cin.clear();// 입력 버퍼 초기화

		if (input[0] == '-' && input[1] == '1')    //종료문자 처리
		{
			cout << "클라이언트 종료!" << endl;
			threadStop = false;
			break;
		}
		if (input != "")
			cout << "[ACK] 문자열 입력 성공!" << endl;
		int sendReturn;

		builder.Finish(CreateC2S_CHATECHO_REQ(builder, strlen(input) + 16, MenuNum, pid, strlen(input), builder.CreateString(input)));
		char tmpBuffer[BUF_SIZE] = { 0, };
		memcpy(&tmpBuffer, builder.GetBufferPointer(), builder.GetSize());
		if (send(hSocket, tmpBuffer, builder.GetSize(), 0))// 입력 받은 문자를 서버에 보냄
		{
			sendReturn = 1;
			cout << "문자열 전송 성공" << endl;
		}
		else
		{
			sendReturn = 0;
			cout << "문자열 전송 실패" << endl;
		}
		builder.Clear();
		builder.Finish(CreateS2C_CHATECHO_ACK(builder, strlen(input) + 16, MenuNum, sendReturn, strlen(input), builder.CreateString(input)));
		auto s2cEchoAck = GetS2C_CHATECHO_ACK(builder.GetBufferPointer());
		cout << "[ACK] [send] " << "총 버퍼 사이즈: \"" << s2cEchoAck->size() << "\" Code(채팅 에코:1, 채팅 룸:2, 방 번호 유무:4): \"" << s2cEchoAck->code() << "\" Result(문자열 전송 성공: 1, 문자열 전송 실패: 0): \"" << s2cEchoAck->result() << "\" , 문자열 길이: \"" << s2cEchoAck->stringLength() << "\" , 문자열: \"" << s2cEchoAck->msg()->c_str() << "\" from " << pid << " 번째 client " << endl;
	}
	cout << "\n채팅 에코 프로그램  종료" << endl;
	builder.Clear();
}

void ChattingRoom()
{
	flatbuffers::FlatBufferBuilder builder;
	threadStop = true;
	cout << "\n채팅 룸 프로그램  " << endl;
	cout << "\n입장할 채팅 방 번호를 입력 하세요!: (입력 안할시 default: -1, -2 입력시 종료) ";

	char input[BUF_SIZE] = { 0, };
	memset(input, 0, BUF_SIZE);
	cin.getline(input, BUF_SIZE, '\n');
	cin.clear();// 입력 버퍼 초기화

	int roomNo;
	if (strlen(input) == 0 || atoi(input) == -1)
		roomNo = 0; // 방 번호 입력 안할시 -1번 방 선택
	else
		roomNo = atoi(input); // 1번방 또는 2번 방

	if (roomNo == -2)    //종료문자 처리
	{
		threadStop = false;
		cout << "\n채팅 룸 프로그램  종료" << endl;
		mainLoop = true;
	}
	else // 방 번호 서버에서 확인
	{
		builder.Finish(CreateC2S_ROOM_ENTER_REQ(builder, 16, 4, roomNo, pid));
		char tmpBuffer[BUF_SIZE] = { 0, };
		memcpy(&tmpBuffer, builder.GetBufferPointer(), builder.GetSize());
		send(hSocket, tmpBuffer, builder.GetSize(), 0);
	}
	builder.Clear();
}

void RecvCharEcho(char* buffer)
{
	flatbuffers::FlatBufferBuilder builder;
	auto s2cEchoNty = GetS2C_CHATECHO_NTY(buffer);
	builder.Finish(CreateS2C_CHATECHO_NTY(builder, s2cEchoNty->size() , s2cEchoNty->code(), s2cEchoNty->userIdx(), s2cEchoNty->stringLength(), builder.CreateString(s2cEchoNty->msg()->c_str())));

	cout << "\n[recv] msg received. 총 버퍼 사이즈 : \"" << s2cEchoNty->size() << "\", Code(채팅 에코:1, 채팅 룸:2, 방 번호 유무:4): \"" << s2cEchoNty->code() << "\", 문자열 길이: \"" << s2cEchoNty->stringLength() << "\" , 문자열: \"" << s2cEchoNty->msg()->c_str() << "\" from server \"" << s2cEchoNty->userIdx() << "\" 번째 Client" << endl;// 받은 숫자를 콘솔 창에 출력
	if (MenuNum == 1)
		cout << "\n문자를 입력해주세요: (-1 입력 시 종료) ";
	else if (MenuNum != 2)
		MenuSelection();
	builder.Clear();
}

void RecvCharRoom(char* buffer)
{
	flatbuffers::FlatBufferBuilder builder;
	auto s2cRoomNty = GetS2C_ROOM_ENTER_NTY(buffer);
	builder.Finish(CreateS2C_ROOM_ENTER_NTY(builder, s2cRoomNty->size(), s2cRoomNty->code(), s2cRoomNty->roomNo(), s2cRoomNty->userIdx() ));

	int roomNo = s2cRoomNty->roomNo() == 0 ?  -1 : s2cRoomNty->roomNo();
	cout << "\n[recv] msg received. 총 버퍼 사이즈 : \"" << s2cRoomNty->size() << "\", Code(채팅 에코:1, 채팅 룸:2, 방 번호 유무:4): \"" << s2cRoomNty->code() << "\", Room No: \"" << roomNo << "\" from server \"" << s2cRoomNty->userIdx() << "\" 번째 Client" << endl;// 받은 숫자를 콘솔 창에 출력
	mainLoop = true;

	if (MenuNum == 1)
		cout << "\n문자를 입력해주세요: (-1 입력 시 종료) ";
	else if (MenuNum != 2)
		MenuSelection();
	builder.Clear();
}

void RecvCharPid(char* buffer)
{
	auto s2cPidAck = GetS2C_PID_ACK(buffer);
	cout << "\n유저 번호: " << s2cPidAck->pid() << endl;
	pid = s2cPidAck->pid();
}

void RecvCharValidRoomNo(char* buffer)
{
	flatbuffers::FlatBufferBuilder builder;
	auto s2cRoomNty = GetS2C_ROOM_ENTER_NTY(buffer);

	if (s2cRoomNty->roomNo() == -100)
	{
		cout << "[ACK] 없는 방 입니다. 방을 다시 입력 하세요" << endl;
		mainLoop = true;
	}
	else
	{
		builder.Finish(CreateS2C_ROOM_ENTER_NTY(builder, s2cRoomNty->size(), 2, s2cRoomNty->roomNo(), s2cRoomNty->userIdx())); // 코드 변경
		s2cRoomNty = GetS2C_ROOM_ENTER_NTY(builder.GetBufferPointer());
		int tmpRoomNo = s2cRoomNty->roomNo() == 0 ? -1 : s2cRoomNty->roomNo();
		int sendResult;
		cout << "[ACK] " << tmpRoomNo << "번방 선택!" << endl;

		builder.Clear();
		builder.Finish(CreateC2S_ROOM_ENTER_REQ(builder, builder.GetSize(), s2cRoomNty->code(), s2cRoomNty->roomNo(), s2cRoomNty->userIdx())); // 코드 변경
		char tmpBuffer[BUF_SIZE] = { 0, };
		memcpy(&tmpBuffer, builder.GetBufferPointer(), builder.GetSize());
		if (send(hSocket, tmpBuffer, builder.GetSize(), 0))// 입력 받은 문자를 서버에 보냄
			sendResult = 1;
		else // 서버에 보내는 것을 실패 했을 때
			sendResult = 0;

		builder.Clear();
		builder.Finish(CreateS2C_ROOM_ENTER_ACK(builder, builder.GetSize(), s2cRoomNty->code(), s2cRoomNty->roomNo(), sendResult));
		auto s2cRoomAck = GetS2C_ROOM_ENTER_ACK(builder.GetBufferPointer());
		cout << "[ACK] [send] \"" << "총 버퍼 사이즈: \"" << s2cRoomAck->size() << "\" , Code(채팅 에코:1, 채팅 룸:2, 방 번호 유무:4): \"" << s2cRoomAck->code() << "\" , room No: \"" << tmpRoomNo << "\" Result(채팅 방 입장 성공: 1, 채팅 방 입장 실패: 0): \"" << s2cRoomAck->result() << "\" from " << pid << "번째 client" << endl;
	}
	builder.Clear();
}

void Char_Recv()//클라이언트에서 문자열 입력을 받는 도중에 서버에서 문자열을 받으면 스레드를 이용해서 밑에 함수 실행함
{
	while (threadStop) // 소켓이 돌고 있으므로 소켓 먼저 종료 시키고 스레드 종료 시키면 정상 종료
	{
		char tmpBuffer[BUF_SIZE] = { 0, };
		if (recv(hSocket, tmpBuffer, BUF_SIZE, 0)) //recv(소켓, 수신 정보를 담을 배열주소, 그 배열의 크기, flag)
		{ //  flags의 값이 0이면 일반 데이터를 수신
		  //대상 소켓으로부터 보내온 정보를 받아주는 역할
		  //보내준 데이터가 없다면 여기에서 받을 때까지 계속 대기 상태에 있음
			auto s2cPidAck = GetS2C_PID_ACK(tmpBuffer);
			switch (s2cPidAck->code())
			{
			case Code::NO_CHOICE:
				break;
			case Code::CHAT_ECHO:
				callbackMap[CHAT_ECHO](tmpBuffer);
				break;
			case Code::CHAT_ROOM:
				callbackMap[CHAT_ROOM](tmpBuffer);
				break;
			case Code::PID:
				callbackMap[PID](tmpBuffer);
				break;
			case Code::VALID_ROOM_NO:
				callbackMap[VALID_ROOM_NO](tmpBuffer);
				break;
			}
		}
		memset(tmpBuffer, 0, BUF_SIZE);
	}
}

void CommonFunc()
{
	WSADATA wsaData;//WSADATA 구조체 변수 객체 선언
	SOCKADDR_IN serverAddr;// SOCKADDR_IN 구조체는 IPv4 주소를 저장하는 구조체

	//WS2_32.dll을 불러와 메모리에 적재하는 함수
	// WSAStartup함수는 wsaData를 초기화 하는 함수
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{//MAKEWORD: 윈도우 소켓 2.2버젼을 사용
		cout << "WSAStartup() 함수 에러" << endl;// 문자열 출력과 함께 개행 출력
		exit(1);// WSAStartup이라는 함수는 성공시 0, 실패 시 SOCKET_ERROR를 리턴
	}

	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	//AF_INET -> IPv4 socket, AF_INET6 -> IPv6 socket

	//서버 소켓 만듬  TCP 프로토콜의 소켓
	//서버 소켓: 클라이언트에서 접속을 요청할 경우 받아줄 소켓이 필요 그 소켓이 서버 소켓
	//PF_INET 프로토콜 체계
	// SOCK_STREAM: 연결 지향형 TCP 소켓 (소켓과 소켓이 계속 연결되어 있는 상태 유지, 연결지향형 소켓이란 소켓과 소켓의 연결이 1 vs 1)
	if (hSocket == INVALID_SOCKET)// 소켓 생성에 실패할 경우 INVALID_SOCKET 반환
	{
		cout << "소켓 핸들 에러" << endl;
		exit(1);
	}

	ZeroMemory(&serverAddr, sizeof(serverAddr));//메모리를 0으로 채움
	serverAddr.sin_family = AF_INET;//sockaddr_in 구조체 객체 serverAddr에 변수 값 할당 / AF_INET 주소체계 표현
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	serverAddr.sin_port = htons(atoi(Port));
	serverAddr.sin_family = AF_INET;//sockaddr_in 구조체 객체 serverAddr에 변수 값 할당 / AF_INET 주소체계 표현
	InetPton(AF_INET, IP, &(serverAddr.sin_addr.S_un));

	if (connect(hSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{//connect(소켓, 소켓 구성요소 구조체의 주소, 그 구조체의 크기);
		cout << "접속이 끊겼음...." << endl;
		exit(1);
	}// 지정된 소켓에 연결을 설정 (서버에 연결 하기 위해)
	cout << "[connect] from client " << endl;
}
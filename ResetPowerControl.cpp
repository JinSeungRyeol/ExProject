#include "pch.h"
#include "ResetPowerControl.h"
#include "tlhelp32.h"				//=====프로세스 찾기 및 종료
#include <WinSock2.h>				//=====소켓 통신 헤더 및 라이브러리
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#define ACADD	6					//TDR 전원제어 AC 번호 추가
using namespace std;


//*****************************************************************************************
//									TDR 전원 제어
//*****************************************************************************************

void ResetPowerControl::TDRPowerResetFunc(CString strIP, CString strPW, CString strACNum)
{
	
	
	CString strObject;
	int nACNumber = _wtoi(strACNum) + ACADD;			//AC 번호 + 6 == 리셋 AC 번호

	strObject.Format(_T("TDRPowerControl.exe %s %s %d"), strIP, strPW, nACNumber);

	ExecProcess(strObject, nACNumber);
}


BOOL ResetPowerControl::ExecProcess(CString strObject, int nACNumber)
{
	if (m_Process.hProcess != INVALID_HANDLE_VALUE)
	{
		if (!KillProcess())
		{
			//m_ListOutput.AddString(_T("KillProcess Error : ")+dwError);
		}
		Sleep(100);
	}

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));

	si.cb = sizeof(STARTUPINFO);			//구조체 크기
	si.dwFlags = STARTF_USESHOWWINDOW;			//wShowWindow 기능 사용
	si.dwXSize = 0;							//창의 너비	
	si.dwX = 0;							//창의 x 픽셀		
	si.dwY = 0;							//창의 y 픽셀
	si.wShowWindow = SW_HIDE;						//nCmdShow 파라미터에 지정할수 있는 값 

	if (CreateProcessW(NULL, strObject.GetBuffer(0), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_Process) == FALSE)
	{
		/*CreateProcessW
		LPCSTR				  IpAppLicationName			= 실행할 모듈의 이름
		LPSTR				  IpCommandLine				= 실행할 명령 줄
		LPSECURITY_ATTRIBUTES lpProcessAttributes,		= 새 프로세스 개체에 반환된 핸들이 하위프로세스에 의해 상속될 수 있는지 여부
		LPSECURITY_ATTRIBUTES lpThreadAttributes,		= 스레드 객체에 반환된 핸들이 하위프로세스에 의해 상속될 수 있는지 여부
		BOOL                  bInheritHandles,			= TRUE : 상속 , FALSE : 상속 X
		DWORD                 dwCreationFlags,			= 우선 순위 클래스와 프로세스 생성을 제어하는 플래그 값, 기본 값 : NORMAL_PRIORITY_CLASS
		LPVOID                lpEnvironment,			= 새 프로세스의 환경블록에 대한 포인터, NULL : 호출 프로세스의 환경 사용
		LPCSTR                lpCurrentDirectory,		= 현재 디렉터리에 대한 전체 경로
		LPSTARTUPINFOA        lpStartupInfo,			= STARTUPINFO 구조체
		LPPROCESS_INFORMATION lpProcessInformation		= PROCESS_INFORMATION 구조체*/

		DWORD dwLastError(0);
		dwLastError = GetLastError();

		//m_ListOutput.AddString(_T("CreateProcess Error : ")+dwError);
		CloseHandle(m_Process.hProcess);
		CloseHandle(m_Process.hThread);

		m_Process.hProcess = INVALID_HANDLE_VALUE;
		m_Process.hThread = INVALID_HANDLE_VALUE;

		return FALSE;
	}

	switch (nACNumber)
	{
	case 7:
		Log.LogWrite("=====TDR Power AC1 RESET=====");
		break;
	case 8:
		Log.LogWrite("=====TDR Power AC2 RESET=====");
		break;
	case 9:
		Log.LogWrite("=====TDR Power AC3 RESET=====");
		break;
	case 10:
		Log.LogWrite("=====TDR Power AC4 RESET=====");
		break;
	case 11:
		Log.LogWrite("=====TDR Power AC5 RESET=====");
		break;
	case 12:
		Log.LogWrite("=====TDR Power AC6 RESET=====");
		break;
	case 13:
		Log.LogWrite("=====TDR Power AC7 RESET=====");
		break;
	case 14:
		Log.LogWrite("=====TDR Power AC8 RESET=====");
		break;
	}

	return TRUE;
}



BOOL ResetPowerControl::KillProcess()

{
	DWORD dwError(0);
	DWORD dwExitcode(0);

	if (m_Process.hProcess == INVALID_HANDLE_VALUE)
		return FALSE;


	//=======================================================================프로세스 종료상태 확인
	if (!GetExitCodeProcess((HANDLE)m_Process.hProcess, &dwExitcode))
	{
		dwError = GetLastError();
		//에러 코드 작성
		return FALSE;
	}
	else
	{	//===================================================================지정 프로세스와 모든 스레드 종료
		if (!TerminateProcess((HANDLE)m_Process.hProcess, dwExitcode))
		{
			dwError = GetLastError();
			//에러코드작성
			return FALSE;
		}
	}
	CloseHandle(m_Process.hProcess);
	CloseHandle(m_Process.hThread);
	m_Process.hProcess = INVALID_HANDLE_VALUE;
	m_Process.hThread = INVALID_HANDLE_VALUE;
	return TRUE;
}


//*******************************************************************************
//						TDR 실행 프로그램 종료
//*******************************************************************************
BOOL ResetPowerControl::KillProcessModule()
{
	//********************현재 모든 프로세스들의 상태를 스냅*************************//
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry32;

	CString strProcessName = _T("TDRPowerControl.exe");
	DWORD dwExitCode(0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		TRACE(_T("------------------------hProcessSnap == INVALID_HANDLE_VALUE\n"));
	//exit(EXIT_FAILURE);

	processEntry32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &processEntry32))
	{
		CloseHandle(hProcessSnap);
		TRACE(_T("------------------------hProcessSnap == process32First Failed.\n"));
		//exit(EXIT_FAILURE);
	}


	while (Process32Next(hProcessSnap, &processEntry32))
	{
		//*******************************실행중인 프로세스가 있다면 종료***************************//
		if (strProcessName.Compare(processEntry32.szExeFile) == 0)
		{
			HANDLE hKillProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry32.th32ProcessID);

			if (hKillProcess)
			{
				if (TerminateProcess(hKillProcess, dwExitCode))
				{
					TRACE(_T("-----------------------Kill TDRProgram-----------------------\n"));
					GetExitCodeProcess(hKillProcess, &dwExitCode);
				}
			}
		}
	}
	CloseHandle(hProcessSnap);
	return FALSE;
}

//*******************************************************************************
//							RB 전원제어기 리셋 함수 호출
//*******************************************************************************
void ResetPowerControl::RBPowerResetFunc(CString strIP, CString strPORT, CString strACNum)
{
	
	int nPort = _wtoi(strPORT);
	int nAcNum = _wtoi(strACNum);
	char pszRBIP[DATASIZE] = { 0, };


	int nLength = WideCharToMultiByte(CP_ACP, 0, strIP, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, strIP, -1, pszRBIP, nLength, NULL, NULL);

	//int nAcNumber = _wtoi(m_acNumber);
	//int nPort		= _wtoi(m_Port);
	//char* pszRBIP = new char[DATASIZE];

	//int nLength = WideCharToMultiByte(CP_ACP, 0, m_PowerIP, -1, NULL, 0, NULL, NULL);
	//WideCharToMultiByte(CP_ACP, 0, m_PowerIP, -1, pszRBIP, nLength, NULL, NULL);


	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_Socket == INVALID_SOCKET)
	{
		TRACE(_T("===============SOCKET ERROR %d\n"), WSAGetLastError());
		return;
	}

	SOCKADDR_IN SocketAddr = {};
	SocketAddr.sin_family = AF_INET;
	SocketAddr.sin_port = htons(nPort);
	inet_pton(AF_INET,pszRBIP,&SocketAddr.sin_addr);

	//연결 시간 매우느림 . 리셋 중 다시 시작하려할 때 늦게 풀리는 이유.
	//수정 요망

	DWORD dwTick = GetTickCount();

	if (connect(m_Socket, (SOCKADDR*)&SocketAddr, sizeof(SocketAddr)) == SOCKET_ERROR)
	{
		TRACE(_T("==============Connect Error %d\n"), WSAGetLastError());
		shutdown(m_Socket, SD_BOTH);
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return;
	}
	TRACE(_T("Connect Complete : %02d ms\n"), GetTickCount() - dwTick);


	switch (nAcNum)
	{
	case 1:
		ResetSendDataPC1();
		break;
	case 2:
		ResetSendDataPC2();
		break;
	case 3:
		ResetSendDataCAM1();
		break;
	case 4:
		ResetSendDataCAM2();
		break;
	case 5:
		ResetSendDataNETWORK();
		break;
	case 6:
		ResetSendDataLOOP1();
		break;
	case 7:
		ResetSendDataLOOP2();
		break;
	}

	shutdown(m_Socket, SD_BOTH);
	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
	return;
}


//*******************************************************************************
//						RB 전원제어기 리셋 데이터 전송
//*******************************************************************************
void ResetPowerControl::ResetSendDataPC1()
{
	BYTE SocketSendData[3] = { 0, };

	SocketSendData[0] = 1;						//PC1 리셋
	SocketSendData[1] = 0x03;					//리셋
	SocketSendData[2] = RESETTIME_PC1;			//PC1 리셋시간

	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
	{
		TRACE(_T("---------------SocketSend Error PC1 : %d \n"), WSAGetLastError());
		return;
	}
	Log.LogWrite("=====RB Power PC1 RESET=====");
}

void ResetPowerControl::ResetSendDataPC2()
{
	BYTE SocketSendData[3] = { 0, };

	SocketSendData[0] = 2;						//PC2 리셋
	SocketSendData[1] = 0x03;					//리셋
	SocketSendData[2] = RESETTIME_PC2;			//PC2 리셋시간

	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
	{
		TRACE(_T("---------------SocketSend Error PC2 : %d \n"), WSAGetLastError());
		return;
	}
	Log.LogWrite("=====RB Power PC2 RESET=====");
}

void ResetPowerControl::ResetSendDataCAM1()
{
	BYTE SocketSendData[3] = { 0, };

	SocketSendData[0] = 3;						//CAM1 리셋
	SocketSendData[1] = 0x03;					//리셋
	SocketSendData[2] = RESETTIME_CAM1;			//CAM1 리셋시간

	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
	{
		TRACE(_T("---------------SocketSend Error CAM1 : %d \n"), WSAGetLastError());
		return;
	}
	Log.LogWrite("=====RB Power CAM1 RESET=====");
}

void ResetPowerControl::ResetSendDataCAM2()
{
	BYTE SocketSendData[3] = { 0, };

	SocketSendData[0] = 4;						//CAM2 리셋
	SocketSendData[1] = 0x03;					//리셋
	SocketSendData[2] = RESETTIME_CAM2;			//CAM2 리셋시간

	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
	{
		TRACE(_T("---------------SocketSend Error CAM2 : %d \n"), WSAGetLastError());
		return;
	}
	Log.LogWrite("=====RB Power CAM2 RESET=====");
}

void ResetPowerControl::ResetSendDataNETWORK()
{
	BYTE SocketSendData[3] = { 0, };

	SocketSendData[0] = 5;						//NETWORK 리셋
	SocketSendData[1] = 0x03;					//리셋
	SocketSendData[2] = RESETTIME_NETWORK;		//NETWORK 리셋시간

	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
	{
		TRACE(_T("---------------SocketSend Error NETWORK : %d \n"), WSAGetLastError());
		return;
	}
	Log.LogWrite("=====RB Power NEWWORK RESET=====");
}

void ResetPowerControl::ResetSendDataLOOP1()
{
	BYTE SocketSendData[3] = { 0, };

	SocketSendData[0] = 6;						//LOOP1 리셋
	SocketSendData[1] = 0x03;					//리셋
	SocketSendData[2] = RESETTIME_LOOP1;		//LOOP1 리셋시간

	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
	{
		TRACE(_T("---------------SocketSend Error LOOP1 : %d \n"), WSAGetLastError());
		return;
	}
	Log.LogWrite("=====RB Power LOOP1 RESET=====");
}

void ResetPowerControl::ResetSendDataLOOP2()
{
	BYTE SocketSendData[3] = { 0, };

	SocketSendData[0] = 7;						//LOOP2 리셋
	SocketSendData[1] = 0x03;					//리셋
	SocketSendData[2] = RESETTIME_LOOP2;		//LOOP2 리셋시간

	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
	{
		TRACE(_T("---------------SocketSend Error LOOP2 : %d \n"), WSAGetLastError());
		return;
	}
	Log.LogWrite("=====RB Power LOOP2 RESET=====");
}


//*******************************************************************************
//						RB전원제어기 리셋 요청 응답
//*******************************************************************************
int ResetPowerControl::SocketSend(BYTE btCode, BYTE* btData, int nLength)
{
	if (m_Socket == INVALID_SOCKET)
	{
		TRACE(_T("Send Fail \n"));
		return 0;
	}

	m_SendBuf[0] = DLE;																	//DLE
	m_SendBuf[1] = STX;																	//STX
	m_SendBuf[2] = btCode;																//OPCODE
	m_SendBuf[3] = (BYTE)nLength;														//LENGTH
	memcpy(m_SendBuf + HEADER_SIZE + IDENTIFIER_SIZE, btData, nLength);					//DATA
	m_SendBuf[HEADER_SIZE + IDENTIFIER_SIZE + nLength] = DLE;							//DLE
	m_SendBuf[HEADER_SIZE + IDENTIFIER_SIZE + nLength + 1] = ETX;						//ETX
	UINT nCrc = CRC16((char*)m_SendBuf + HEADER_SIZE, IDENTIFIER_SIZE + nLength);
	m_SendBuf[HEADER_SIZE + IDENTIFIER_SIZE + nLength + TAIL_SIZE] = HIBYTE(nCrc);		//CRC
	m_SendBuf[HEADER_SIZE + IDENTIFIER_SIZE + nLength + TAIL_SIZE + 1] = LOBYTE(nCrc);	//CRC

	int nRet = SOCKET_ERROR;

	nRet = send(m_Socket, (char*)m_SendBuf, HEADER_SIZE + IDENTIFIER_SIZE + nLength + TAIL_SIZE + CRC_SIZE, 0);
	return nRet;
}

//****************************************************************************************
//						CRC 에러체크 : OPCODE, LENGTH, DATA 계산
//****************************************************************************************
UINT ResetPowerControl::CRC16(char* rData, UINT len)
{
	UINT	wCrc = 0xFFFF;
	UINT	wCh;

	for (UINT i = 0; i < len; i++)
	{
		wCh = (UINT) * (rData + i);

		for (UINT j = 0; j < 8; j++)
		{
			if ((wCh ^ wCrc) & 0x0001)
				wCrc = (wCrc >> 1) ^ 0xA001;
			else
				wCrc >>= 1;

			wCh >>= 1;
		}
	}

	return wCrc;
}
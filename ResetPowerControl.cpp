#include "pch.h"
#include "ResetPowerControl.h"
#include "tlhelp32.h"				//=====���μ��� ã�� �� ����
#include <WinSock2.h>				//=====���� ��� ��� �� ���̺귯��
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#define ACADD	6					//TDR �������� AC ��ȣ �߰�
using namespace std;


//*****************************************************************************************
//									TDR ���� ����
//*****************************************************************************************

void ResetPowerControl::TDRPowerResetFunc(CString strIP, CString strPW, CString strACNum)
{
	
	
	CString strObject;
	int nACNumber = _wtoi(strACNum) + ACADD;			//AC ��ȣ + 6 == ���� AC ��ȣ

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

	si.cb = sizeof(STARTUPINFO);			//����ü ũ��
	si.dwFlags = STARTF_USESHOWWINDOW;			//wShowWindow ��� ���
	si.dwXSize = 0;							//â�� �ʺ�	
	si.dwX = 0;							//â�� x �ȼ�		
	si.dwY = 0;							//â�� y �ȼ�
	si.wShowWindow = SW_HIDE;						//nCmdShow �Ķ���Ϳ� �����Ҽ� �ִ� �� 

	if (CreateProcessW(NULL, strObject.GetBuffer(0), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_Process) == FALSE)
	{
		/*CreateProcessW
		LPCSTR				  IpAppLicationName			= ������ ����� �̸�
		LPSTR				  IpCommandLine				= ������ ��� ��
		LPSECURITY_ATTRIBUTES lpProcessAttributes,		= �� ���μ��� ��ü�� ��ȯ�� �ڵ��� �������μ����� ���� ��ӵ� �� �ִ��� ����
		LPSECURITY_ATTRIBUTES lpThreadAttributes,		= ������ ��ü�� ��ȯ�� �ڵ��� �������μ����� ���� ��ӵ� �� �ִ��� ����
		BOOL                  bInheritHandles,			= TRUE : ��� , FALSE : ��� X
		DWORD                 dwCreationFlags,			= �켱 ���� Ŭ������ ���μ��� ������ �����ϴ� �÷��� ��, �⺻ �� : NORMAL_PRIORITY_CLASS
		LPVOID                lpEnvironment,			= �� ���μ����� ȯ���Ͽ� ���� ������, NULL : ȣ�� ���μ����� ȯ�� ���
		LPCSTR                lpCurrentDirectory,		= ���� ���͸��� ���� ��ü ���
		LPSTARTUPINFOA        lpStartupInfo,			= STARTUPINFO ����ü
		LPPROCESS_INFORMATION lpProcessInformation		= PROCESS_INFORMATION ����ü*/

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


	//=======================================================================���μ��� ������� Ȯ��
	if (!GetExitCodeProcess((HANDLE)m_Process.hProcess, &dwExitcode))
	{
		dwError = GetLastError();
		//���� �ڵ� �ۼ�
		return FALSE;
	}
	else
	{	//===================================================================���� ���μ����� ��� ������ ����
		if (!TerminateProcess((HANDLE)m_Process.hProcess, dwExitcode))
		{
			dwError = GetLastError();
			//�����ڵ��ۼ�
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
//						TDR ���� ���α׷� ����
//*******************************************************************************
BOOL ResetPowerControl::KillProcessModule()
{
	//********************���� ��� ���μ������� ���¸� ����*************************//
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
		//*******************************�������� ���μ����� �ִٸ� ����***************************//
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
//							RB ��������� ���� �Լ� ȣ��
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

	//���� �ð� �ſ���� . ���� �� �ٽ� �����Ϸ��� �� �ʰ� Ǯ���� ����.
	//���� ���

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
//						RB ��������� ���� ������ ����
//*******************************************************************************
void ResetPowerControl::ResetSendDataPC1()
{
	BYTE SocketSendData[3] = { 0, };

	SocketSendData[0] = 1;						//PC1 ����
	SocketSendData[1] = 0x03;					//����
	SocketSendData[2] = RESETTIME_PC1;			//PC1 ���½ð�

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

	SocketSendData[0] = 2;						//PC2 ����
	SocketSendData[1] = 0x03;					//����
	SocketSendData[2] = RESETTIME_PC2;			//PC2 ���½ð�

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

	SocketSendData[0] = 3;						//CAM1 ����
	SocketSendData[1] = 0x03;					//����
	SocketSendData[2] = RESETTIME_CAM1;			//CAM1 ���½ð�

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

	SocketSendData[0] = 4;						//CAM2 ����
	SocketSendData[1] = 0x03;					//����
	SocketSendData[2] = RESETTIME_CAM2;			//CAM2 ���½ð�

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

	SocketSendData[0] = 5;						//NETWORK ����
	SocketSendData[1] = 0x03;					//����
	SocketSendData[2] = RESETTIME_NETWORK;		//NETWORK ���½ð�

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

	SocketSendData[0] = 6;						//LOOP1 ����
	SocketSendData[1] = 0x03;					//����
	SocketSendData[2] = RESETTIME_LOOP1;		//LOOP1 ���½ð�

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

	SocketSendData[0] = 7;						//LOOP2 ����
	SocketSendData[1] = 0x03;					//����
	SocketSendData[2] = RESETTIME_LOOP2;		//LOOP2 ���½ð�

	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
	{
		TRACE(_T("---------------SocketSend Error LOOP2 : %d \n"), WSAGetLastError());
		return;
	}
	Log.LogWrite("=====RB Power LOOP2 RESET=====");
}


//*******************************************************************************
//						RB��������� ���� ��û ����
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
//						CRC ����üũ : OPCODE, LENGTH, DATA ���
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
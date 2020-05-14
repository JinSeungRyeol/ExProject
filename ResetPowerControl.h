#pragma once
#include <afxdialogex.h>
//#include "PowerReset_ProgramDlg.h"
#include "Log.h"
#include "EZIni.h"

#define DATASIZE 32

class ResetPowerControl
{
public:
	ResetPowerControl() {
		m_acNumber = ""; m_Port = ""; m_Password = ""; m_IP = ""; memset(&m_Process, NULL, sizeof(PROCESS_INFORMATION));
		memset(&m_RecvBuf, 0, DATASIZE); memset(&m_SendBuf, 0, DATASIZE);m_Socket = INVALID_SOCKET;
	}

	//************************** SOCKET enum ***********************************************************************
	enum { STX = 0x02, DLE = 0x10, ETX = 0x03 };

	enum {
		HEADER_SIZE = 2
		, IDENTIFIER_SIZE = 2
		, TAIL_SIZE = 2
		, CRC_SIZE = 2
		, RECV_SIZE = 255
		, SEND_SIZE = 255
	};

	enum {
		RESETTIME_PC1 = 10
		, RESETTIME_PC2 = 10
		, RESETTIME_CAM1 = 5
		, RESETTIME_CAM2 = 5
		, RESETTIME_NETWORK = 5
		, RESETTIME_LOOP1 = 3
		, RESETTIME_LOOP2 = 3
	};
	//**************************************************************************************************************

	CString m_acNumber;
	CString m_Port;
	CString m_Password;
	CString m_IP;

	BOOL ExecProcess(CString, int);
	BOOL KillProcess();
	BOOL KillProcessModule();												//TDR 실행파일 종료

	void TDRPowerResetFunc(CString strIP, CString strPW, CString strACNum);												//TDR 전원제어기 리셋
	void RBPowerResetFunc(CString strIP, CString strPORT, CString strACNum);												//RB  전원제어기 리셋

	void ResetSendDataPC1();												//PC1 리셋 데이터
	void ResetSendDataPC2();												//PC2 리셋 데이터
	void ResetSendDataCAM1();												//CAM1 리셋 데이터
	void ResetSendDataCAM2();												//CAM2 리셋 데이터
	void ResetSendDataNETWORK();											//NETWORK 리셋 데이터
	void ResetSendDataLOOP1();												//LOOP1 리셋 데이터
	void ResetSendDataLOOP2();												//LOOP1 리셋 데이터

	int SocketSend(BYTE btCode, BYTE* btData, int nLength);										//소켓 전송

	UINT CRC16(char*, UINT);


	CLog  Log;


	PROCESS_INFORMATION m_Process;

private:
	

	SOCKET		m_Socket;
	BYTE		m_SendBuf[SEND_SIZE];
	BYTE		m_RecvBuf[RECV_SIZE];


};

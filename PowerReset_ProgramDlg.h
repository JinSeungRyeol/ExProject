
#pragma once
// PowerReset_ProgramDlg.h: 헤더 파일
//

#include "ResetPowerControl.h"
#include "EZIni.h"
#include "Log.h"
#define DATASIZE 32

typedef struct _OUTPUTDATA
{
	CString strAddr;
	CString strRoundTrip;
	CString DataSize;
	CString TTL;
}OUTPUTDATA;

extern EZIni g_Ini;			//INI 사용 클래스
extern ResetPowerControl m_Power;

// CPowerResetProgramDlg 대화 상자
class CPowerResetProgramDlg : public CDialogEx
{
// 생성입니다.
public:
	CPowerResetProgramDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	////************************** SOCKET enum ***********************************************************************
	//enum { STX = 0x02, DLE = 0x10, ETX = 0x03 };

	//enum { HEADER_SIZE		= 2
	//	, IDENTIFIER_SIZE	= 2
	//	, TAIL_SIZE			= 2
	//	, CRC_SIZE			= 2
	//	, RECV_SIZE			= 255
	//	, SEND_SIZE			= 255 };

	//enum { RESETTIME_PC1	= 10
	//	, RESETTIME_PC2		= 10
	//	, RESETTIME_CAM1	= 5
	//	, RESETTIME_CAM2	= 5
	//	, RESETTIME_NETWORK = 5
	//	, RESETTIME_LOOP1	= 3
	//	, RESETTIME_LOOP2	= 3 };
	////**************************************************************************************************************

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POWERRESET_PROGRAM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
private:
	CWinThread* m_pThread;
	BOOL		m_bThreadDo;

	
	//SOCKET		m_Socket;
	//BYTE		m_SendBuf[SEND_SIZE];
	//BYTE		m_RecvBuf[RECV_SIZE];
protected:

	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	

	static UINT StartThread(LPVOID);
	//UINT CRC16(char*, UINT);

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtResetTest();
	afx_msg LRESULT OnUserFunc(WPARAM wParam, LPARAM lParam);


	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CIPAddressCtrl m_Center_IP;
	CIPAddressCtrl m_TDR_IP;
	CString m_RB_Port;
	CString m_TDR_Password;
	CString m_AcNumber;
	CString m_PowerIP;
	CString m_TempCenter_IP;
	
	void GetControlValueSettingFunc();										//Control 값 변경시 INI 변경유지
	void SetIniDataFunc();													//INI 초기 값 설정
	void GetErrorOutputFunc();												//CMD 로그 출력
	//void TDRPowerResetFunc();												//TDR 전원제어기 리셋
	//void RBPowerResetFunc();												//RB  전원제어기 리셋
	//
	//void ResetSendDataPC1();												//PC1 리셋 데이터
	//void ResetSendDataPC2();												//PC2 리셋 데이터
	//void ResetSendDataCAM1();												//CAM1 리셋 데이터
	//void ResetSendDataCAM2();												//CAM2 리셋 데이터
	//void ResetSendDataNETWORK();											//NETWORK 리셋 데이터
	//void ResetSendDataLOOP1();												//LOOP1 리셋 데이터
	//void ResetSendDataLOOP2();												//LOOP1 리셋 데이터
	//
	//int SocketSend(BYTE, BYTE*, int);										//소켓 전송

	//BOOL ExecProcess(CString, int);											
	//BOOL KillProcess();														
	//BOOL KillProcessModule();												//TDR 실행파일 종료

	char* pszSendBuf	= new char[DATASIZE];								//송신확인 배열
	char* pszCenterAddr = new char[DATASIZE];								//IP주소 담을 포인터 변수
	char* pszReplyBuf	= new char[DATASIZE + sizeof(ICMP_ECHO_REPLY)];		//수신확인 배열

	int m_nErrorCount;														//Ping 연결 에러 카운트
	int m_nActiveCount;														//Ping 활성화/비활성화 체크용 
	int m_nResetTime;														//Reset 시간
	UINT m_nPingTime;														//Ping 확인 주기

	PROCESS_INFORMATION m_Process;
	
	CLog  Log;																//Log 사용 클래스
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	
};

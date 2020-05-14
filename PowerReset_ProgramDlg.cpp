
// PowerReset_ProgramDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "afxdialogex.h"
#include "PowerReset_Program.h"
#include "PowerReset_ProgramDlg.h"
#include "CResetTestDlg.h"			//=====하위 Dialog 추가
#include <iostream>
#include "tlhelp32.h"				//=====프로세스 찾기 및 종료
#include <locale.h>					//=====한글 출력
//#include <WinSock2.h>				//=====소켓 통신 헤더 및 라이브러리
#include <WS2tcpip.h>
#include "icmpapi.h"				//=====ICMP 함수 헤더 및 라이브러리
//#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")


using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define ACADD				6					//TDR 전원제어 AC 번호 추가
#define CONNECT_SUCCESS		WM_USER + 1		
#define CONNECT_FAIL		WM_USER + 2
#define CONNECT_ERROR		11050
#define CONNECT_TIMEOUT		11010
#define CONNECT_ERR			1231
#define PAUSE				110
#define ACCTIVE				111
#define IP					200
#define PASSWORD			201
#define PORT				2500


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPowerResetProgramDlg 대화 상자
//memset(&m_Power, 0, sizeof(ResetPowerControl));



CPowerResetProgramDlg::CPowerResetProgramDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_POWERRESET_PROGRAM_DIALOG, pParent)
	, m_RB_Port(_T(""))
	, m_TDR_Password(_T(""))
	, m_AcNumber(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	memset(&m_Process, 0, sizeof(PROCESS_INFORMATION));
	memset(pszSendBuf, 0, DATASIZE);
	memset(pszReplyBuf, 0, DATASIZE + sizeof(ICMP_ECHO_REPLY));
	sprintf_s(pszSendBuf, DATASIZE, "Echo Data");							//확인용 임시 데이터

	m_nActiveCount  = ACCTIVE;
	m_nErrorCount	= 0;
	m_nPingTime		= 0;
	m_nResetTime	= 0;

	//m_Socket = INVALID_SOCKET;
	//memset(m_SendBuf, 0, SEND_SIZE);
	//memset(m_RecvBuf, 0, RECV_SIZE);
}

void CPowerResetProgramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_CENTER, m_Center_IP);
	DDX_Control(pDX, IDC_IPADDRESS_TDR, m_TDR_IP);
	DDX_Text(pDX, IDC_EDIT_PORT, m_RB_Port);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_TDR_Password);
	DDX_Text(pDX, IDC_EDIT_AC, m_AcNumber);
}

BEGIN_MESSAGE_MAP(CPowerResetProgramDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CPowerResetProgramDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CPowerResetProgramDlg::OnBnClickedButtonConnect)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTT_RESET_TEST, &CPowerResetProgramDlg::OnBnClickedButtResetTest)
ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CPowerResetProgramDlg 메시지 처리기

BOOL CPowerResetProgramDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	setlocale(LC_ALL, "");


	m_nErrorCount = 0;

	m_Power.KillProcessModule();		//TDRPowerControl.exe 종료
	SetIniDataFunc();			//INI 값 설정

	//LogWrite() : 로그데이터 출력
	//LogFileDelete() : 오래된 로그파일 30일 기준 삭제
	Log.LogWrite("=====PowerReset Program SETUP IP : %S=====", m_TempCenter_IP);
	Log.LogFileDelete();

	//************************ 컨트롤 비활성화 ********************//
	GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS_CENTER)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS_TDR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTT_RESET_TEST)->EnableWindow(FALSE);
	//*************************************************************//

	m_bThreadDo = TRUE;
	m_pThread = AfxBeginThread(StartThread, this);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


//Center IP : 192.168.0.101 //TDR IP : 192.168.0.101 //RB IP  : 192.168.0.151
//Password : abcde12#
//Port = 2500	//AC : 5
//*****************************************************************************
//						INI 값 설정 및 화면에 값 로드
//*****************************************************************************
void CPowerResetProgramDlg::SetIniDataFunc()
{
	WCHAR wszApp[MAX_PATH] = { NULL, };
	GetModuleFileName(NULL, wszApp, MAX_PATH);
	PathRemoveFileSpec(wszApp);
	CString strIni;
	strIni.Format(_T("%s\\%s.ini"), wszApp, _T("PowerResetProgram"));
	g_Ini.SetFileName(strIni, TRUE);

	char pszCenterTempAddr[DATASIZE] = {0,}; // 시작 IP값 저장할 배열

	//*************************RB,TDR IP, PASSWORD, PORT*****************************//

	m_PowerIP = g_Ini["POWER"]["IP"] << _T("192.168.0.151");

	int nLength = WideCharToMultiByte(CP_ACP, 0, m_PowerIP, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, m_PowerIP, -1, pszCenterTempAddr, nLength, NULL, NULL);
	
	char* context;
	char* token;
	int i = 0;
	BYTE btTDRIP[4];
	
	token = strtok_s(pszCenterTempAddr, ".", &context);
	btTDRIP[i] = atoi(token);
	
	while (i < 3)
	{
		i++;
		token = strtok_s(NULL, ".", &context);
		btTDRIP[i] = atoi(token);
	}
	
	m_TDR_IP.SetAddress(btTDRIP[0], btTDRIP[1], btTDRIP[2], btTDRIP[3]);

	m_TDR_Password = g_Ini["POWER"]["PASSWORD"] << _T("abcde12#");
	TRACE(_T("password is %s"),m_TDR_Password);
	m_RB_Port = g_Ini["POWER"]["PORT"] << _T("2500");
	m_AcNumber =g_Ini["POWER"]["AC"] << _T("5");
	
	//*******************************Center IP, ACNumber*******************************//

	m_TempCenter_IP = g_Ini["Center"]["IP"] << _T("192.168.0.101");
//******************************************************************************************//
	nLength = WideCharToMultiByte(CP_ACP, 0, m_TempCenter_IP, -1, NULL, 0, NULL, NULL);			//	시작 IP값을 가지고 있는 배열
	WideCharToMultiByte(CP_ACP, 0, m_TempCenter_IP, -1, pszCenterAddr, nLength, NULL, NULL);	//	
//******************************************************************************************//
	nLength = WideCharToMultiByte(CP_ACP, 0, m_TempCenter_IP, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, m_TempCenter_IP, -1, pszCenterTempAddr, nLength, NULL, NULL);

	BYTE btCenterIP[4];
	i = 0;

	token = strtok_s(pszCenterTempAddr, ".", &context);
	btCenterIP[i] = atoi(token);

	while (i < 3)
	{
		i++;
		token = strtok_s(NULL, ".", &context);
		btCenterIP[i] = atoi(token);
	}
	m_Center_IP.SetAddress(btCenterIP[0], btCenterIP[1], btCenterIP[2], btCenterIP[3]);


	//*******************************************************************************
	//							RESET, PING 시간
	//*******************************************************************************
#ifdef _DEBUG
	m_nResetTime = g_Ini["TIME"]["RESET"] << 20;		//Second = 20초
	m_nPingTime  = g_Ini["TIME"]["PING"] << 1000;		//Millisecond = 1초
#else
	m_nResetTime = g_Ini["TIME"]["RESET"] << 300;		//Second = 300초 = 5분
	m_nPingTime  = g_Ini["TIME"]["PING"] << 10000;		//Millisecond = 10초
#endif

	//==================================================INI 값 설정
	g_Ini["POWER"]["IP"]		 = m_PowerIP;
	g_Ini["POWER"]["PASSWORD"]	 = m_TDR_Password;
	g_Ini["POWER"]["PORT"]		 = m_RB_Port;
	g_Ini["POWER"]["AC"]		 = m_AcNumber;
	g_Ini["Center"]["IP"]		 = m_TempCenter_IP;
	g_Ini["TIME"]["RESET"]		 = m_nResetTime;
	g_Ini["TIME"]["PING"]		 = m_nPingTime;

	UpdateData(FALSE);

}


void CPowerResetProgramDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPowerResetProgramDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPowerResetProgramDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPowerResetProgramDlg::OnBnClickedButtonExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bThreadDo = FALSE;
	OnOK();
}


BOOL CPowerResetProgramDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)return TRUE;
	if (pMsg->wParam == VK_F4)
	{
		HWND hWnd = GetConsoleWindow();
		::ShowWindow(hWnd,TRUE);
	}
	else if (pMsg->wParam == VK_F5)
	{
		HWND hWnd = GetConsoleWindow();
		::ShowWindow(hWnd, FALSE);
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPowerResetProgramDlg::OnBnClickedButtonConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//****************************************************************활성화
	if (m_nActiveCount == ACCTIVE)
	{
		m_bThreadDo = FALSE;

		if (m_pThread != NULL)
			WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_IPADDRESS_CENTER)->EnableWindow(TRUE);
		GetDlgItem(IDC_IPADDRESS_TDR)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTT_RESET_TEST)->EnableWindow(TRUE);

		m_nErrorCount  = 0;
		m_nActiveCount = PAUSE;
	}
	//****************************************************************비활성화
	else if (m_nActiveCount == PAUSE)
	{
		GetDlgItem(IDC_BUTT_RESET_TEST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDRESS_CENTER)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDRESS_TDR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AC)->EnableWindow(FALSE);

		GetControlValueSettingFunc();
		
		Log.LogWrite("=====Ping Check RESETUP IP : %S=====", m_TempCenter_IP);
		m_nErrorCount  = 0;
		m_nActiveCount = ACCTIVE;

		m_bThreadDo = TRUE;
		m_pThread = AfxBeginThread(StartThread, this);
	}
	
}

//***************************************************************************
//				비활성화 클릭 시 INI 값 변경 유지
//***************************************************************************
void CPowerResetProgramDlg::GetControlValueSettingFunc()
{
	UpdateData(TRUE);

	BYTE IP_Fst, IP_Sec, IP_Trd, IP_Fth;

	//*******************************CENTER IP 변경 ******************************//
	m_Center_IP.GetAddress(IP_Fst, IP_Sec, IP_Trd, IP_Fth);
	m_TempCenter_IP.Format(_T("%d.%d.%d.%d"), IP_Fst, IP_Sec, IP_Trd, IP_Fth);

	int nLength = WideCharToMultiByte(CP_ACP, 0, m_TempCenter_IP, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, m_TempCenter_IP, -1, pszCenterAddr, nLength, NULL, NULL);

	//*******************************전원제어기 IP 변경 *****************************//
	m_TDR_IP.GetAddress(IP_Fst, IP_Sec, IP_Trd, IP_Fth);
	m_PowerIP.Format(_T("%d.%d.%d.%d"), IP_Fst, IP_Sec, IP_Trd, IP_Fth);

	//******************************INI 값 변경 ******************//

	g_Ini["CENTER"]["IP"]		= m_TempCenter_IP;
	g_Ini["POWER"]["IP"]		= m_PowerIP;
	g_Ini["POWER"]["PORT"]		= m_RB_Port;
	g_Ini["POWER"]["PASSWORD"]  = m_TDR_Password;
	g_Ini["POWER"]["AC"]		= m_AcNumber;
	g_Ini["TIME"]["PING"]		= m_nPingTime;
	g_Ini["TIME"]["RESET"]		= m_nResetTime;
}
//***************************************************************************************
//									스레드 함수 시작
//***************************************************************************************
UINT CPowerResetProgramDlg::StartThread(LPVOID pThread)
{
	CPowerResetProgramDlg* pMyDlg = (CPowerResetProgramDlg*)pThread;

	HANDLE hIcmp = IcmpCreateFile();

	if (hIcmp == INVALID_HANDLE_VALUE)
	{
		IcmpCloseHandle(hIcmp);
		return 0;
	}

	int nMsgData	= 0;
	int nAcNumber	= _wtoi(pMyDlg->m_AcNumber);

	SYSTEMTIME st, stLoop;
	memset(&st, NULL, sizeof(SYSTEMTIME));
	memset(&stLoop, NULL, sizeof(SYSTEMTIME));

	//*********************************************************************
	//								옵션 설정
	//*********************************************************************
	IP_OPTION_INFORMATION option;
	memset(&option, NULL, sizeof(IP_OPTION_INFORMATION));

	option.Ttl	 = 128;
	option.Tos	 = 0;
	option.Flags = IP_FLAG_DF;

	OUTPUTDATA outputData;
	DWORD dwCenterAddr;
	inet_pton(AF_INET,pMyDlg->pszCenterAddr,&dwCenterAddr);

	//DEBUG Mode = Timer Loop BreakTime is Not 1000
	//DEBUG Mode Ping First CheckTime longer then 1000.
	//*********************************************************************
	//			  				연결 상태 확인
	//*********************************************************************
	DWORD dwCurTime = GetTickCount();

	while (pMyDlg->m_bThreadDo)
	{
		Sleep(1);
		//*************************************************
		DWORD dwTick = GetTickCount();					///	 *DEBUG = 1초 확인
		if (dwTick - dwCurTime < pMyDlg->m_nPingTime)	///	 *RELEASE = 10초 확인
			continue;									///
		dwCurTime = dwTick;								///
		//*************************************************
		
		DWORD dwRet = IcmpSendEcho(hIcmp, dwCenterAddr, pMyDlg->pszSendBuf, DATASIZE, &option, pMyDlg->pszReplyBuf, DATASIZE + sizeof(ICMP_ECHO_REPLY), 1500);

		if (dwRet > 0)
		{
			ICMP_ECHO_REPLY EchoReply;
			memcpy(&EchoReply, pMyDlg->pszReplyBuf, sizeof(ICMP_ECHO_REPLY));

			struct in_addr replyAddr;
			replyAddr.S_un.S_addr = EchoReply.Address;

			outputData.strAddr.Format(_T("%d.%d.%d.%d")
				, replyAddr.S_un.S_un_b.s_b1
				, replyAddr.S_un.S_un_b.s_b2
				, replyAddr.S_un.S_un_b.s_b3
				, replyAddr.S_un.S_un_b.s_b4);

			outputData.strRoundTrip.Format(_T("%d"), EchoReply.RoundTripTime);
			outputData.DataSize.Format(_T("%d"), EchoReply.DataSize);
			outputData.TTL.Format(_T("%d"), option.Ttl);


			//**********************************************************수신 바이트 == 0 일 때, 비정상
			if (outputData.DataSize.CompareNoCase(_T("0")) == 0)
			{
				nMsgData = CONNECT_FAIL;
				pMyDlg->OnUserFunc(nMsgData, (LPARAM)&outputData);
				pMyDlg->m_nErrorCount++;
			}
			//**********************************************************수신 바이트 == 32, 정상 연결
			else if (outputData.DataSize.CompareNoCase(_T("32")) == 0)
			{
				nMsgData = CONNECT_SUCCESS;
				pMyDlg->OnUserFunc(nMsgData, (LPARAM)&outputData);
				pMyDlg->m_nErrorCount = 0;		
			}
			//*********************************************************** TDR,RB 리셋

			if (pMyDlg->m_nErrorCount == 5)
			{
				wcout << _T("TDR, RB 전원제어기 RESET : AC Number = ") << nAcNumber << endl;
				m_Power.TDRPowerResetFunc(pMyDlg->m_PowerIP, pMyDlg->m_TDR_Password, pMyDlg->m_AcNumber);
				m_Power.RBPowerResetFunc(pMyDlg->m_PowerIP, pMyDlg->m_RB_Port, pMyDlg->m_AcNumber);
								
				GetLocalTime(&st);									 
				while (TRUE)								
				{							
					Sleep(1);

					if (pMyDlg->m_bThreadDo == FALSE)
						break;
//******************************************************************** 리셋 대기 시간 ****************************************						
					GetLocalTime(&stLoop);																					//
																															//	* DEBUG = 20초
					if (((stLoop.wMinute * 60) + stLoop.wSecond) - ((st.wMinute * 60) + st.wSecond) < pMyDlg->m_nResetTime)	//	* RELEASE = 300초 (5분)
						continue;																							//
//****************************************************************************************************************************			

					pMyDlg->Log.LogWrite("=====Ping Check RESTART=====");
					m_Power.KillProcessModule();
					pMyDlg->m_nErrorCount = 0;
					break;								
				}											
			}
		}

		else if (dwRet == 0)
		{
			pMyDlg->GetErrorOutputFunc();

			if (pMyDlg->m_nErrorCount == 5)
			{
				wcout << _T("TDR, RB 전원제어기 RESET // AC Number = ") << nAcNumber << endl;
				m_Power.TDRPowerResetFunc(pMyDlg->m_PowerIP, pMyDlg->m_TDR_Password, pMyDlg->m_AcNumber);
				m_Power.RBPowerResetFunc(pMyDlg->m_PowerIP, pMyDlg->m_RB_Port, pMyDlg->m_AcNumber);

				GetLocalTime(&st);
				while (TRUE)
				{
					if (pMyDlg->m_bThreadDo == FALSE)
						break;
//******************************************************************** 리셋 대기 시간 ********************************************						
					GetLocalTime(&stLoop);																						//
																																//	* DEBUG = 20초
					if (((stLoop.wMinute * 60) + stLoop.wSecond) - ((st.wMinute * 60) + st.wSecond) < pMyDlg->m_nResetTime)		//	* RELEASE = 300초 (5분)
						continue;																								//
//********************************************************************************************************************************				
					pMyDlg->Log.LogWrite("=====Ping Check RESTART=====");
					m_Power.KillProcessModule();
					pMyDlg->m_nErrorCount = 0;
					break;
				}
			}
		}
	}
	pMyDlg->m_pThread = NULL;
	return 0;
}


//******************************************************************************
//								Ping 에러 검출
//******************************************************************************
void CPowerResetProgramDlg::GetErrorOutputFunc()
{
	int nMsgSendData;
	switch (GetLastError())
	{
	case 1231:
		nMsgSendData = CONNECT_ERR;
		OnUserFunc(nMsgSendData, NULL);
		m_nErrorCount++;
		break;
	case 11010:
		nMsgSendData = CONNECT_TIMEOUT;
		OnUserFunc(nMsgSendData, NULL);
		m_nErrorCount++;
		break;
	case 11050:
		nMsgSendData = CONNECT_ERROR;
		OnUserFunc(nMsgSendData, NULL);
		m_nErrorCount++;
		break;
	}
}

//*****************************************************************************************
//										연결 상태 출력
//*****************************************************************************************
LRESULT CPowerResetProgramDlg::OnUserFunc(WPARAM wParam, LPARAM lParam)
{
	OUTPUTDATA* outputdata = (OUTPUTDATA*)lParam;

	switch (wParam)
	{
	case CONNECT_SUCCESS:
		wcout << _T("IP : ") << outputdata->strAddr.GetBuffer() << _T("의 응답 : ");
		wcout << _T("   바이트 : ") << outputdata->DataSize.GetBuffer() << _T("   시간 : ");
		wcout << outputdata->strRoundTrip.GetBuffer() << _T("ms ") << _T("   TTL : ") << outputdata->TTL.GetBuffer() << endl;
		break;
	case CONNECT_FAIL:
		//CMD
		wcout << _T("IP : ") << outputdata->strAddr.GetBuffer() << _T("의 응답 : 대상 호스트에 연결할 수 없습니다.");
		wcout << _T("  (") << m_nErrorCount + 1 << _T("/5)") << endl;
		//Log
		Log.LogWrite("대상 호스트에 연결할 수 없습니다. (%d/5)", m_nErrorCount + 1);
		break;
	case CONNECT_ERR:
		//CMD
		wcout << _T("IP : ") << outputdata->strAddr.GetBuffer() << _T("의 응답 : 대상 호스트에 연결할 수 없습니다.");
		wcout << _T("  (") << m_nErrorCount + 1 << _T("/5)") << endl;
		//Log
		Log.LogWrite("대상 호스트에 연결할 수 없습니다. (%d/5)", m_nErrorCount + 1);
		break;
	case CONNECT_TIMEOUT:
		//CMD
		wcout << _T("요청 시간이 만료 되었습니다.");
		wcout << _T("  (") << m_nErrorCount + 1 << _T("/5)") << endl;
		//Log
		Log.LogWrite("요청 시간이 만료 되었습니다. (%d/5)", m_nErrorCount + 1);
		break;
	case CONNECT_ERROR:
		//CMD
		wcout << _T("IP : ") << outputdata->strAddr.GetBuffer() << _T("의 응답 : 대상 호스트에 연결할 수 없습니다.");
		wcout << _T("  (") << m_nErrorCount + 1 << _T("/5)") << endl;
		//Log
		Log.LogWrite("대상 호스트에 연결할 수 없습니다. (%d/5)", m_nErrorCount+1);
		break;
	}

	return 0;
}

////*****************************************************************************************
////									TDR 전원 제어
////*****************************************************************************************
//
//void CPowerResetProgramDlg::TDRPowerResetFunc()
//{
//	CString strObject;
//	int nACNumber = _wtoi(m_AcNumber) + ACADD;			//AC 번호 + 6 == 리셋 AC 번호
//
//	strObject.Format(_T("TDRPowerControl.exe %s %s %d"), m_PowerIP, m_TDR_Password, nACNumber);
//	
//	ExecProcess(strObject, nACNumber);
//}
//
//
//BOOL CPowerResetProgramDlg::ExecProcess(CString strObject, int nACNumber)
//{
//	if (m_Process.hProcess != INVALID_HANDLE_VALUE)
//	{
//		if (!KillProcess())
//		{
//			//m_ListOutput.AddString(_T("KillProcess Error : ")+dwError);
//		}
//		Sleep(100);
//	}
//
//	STARTUPINFO si;
//	ZeroMemory(&si, sizeof(si));
//
//	si.cb			= sizeof(STARTUPINFO);			//구조체 크기
//	si.dwFlags		= STARTF_USESHOWWINDOW;			//wShowWindow 기능 사용
//	si.dwXSize		= 0;							//창의 너비	
//	si.dwX			= 0;							//창의 x 픽셀		
//	si.dwY			= 0;							//창의 y 픽셀
//	si.wShowWindow  = SW_HIDE;						//nCmdShow 파라미터에 지정할수 있는 값 
//
//	if (CreateProcessW(NULL, strObject.GetBuffer(0), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_Process) == FALSE)
//	{
//		/*CreateProcessW
//		LPCSTR				  IpAppLicationName			= 실행할 모듈의 이름
//		LPSTR				  IpCommandLine				= 실행할 명령 줄
//		LPSECURITY_ATTRIBUTES lpProcessAttributes,		= 새 프로세스 개체에 반환된 핸들이 하위프로세스에 의해 상속될 수 있는지 여부
//		LPSECURITY_ATTRIBUTES lpThreadAttributes,		= 스레드 객체에 반환된 핸들이 하위프로세스에 의해 상속될 수 있는지 여부
//		BOOL                  bInheritHandles,			= TRUE : 상속 , FALSE : 상속 X
//		DWORD                 dwCreationFlags,			= 우선 순위 클래스와 프로세스 생성을 제어하는 플래그 값, 기본 값 : NORMAL_PRIORITY_CLASS
//		LPVOID                lpEnvironment,			= 새 프로세스의 환경블록에 대한 포인터, NULL : 호출 프로세스의 환경 사용
//		LPCSTR                lpCurrentDirectory,		= 현재 디렉터리에 대한 전체 경로
//		LPSTARTUPINFOA        lpStartupInfo,			= STARTUPINFO 구조체
//		LPPROCESS_INFORMATION lpProcessInformation		= PROCESS_INFORMATION 구조체*/
//
//		DWORD dwLastError(0);
//		dwLastError = GetLastError();
//
//		//m_ListOutput.AddString(_T("CreateProcess Error : ")+dwError);
//		CloseHandle(m_Process.hProcess);
//		CloseHandle(m_Process.hThread);
//
//		m_Process.hProcess = INVALID_HANDLE_VALUE;
//		m_Process.hThread  = INVALID_HANDLE_VALUE;
//
//		return FALSE;
//	}
//
//	switch (nACNumber)
//	{
//	case 7:
//		Log.LogWrite("=====TDR Power AC1 RESET=====");
//		break;
//	case 8:
//		Log.LogWrite("=====TDR Power AC2 RESET=====");
//		break;
//	case 9:
//		Log.LogWrite("=====TDR Power AC3 RESET=====");
//		break;
//	case 10:
//		Log.LogWrite("=====TDR Power AC4 RESET=====");
//		break;
//	case 11:
//		Log.LogWrite("=====TDR Power AC5 RESET=====");
//		break;
//	case 12:
//		Log.LogWrite("=====TDR Power AC6 RESET=====");
//		break;
//	case 13:
//		Log.LogWrite("=====TDR Power AC7 RESET=====");
//		break;
//	case 14:
//		Log.LogWrite("=====TDR Power AC8 RESET=====");
//		break;
//	}
//
//	return TRUE;
//}
//
//
//
//BOOL CPowerResetProgramDlg::KillProcess()
//
//{
//	DWORD dwError(0);
//	DWORD dwExitcode(0);
//
//	if (m_Process.hProcess == INVALID_HANDLE_VALUE)
//		return FALSE;
//
//
//	//=======================================================================프로세스 종료상태 확인
//	if (!GetExitCodeProcess((HANDLE)m_Process.hProcess, &dwExitcode))
//	{
//		dwError = GetLastError();
//		//에러 코드 작성
//		return FALSE;
//	}
//	else
//	{	//===================================================================지정 프로세스와 모든 스레드 종료
//		if (!TerminateProcess((HANDLE)m_Process.hProcess, dwExitcode))
//		{
//			dwError = GetLastError();
//			//에러코드작성
//			return FALSE;
//		}
//	}
//	CloseHandle(m_Process.hProcess);
//	CloseHandle(m_Process.hThread);
//	m_Process.hProcess = INVALID_HANDLE_VALUE;
//	m_Process.hThread = INVALID_HANDLE_VALUE;
//	return TRUE;
//}
//

void CPowerResetProgramDlg::OnDestroy()
{
	//==========================스레드 종료
	m_bThreadDo = FALSE;
	if (m_pThread != NULL)
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);

	//==========================콘솔창 숨김
	HWND hWnd = GetConsoleWindow();
	::ShowWindow(hWnd, FALSE);

	//==========================TDR프로그램 종료
	m_Power.KillProcessModule();

	delete[] pszCenterAddr;
	delete[] pszReplyBuf;
	delete[] pszSendBuf;
	pszCenterAddr = NULL;
	pszReplyBuf = NULL;
	pszSendBuf = NULL;
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


////*******************************************************************************
////						TDR 실행 프로그램 종료
////*******************************************************************************
//BOOL CPowerResetProgramDlg::KillProcessModule()
//{
////********************현재 모든 프로세스들의 상태를 스냅*************************//
//	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	PROCESSENTRY32 processEntry32;
//
//	CString strProcessName = _T("TDRPowerControl.exe");
//	DWORD dwExitCode(0);
//
//	if (hProcessSnap == INVALID_HANDLE_VALUE)
//		TRACE(_T("------------------------hProcessSnap == INVALID_HANDLE_VALUE\n"));
//		//exit(EXIT_FAILURE);
//
//	processEntry32.dwSize = sizeof(PROCESSENTRY32);
//
//	if (!Process32First(hProcessSnap, &processEntry32))
//	{
//		CloseHandle(hProcessSnap);
//		TRACE(_T("------------------------hProcessSnap == process32First Failed.\n"));
//		//exit(EXIT_FAILURE);
//	}
//
//
//	while (Process32Next(hProcessSnap, &processEntry32))
//	{
//		//*******************************실행중인 프로세스가 있다면 종료***************************//
//		if (strProcessName.Compare(processEntry32.szExeFile) == 0)
//		{
//			HANDLE hKillProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry32.th32ProcessID);
//
//			if (hKillProcess)
//			{
//				if (TerminateProcess(hKillProcess, dwExitCode))
//				{
//					TRACE(_T("-----------------------Kill TDRProgram-----------------------\n"));
//					GetExitCodeProcess(hKillProcess, &dwExitCode);
//				}
//			}
//		}
//	}
//	CloseHandle(hProcessSnap);
//		return FALSE;
//}
//
////*******************************************************************************
////							RB 전원제어기 리셋 함수 호출
////*******************************************************************************
//void CPowerResetProgramDlg::RBPowerResetFunc()
//{
//	int nAcNumber	= _wtoi(m_AcNumber);
//	//int nPort		= _wtoi(m_RB_Port);
//	int nPort = 2500;
//	CString m_PowerIP = _T("192.168.0.151");
//	char* pszRBIP	= new char[DATASIZE];
//
//	int nLength = WideCharToMultiByte(CP_ACP, 0, m_PowerIP, -1, NULL, 0, NULL, NULL);
//	WideCharToMultiByte(CP_ACP, 0, m_PowerIP, -1, pszRBIP, nLength, NULL, NULL);
//
//
//	m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (m_Socket == INVALID_SOCKET)
//	{
//		TRACE(_T("===============SOCKET ERROR %d\n"),WSAGetLastError());
//		return;
//	}
//
//	SOCKADDR_IN SocketAddr = {};
//	SocketAddr.sin_family = AF_INET;
//	SocketAddr.sin_port = htons(nPort);
//	SocketAddr.sin_addr.s_addr = inet_addr(pszRBIP);
//
//	//연결 시간 매우느림 . 리셋 중 다시 시작하려할 때 늦게 풀리는 이유.
//	//수정 요망
//	
//	DWORD dwTick = GetTickCount();
//
//	if (connect(m_Socket, (SOCKADDR*)&SocketAddr, sizeof(SocketAddr)) == CONNECT_ERROR)
//	{
//		TRACE(_T("==============Connect Error %d\n"),WSAGetLastError());
//		delete[] pszRBIP;
//		pszRBIP = NULL;
//		shutdown(m_Socket, SD_BOTH);
//		closesocket(m_Socket);
//		m_Socket = INVALID_SOCKET;
//		return;
//	}
//	TRACE(_T("Connect Complete : %02d ms\n"), GetTickCount() - dwTick);
//
//
//	switch (nAcNumber)
//	{
//	case 1:
//		ResetSendDataPC1();
//		break;
//	case 2:
//		ResetSendDataPC2();
//		break;
//	case 3:
//		ResetSendDataCAM1();
//		break;
//	case 4:
//		ResetSendDataCAM2();
//		break;
//	case 5:
//		ResetSendDataNETWORK();
//		break;
//	case 6:
//		ResetSendDataLOOP1();
//		break;
//	case 7:
//		ResetSendDataLOOP2();
//		break;
//	}
//	
//	delete[] pszRBIP;
//	pszRBIP = NULL;
//	shutdown(m_Socket,SD_BOTH);
//	closesocket(m_Socket);
//	m_Socket = INVALID_SOCKET;
//	return;
//}
//
//
////*******************************************************************************
////						RB 전원제어기 리셋 데이터 전송
////*******************************************************************************
//void CPowerResetProgramDlg::ResetSendDataPC1()
//{
//	BYTE SocketSendData[3] = { 0, };
//
//	SocketSendData[0] = 1;						//PC1 리셋
//	SocketSendData[1] = 0x03;					//리셋
//	SocketSendData[2] = RESETTIME_PC1;			//PC1 리셋시간
//
//	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
//	{
//		TRACE(_T("---------------SocketSend Error PC1 : %d \n"), WSAGetLastError());
//		return;
//	}
//	Log.LogWrite("=====RB Power PC1 RESET=====");
//}
//
//void CPowerResetProgramDlg::ResetSendDataPC2()
//{
//	BYTE SocketSendData[3] = { 0, };
//
//	SocketSendData[0] = 2;						//PC2 리셋
//	SocketSendData[1] = 0x03;					//리셋
//	SocketSendData[2] = RESETTIME_PC2;			//PC2 리셋시간
//	
//	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
//	{
//		TRACE(_T("---------------SocketSend Error PC2 : %d \n"), WSAGetLastError());
//		return;
//	}
//	Log.LogWrite("=====RB Power PC2 RESET=====");
//}
//
//void CPowerResetProgramDlg::ResetSendDataCAM1()
//{
//	BYTE SocketSendData[3] = { 0, };
//
//	SocketSendData[0] = 3;						//CAM1 리셋
//	SocketSendData[1] = 0x03;					//리셋
//	SocketSendData[2] = RESETTIME_CAM1;			//CAM1 리셋시간
//
//	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
//	{
//		TRACE(_T("---------------SocketSend Error CAM1 : %d \n"), WSAGetLastError());
//		return;
//	}
//	Log.LogWrite("=====RB Power CAM1 RESET=====");
//}
//
//void CPowerResetProgramDlg::ResetSendDataCAM2()
//{
//	BYTE SocketSendData[3] = { 0, };
//
//	SocketSendData[0] = 4;						//CAM2 리셋
//	SocketSendData[1] = 0x03;					//리셋
//	SocketSendData[2] = RESETTIME_CAM2;			//CAM2 리셋시간
//
//	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
//	{
//		TRACE(_T("---------------SocketSend Error CAM2 : %d \n"), WSAGetLastError());
//		return;
//	}
//	Log.LogWrite("=====RB Power CAM2 RESET=====");
//}
//
//void CPowerResetProgramDlg::ResetSendDataNETWORK()
//{
//	BYTE SocketSendData[3] = { 0, };
//
//	SocketSendData[0] = 5;						//NETWORK 리셋
//	SocketSendData[1] = 0x03;					//리셋
//	SocketSendData[2] = RESETTIME_NETWORK;		//NETWORK 리셋시간
//
//	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
//	{
//		TRACE(_T("---------------SocketSend Error NETWORK : %d \n"), WSAGetLastError());
//		return;
//	}
//	Log.LogWrite("=====RB Power NEWWORK RESET=====");
//}
//
//void CPowerResetProgramDlg::ResetSendDataLOOP1()
//{
//	BYTE SocketSendData[3] = { 0, };
//
//	SocketSendData[0] = 6;						//LOOP1 리셋
//	SocketSendData[1] = 0x03;					//리셋
//	SocketSendData[2] = RESETTIME_LOOP1;		//LOOP1 리셋시간
//
//	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
//	{
//		TRACE(_T("---------------SocketSend Error LOOP1 : %d \n"), WSAGetLastError());
//		return;
//	}
//	Log.LogWrite("=====RB Power LOOP1 RESET=====");
//}
//
//void CPowerResetProgramDlg::ResetSendDataLOOP2()
//{
//	BYTE SocketSendData[3] = { 0, };
//
//	SocketSendData[0] = 7;						//LOOP2 리셋
//	SocketSendData[1] = 0x03;					//리셋
//	SocketSendData[2] = RESETTIME_LOOP2;		//LOOP2 리셋시간
//
//	if (SocketSend(0x05, SocketSendData, 3) == SOCKET_ERROR)
//	{
//		TRACE(_T("---------------SocketSend Error LOOP2 : %d \n"), WSAGetLastError());
//		return;
//	}
//	Log.LogWrite("=====RB Power LOOP2 RESET=====");
//}
//
//
////*******************************************************************************
////						RB전원제어기 리셋 요청 응답
////*******************************************************************************
//int CPowerResetProgramDlg::SocketSend(BYTE btCode, BYTE* btData, int nLength)
//{
//	if (m_Socket == INVALID_SOCKET)
//	{
//		TRACE(_T("Send Fail \n"));
//		return 0;
//	}
//
//	m_SendBuf[0] = DLE;																	//DLE
//	m_SendBuf[1] = STX;																	//STX
//	m_SendBuf[2] = btCode;																//OPCODE
//	m_SendBuf[3] = (BYTE)nLength;														//LENGTH
//	memcpy(m_SendBuf + HEADER_SIZE + IDENTIFIER_SIZE, btData, nLength);					//DATA
//	m_SendBuf[HEADER_SIZE + IDENTIFIER_SIZE + nLength] = DLE;							//DLE
//	m_SendBuf[HEADER_SIZE + IDENTIFIER_SIZE + nLength + 1] = ETX;						//ETX
//	UINT nCrc = CRC16((char*)m_SendBuf + HEADER_SIZE, IDENTIFIER_SIZE + nLength);
//	m_SendBuf[HEADER_SIZE + IDENTIFIER_SIZE + nLength + TAIL_SIZE] = HIBYTE(nCrc);		//CRC
//	m_SendBuf[HEADER_SIZE + IDENTIFIER_SIZE + nLength + TAIL_SIZE + 1] = LOBYTE(nCrc);	//CRC
//
//	int nRet = SOCKET_ERROR;
//	nRet = send(m_Socket, (char*)m_SendBuf, HEADER_SIZE + IDENTIFIER_SIZE + nLength + TAIL_SIZE + CRC_SIZE,0);
//
//	return nRet;
//}
//
////****************************************************************************************
////						CRC 에러체크 : OPCODE, LENGTH, DATA 계산
////****************************************************************************************
//UINT CPowerResetProgramDlg::CRC16(char* rData, UINT len)
//{
//	UINT	wCrc = 0xFFFF;
//	UINT	wCh;
//
//	for (UINT i = 0; i < len; i++)
//	{
//		wCh = (UINT) * (rData + i);
//
//		for (UINT j = 0; j < 8; j++)
//		{
//			if ((wCh ^ wCrc) & 0x0001)
//				wCrc = (wCrc >> 1) ^ 0xA001;
//			else
//				wCrc >>= 1;
//
//			wCh >>= 1;
//		}
//	}
//
//	return wCrc;
//}



//************************************************************************************
//					TDR, RB 전원제어기 ResetTest Dialog 띄우기
//************************************************************************************
void CPowerResetProgramDlg::OnBnClickedButtResetTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bThreadDo = FALSE;

	GetControlValueSettingFunc();

	CResetTestDlg ResetTestDlg(this);
	ResetTestDlg.DoModal();
	ResetTestDlg.DestroyWindow();
}



//************************************************************************************
//								DIALOG 크기 고정
//************************************************************************************
void CPowerResetProgramDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	lpMMI->ptMinTrackSize = CPoint(330,350);
	lpMMI->ptMaxTrackSize = CPoint(330,350);

	CDialogEx::OnGetMinMaxInfo(lpMMI);

}


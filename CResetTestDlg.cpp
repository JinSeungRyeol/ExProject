// CResetTestDlg.cpp: 구현 파일
//

#include "pch.h"
#include "PowerReset_Program.h"
#include "CResetTestDlg.h"
#include "afxdialogex.h"

#define IP 200
#define PASSWORD 201
#define PORT 2500


// CResetTestDlg 대화 상자

IMPLEMENT_DYNAMIC(CResetTestDlg, CDialogEx)

CResetTestDlg::CResetTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ResetTestDlg, pParent)
{
	m_strAcNum = "";
	m_strIP = "";
	m_strPort = "";
	m_strPW = "";
}

CResetTestDlg::~CResetTestDlg()
{
}

void CResetTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResetTestDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTT_TEST_EXIT, &CResetTestDlg::OnBnClickedButtTestExit)
	ON_BN_CLICKED(IDC_BUTT_TDR_AC1, &CResetTestDlg::OnBnClickedButtTdrAc1)
	ON_BN_CLICKED(IDC_BUTT_TDR_AC2, &CResetTestDlg::OnBnClickedButtTdrAc2)
	ON_BN_CLICKED(IDC_BUTT_TDR_AC3, &CResetTestDlg::OnBnClickedButtTdrAc3)
	ON_BN_CLICKED(IDC_BUTT_TDR_AC4, &CResetTestDlg::OnBnClickedButtTdrAc4)
	ON_BN_CLICKED(IDC_BUTT_TDR_AC5, &CResetTestDlg::OnBnClickedButtTdrAc5)
	ON_BN_CLICKED(IDC_BUTT_TDR_AC6, &CResetTestDlg::OnBnClickedButtTdrAc6)
	ON_BN_CLICKED(IDC_BUTT_TDR_AC7, &CResetTestDlg::OnBnClickedButtTdrAc7)
	ON_BN_CLICKED(IDC_BUTT_TDR_AC8, &CResetTestDlg::OnBnClickedButtTdrAc8)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTT_RB_AC1, &CResetTestDlg::OnBnClickedButtRbAc1)
	ON_BN_CLICKED(IDC_BUTT_RB_AC2, &CResetTestDlg::OnBnClickedButtRbAc2)
	ON_BN_CLICKED(IDC_BUTT_RB_AC3, &CResetTestDlg::OnBnClickedButtRbAc3)
	ON_BN_CLICKED(IDC_BUTT_RB_AC4, &CResetTestDlg::OnBnClickedButtRbAc4)
	ON_BN_CLICKED(IDC_BUTT_RB_AC5, &CResetTestDlg::OnBnClickedButtRbAc5)
	ON_BN_CLICKED(IDC_BUTT_RB_AC6, &CResetTestDlg::OnBnClickedButtRbAc6)
	ON_BN_CLICKED(IDC_BUTT_RB_AC7, &CResetTestDlg::OnBnClickedButtRbAc7)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CResetTestDlg 메시지 처리기




BOOL CResetTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.


	m_Power.KillProcessModule();

	m_strIP		= g_Ini["POWER"]["IP"];
	m_strPW		= g_Ini["POWER"]["PASSWORD"];
	m_strPort	= g_Ini["POWER"]["PORT"];
	m_strAcNum	= g_Ini["POWER"]["AC"];

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CResetTestDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CResetTestDlg::OnBnClickedButtTestExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}


//********************************************************************************************
//										TDR 전원 제어
//********************************************************************************************

//	TDR AC 1
void CResetTestDlg::OnBnClickedButtTdrAc1()
{
	m_strAcNum  = "1";

	m_Power.TDRPowerResetFunc(m_strIP, m_strPW, m_strAcNum);
}

//	TDR AC 2
void CResetTestDlg::OnBnClickedButtTdrAc2()
{
	m_strAcNum = "2";

	m_Power.TDRPowerResetFunc(m_strIP, m_strPW, m_strAcNum);
}

//	TDR AC 3
void CResetTestDlg::OnBnClickedButtTdrAc3()
{
	m_strAcNum = "3";

	m_Power.TDRPowerResetFunc(m_strIP, m_strPW, m_strAcNum);
}

//	TDR AC 4
void CResetTestDlg::OnBnClickedButtTdrAc4()
{
	m_strAcNum = "4";

	m_Power.TDRPowerResetFunc(m_strIP, m_strPW, m_strAcNum);
}

//	TDR AC 5
void CResetTestDlg::OnBnClickedButtTdrAc5()
{
	m_strAcNum = "5";

	m_Power.TDRPowerResetFunc(m_strIP, m_strPW, m_strAcNum);
}

//	TDR AC 6
void CResetTestDlg::OnBnClickedButtTdrAc6()
{
	m_strAcNum = "6";

	m_Power.TDRPowerResetFunc(m_strIP, m_strPW, m_strAcNum);
}

//	TDR AC 7
void CResetTestDlg::OnBnClickedButtTdrAc7()
{
	m_strAcNum = "7";

	m_Power.TDRPowerResetFunc(m_strIP, m_strPW, m_strAcNum);
}

//	TDR AC 8
void CResetTestDlg::OnBnClickedButtTdrAc8()
{
	m_strAcNum = "8";

	m_Power.TDRPowerResetFunc(m_strIP, m_strPW, m_strAcNum);
}


//********************************************************************************************
//										RB 전원 제어
//********************************************************************************************

// RB AC 1

void CResetTestDlg::OnBnClickedButtRbAc1()
{
	m_strAcNum = "1";

	m_Power.RBPowerResetFunc(m_strIP, m_strPort, m_strAcNum);
}

// RB AC 2
void CResetTestDlg::OnBnClickedButtRbAc2()
{
	
	m_strAcNum = "2";

	m_Power.RBPowerResetFunc(m_strIP, m_strPort, m_strAcNum);
	
}

// RB AC 3
void CResetTestDlg::OnBnClickedButtRbAc3()
{
	m_strAcNum = "3";

	m_Power.RBPowerResetFunc(m_strIP, m_strPort, m_strAcNum);
}

// RB AC 4
void CResetTestDlg::OnBnClickedButtRbAc4()
{
	m_strAcNum = "4";

	m_Power.RBPowerResetFunc(m_strIP, m_strPort, m_strAcNum);
}

// RB AC 5
void CResetTestDlg::OnBnClickedButtRbAc5()
{
	m_strAcNum = "5";

	m_Power.RBPowerResetFunc(m_strIP, m_strPort, m_strAcNum);
}

// RB AC 6
void CResetTestDlg::OnBnClickedButtRbAc6()
{
	m_strAcNum = "6";

	m_Power.RBPowerResetFunc(m_strIP, m_strPort, m_strAcNum);
}

// RB AC 7
void CResetTestDlg::OnBnClickedButtRbAc7()
{
	m_strAcNum = "7";

	m_Power.RBPowerResetFunc(m_strIP, m_strPort, m_strAcNum);
}

/******************************************************************************************/

void CResetTestDlg::OnDestroy()
{                    
	m_Power.KillProcessModule();

	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CResetTestDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	lpMMI->ptMinTrackSize = CPoint(464,420);
	lpMMI->ptMaxTrackSize = CPoint(464,420);

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

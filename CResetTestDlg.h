#pragma once
#include "PowerReset_ProgramDlg.h"
//#include "ResetPowerControl.h"
// CResetTestDlg 대화 상자

class CResetTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResetTestDlg)

public:
	CResetTestDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CResetTestDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ResetTestDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedButtTestExit();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtTdrAc1();
	afx_msg void OnBnClickedButtTdrAc2();
	afx_msg void OnBnClickedButtTdrAc3();
	afx_msg void OnBnClickedButtTdrAc4();
	afx_msg void OnBnClickedButtTdrAc5();
	afx_msg void OnBnClickedButtTdrAc6();
	afx_msg void OnBnClickedButtTdrAc7();
	afx_msg void OnBnClickedButtTdrAc8();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtRbAc1();
	afx_msg void OnBnClickedButtRbAc2();
	afx_msg void OnBnClickedButtRbAc3();
	afx_msg void OnBnClickedButtRbAc4();
	afx_msg void OnBnClickedButtRbAc5();
	afx_msg void OnBnClickedButtRbAc6();
	afx_msg void OnBnClickedButtRbAc7();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	CString m_strIP;
	CString m_strPW;
	CString m_strPort;
	CString m_strAcNum;

};

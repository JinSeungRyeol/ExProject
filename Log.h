#pragma once
class CLog : public CDialogEx
{
public:
	CString GetFilePath();
		
	void LogWrite(const char* fmt, ...);		//���� ���� �� �α� ���
	void LogFileDelete();
};


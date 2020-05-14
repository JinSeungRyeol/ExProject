#pragma once
class CLog : public CDialogEx
{
public:
	CString GetFilePath();
		
	void LogWrite(const char* fmt, ...);		//파일 생성 및 로그 출력
	void LogFileDelete();
};


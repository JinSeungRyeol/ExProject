#include "pch.h"
#include "Log.h"
#include "locale.h"
#include "stdarg.h"
using namespace std;
//*********************************************************************************************************
//									파일명 및 경로 반환 함수
//*********************************************************************************************************
CString  CLog::GetFilePath()
{
	setlocale(LC_ALL, "");

	SYSTEMTIME stCurTime;
	memset(&stCurTime, 0, sizeof(stCurTime));
	GetLocalTime(&stCurTime);

	CString strFileName;					//파일명
	strFileName.Format(_T(".\\Log\\%04d%02d%02d.log"), stCurTime.wYear, stCurTime.wMonth, stCurTime.wDay);
	CString strPath = strFileName;

	return strPath;
}

//*********************************************************************************************************
//										로그 출력 함수
//*********************************************************************************************************
void CLog::LogWrite(const char* format, ...)
{
	//*******************************************************************************************//
	//									가변 인자 데이터 받기
	//*******************************************************************************************//
	char pszFileData[1024] = { 0, };
	int nLength = 0;

	va_list Marker;
	va_start(Marker, format);

	nLength = _vscprintf(format, Marker) + 1;

	if (pszFileData != NULL)
	{
		vsprintf_s(pszFileData, format, Marker);
	}

	va_end(Marker);
	//===========================================================================================//

	CStdioFile file;
	CFileException e;
	CFileFind finder;

	//******폴더 없을 시 폴더 생성*******//
	CString strDirPath = _T("./Log");
	finder.FindFile(strDirPath);
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(strDirPath))
		CreateDirectory(strDirPath, NULL);

	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	//*************로그 시간*****************//
	CString LogTime;
	LogTime.Format(_T("[%02d:%02d:%02d:%03d]"),stCurTime.wHour, stCurTime.wMinute, stCurTime.wSecond, stCurTime.wMilliseconds);

	//************로그 출력 정보 ************//
	CString LogFileData = LogTime + (CString)pszFileData + _T("\n");
	

	//***** 파일 경로 *****//
	CString strPath = GetFilePath();

	BOOL bRet = file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate ,&e);
	if (bRet == TRUE)
	{
		file.Seek(0, CFile::end);
		file.WriteString(LogFileData);
	}
	else
	{
		TRACE(_T("File Create Error %d\n"), GetLastError());

		return;
	}

	file.Close();
	finder.Close();

}


//*********************************************************************************************************
//									파일 생성 후 30일 경과된 파일 삭제
//*********************************************************************************************************
void CLog::LogFileDelete()
{
	CFile file;
	CFileException e;
	CFileFind finder;

	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	CString strPath = _T(".\\Log\\*.log");
	BOOL bFileCheck = finder.FindFile(strPath);

	while (bFileCheck)
	{
		bFileCheck = finder.FindNextFileW();

		CString strFilePath = finder.GetFilePath();
		CString strFileName = finder.GetFileTitle();

		int nFileDate = _wtoi(strFileName);
		
		int nOldFileYear = nFileDate / 10000;
		int nOldFileMonth = (nFileDate % 10000) / 100;
		int nOldFileDay = nFileDate % 100;
		
		//월 30일 기준으로 가정
		int nOldFileValue = (nOldFileYear * 365) + (nOldFileMonth * 30) + nOldFileDay;
		int nCurTimeValue = (stCurTime.wYear * 365) + (stCurTime.wMonth * 30) + stCurTime.wDay;
		
		//파일 시간 비교 후 삭제 구문 추가
		//저장기간 30일 기준
		if (nCurTimeValue - nOldFileValue >= 30)
		{
			file.Remove(strFilePath);
		}
	}
}


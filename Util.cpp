#include "stdafx.h"
#include "Winver.h"
#include <strsafe.h>

#pragma comment(lib, "version")

CString GetSystemVersion()
{
	CString strFileVersion("");
	LPBYTE  m_lpVersionData = NULL; 

	CString strMyName;
	strMyName.Format(_T("%s.exe"), ::AfxGetApp()->m_pszExeName);

	DWORD dwHandle;
	DWORD dwDataSize = ::GetFileVersionInfoSize((LPTSTR)(LPCTSTR)strMyName, &dwHandle); 
	if( dwDataSize == 0 ) 
		return strMyName;

	// Allocate buffer and retrieve version information
	m_lpVersionData = new BYTE[dwDataSize+1]; 
	if(!::GetFileVersionInfo((LPTSTR)(LPCTSTR)strMyName, dwHandle, dwDataSize, 
		(void**)m_lpVersionData) )
	{
		delete [] m_lpVersionData;
		return strFileVersion;
	}

	UINT nQuerySize;
	VS_FIXEDFILEINFO * vpsffi;
	if(! ::VerQueryValue((void**)m_lpVersionData, _T("\\"),
		(void**)&vpsffi, &nQuerySize) )
	{
		delete [] m_lpVersionData;
		return strFileVersion;
	}

	strFileVersion.Format (_T("%u.%u.%u.%u"),
		HIWORD(vpsffi->dwFileVersionMS),
		LOWORD(vpsffi->dwFileVersionMS),
		HIWORD(vpsffi->dwFileVersionLS),
		LOWORD(vpsffi->dwFileVersionLS));

	delete [] m_lpVersionData;


	return strFileVersion;
}

// 정수값을 읽는다.
INT ReadFromReg_Int(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault, BOOL &bDefault)
{
	HKEY key;
	DWORD dwDisp;
	INT Result;
	DWORD Size;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,
		REG_OPTION_NON_VOLATILE, KEY_READ,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS) 
	{return FALSE;}
	Size=sizeof(LONG);
	if (RegQueryValueEx(key, lpValue, 0, NULL,(LPBYTE)&Result, &Size)
		!=ERROR_SUCCESS)
	{Result=nDefault; bDefault=TRUE;}
	else {bDefault=FALSE;}
	RegCloseKey(key);
	return TRUE;
}

// 문자열을 읽는다.
BOOL ReadFromReg_String(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault, 
					 LPTSTR lpRet, DWORD nSize, BOOL &bDefault) 
{
	HKEY key;
	DWORD dwDisp;
	DWORD Size;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,
		REG_OPTION_NON_VOLATILE, KEY_READ,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS) 
	{return FALSE;}
	Size=nSize;
	if (RegQueryValueEx(key, lpValue, 0, NULL,(LPBYTE)lpRet, &Size)
		!=ERROR_SUCCESS) 
	{_tcscpy_s(lpRet, nSize, lpDefault);bDefault=TRUE;}
	else {bDefault=FALSE;}

	RegCloseKey(key);
	return TRUE;
}

// 정수값을 쓴다.
BOOL WriteToReg_Int(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, UINT nData)
{
	HKEY key;
	DWORD dwDisp;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS) 
	{return FALSE;}
	if (RegSetValueEx(key, lpValue,0,REG_DWORD,(LPBYTE)&nData,sizeof(UINT))
		!=ERROR_SUCCESS) 
	{return FALSE;}
	RegCloseKey(key);
	return TRUE;
}

// 문자열을 쓴다.
BOOL WriteToReg_String(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData)
{
	HKEY key;
	DWORD dwDisp;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS) 
	{return FALSE;}
	size_t DataSize;
	StringCbLength(lpData, 256, &DataSize);
	if (RegSetValueEx(key, lpValue,0,REG_SZ,(LPBYTE)lpData ,DataSize)
		!=ERROR_SUCCESS) 
	{return FALSE;}
	RegCloseKey(key);
	return TRUE;
}

void WriteToIniFile_Int(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, int nValue)
{
	CString str;

	str.Format(_T("%d"), nValue);
	::WritePrivateProfileString(szSection, szKeyName, str, szPathName);
}

void WriteToIniFile_Float(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, float fValue)
{
	CString str;

	str.Format(_T("%f"), fValue);
	::WritePrivateProfileString(szSection, szKeyName, str, szPathName);
}

void WriteToIniFile_String(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, const CString &strValue)
{
	::WritePrivateProfileString(szSection, szKeyName, strValue, szPathName);
}

void ReadFromIniFile_Int(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, int nDefault, int &nReturn)
{
	TCHAR szGet[256];
	CString strDefault;

	// Disk Section
	strDefault.Format(_T("%d"), nDefault);
	DWORD dw = ::GetPrivateProfileString( szSection, szKeyName, strDefault, szGet, 255, szPathName );
	if(strDefault == szGet)
	{
		strDefault.Format(_T("%d"), nDefault+1);
		dw = ::GetPrivateProfileString( szSection, szKeyName, strDefault, szGet, 255, szPathName );
		if(strDefault == szGet)
		{
			WriteToIniFile_Int(szPathName, szSection, szKeyName, nDefault);
		}
		nReturn = nDefault;
	}
	else
	{
		nReturn = _tstoi(szGet);
	}
}	

void ReadFromIniFile_Float(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, float fDefault, float &fReturn)
{
	TCHAR szGet[256];
	CString strDefault;

	// Disk Section
	strDefault.Format(_T("%.10f"), fDefault);
	DWORD dw = ::GetPrivateProfileString( szSection, szKeyName, strDefault, szGet, 255, szPathName );
	if(strDefault == szGet)
	{
		strDefault.Format(_T("%.10f"), fDefault*1.1);
		dw = ::GetPrivateProfileString( szSection, szKeyName, strDefault, szGet, 255, szPathName );
		if(strDefault == szGet)
		{
			WriteToIniFile_Float(szPathName, szSection, szKeyName, fDefault);
		}
		fReturn = fDefault;
	}
	else
	{
		fReturn = float(_tstof(szGet));
	}
}	

void ReadFromIniFile_String(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, const CString &strDefault, CString &strReturn)
{
	TCHAR szGet[256];

	DWORD dw = ::GetPrivateProfileString( szSection, szKeyName, strDefault, szGet, 255, szPathName );
	if(strDefault == szGet)
	{
		CString strBuf;
		strBuf.Format(_T("%sApple"),strDefault);
		dw = ::GetPrivateProfileString( szSection, szKeyName, strBuf, szGet, 255, szPathName );
		if(strBuf == szGet)
		{
			WriteToIniFile_String(szPathName, szSection, szKeyName, strDefault);
		}
		strReturn = strDefault;
	}
	else
	{
		strReturn = CString(szGet);
	}
}

BOOL IsFileExist(LPCTSTR lpPathName, BOOL bMsg)
{
	if(lstrlen(lpPathName) <= 0) { return FALSE; }

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	
	if ((hFind = ::FindFirstFile (lpPathName, &fd)) != INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind);
		return TRUE;
	}
	
	if(bMsg)
	{
		CString strBuf;
		strBuf.Format(_T("%s이 존재하지 않습니다."), lpPathName);
		AfxMessageBox(strBuf);
	}   
	return FALSE;
}

//서브 폴더까지 알아서 생성 시켜 준다. //ReverseFind
BOOL MakeDirectory(CString strPathName)
{
	strPathName+=_T('\\');
	int nMakeTree = 0;
	nMakeTree = strPathName.Find(_T('\\'),nMakeTree);
	CString strBuf;
	
	while(nMakeTree>0 && nMakeTree<strPathName.GetLength())
	{
		strBuf = strPathName.Left(nMakeTree);
		//폴더 만들기.
		if(GetFileAttributes(strBuf) == -1)
		{
			CreateDirectory(strBuf, NULL);
		}
		nMakeTree = strPathName.Find(_T('\\'),nMakeTree+1);
	}
	return TRUE;
}

BOOL MakeFolder(CString strPathName)
{
	return MakeDirectory(strPathName);
}

/*
BOOL CreateDirectories(CString jstr)
{

	int cnt = 0;

	while(jstr.Find(TEXT("\\"),cnt)>0)
	{
		cnt = jstr.Find(TEXT("\\"),cnt);
		CString strDit = jstr.Left(cnt);
		if(strDit.Right(1)!=":")
			CreateDirectory(strDit,NULL);
		cnt++;
		cnt++;
	}

	return TRUE;
}
*/

void GetDirTitleList( CString aszPathName, CString aszFileName, OUT CArray<CString> &arDirTitles )
{
	arDirTitles.RemoveAll();

	if(aszPathName.GetLength() <= 0 || aszFileName.GetLength() <= 0) { return; }
	aszPathName += _T("\\");
	aszPathName.Replace(_T("\\\\"),_T("\\"));
	aszPathName += aszFileName;

	CFileFind	finder;
	BOOL bFound = finder.FindFile(aszPathName);
	while(bFound)
	{
		bFound = finder.FindNextFile();
		if(!finder.IsDots() && finder.IsDirectory())
		{
			arDirTitles.Add(finder.GetFileTitle());
		}
	}
	finder.Close();
	return;
}

void GetDirPathList( CString aszPathName, CString aszFileName, OUT CArray<CString> &arDirPaths )
{
	arDirPaths.RemoveAll();

	if(aszPathName.GetLength() <= 0 || aszFileName.GetLength() <= 0) { return; }
	aszPathName += _T("\\");
	aszPathName.Replace(_T("\\\\"),_T("\\"));
	aszPathName += aszFileName;

	CFileFind	finder;
	BOOL bFound = finder.FindFile(aszPathName);
	while(bFound)
	{
		bFound = finder.FindNextFile();
		if(!finder.IsDots() && finder.IsDirectory())
		{
			arDirPaths.Add(finder.GetFilePath());
		}
	}
	finder.Close();
	return;
}

//c:\myhtml\myfile.txt returns the file name "myfile.txt". 
//c:\myhtml\myfile.txt returns the file path "c:\myhtml\myfile.txt". 
//c:\myhtml\myfile.txt returns the file title "myfile". 
void GetFileTitleList(CString strPathName, CString strFileName, OUT CArray<CString> &arFileTitles)
{
	arFileTitles.RemoveAll();

	if(strPathName.GetLength() <= 0 || strFileName.GetLength() <= 0) { return; }
	strPathName += _T("\\");
	strPathName.Replace(_T("\\\\"),_T("\\"));
	strPathName += strFileName;

	CFileFind	finder;
	BOOL bFound = finder.FindFile(strPathName);
	while(bFound)
	{
		bFound = finder.FindNextFile();
		if(!finder.IsDots() && !finder.IsDirectory())
		{
		//	TRACE(_T("FOUND %s\r\n"), (LPCTSTR)finder.GetFilePath());
			arFileTitles.Add(finder.GetFileTitle());
		}
	}
	finder.Close();
	return;
}

//c:\myhtml\myfile.txt returns the file name "myfile.txt". 
//c:\myhtml\myfile.txt returns the file path "c:\myhtml\myfile.txt". 
//c:\myhtml\myfile.txt returns the file title "myfile". 
void GetFileNameList(CString strPathName, CString strFileName, OUT CArray<CString> &arFileNames)
{
	arFileNames.RemoveAll();

	if(strPathName.GetLength() <= 0 || strFileName.GetLength() <= 0) { return; }
	strPathName += _T("\\");
	strPathName.Replace(_T("\\\\"),_T("\\"));
	strPathName += strFileName;

	CFileFind	finder;
	BOOL bFound = finder.FindFile(strPathName);
	while(bFound)
	{
		bFound = finder.FindNextFile();
		if(!finder.IsDots() && !finder.IsDirectory())
		{
		//	TRACE(_T("FOUND %s\r\n"), (LPCTSTR)finder.GetFilePath());
			arFileNames.Add(finder.GetFileName());
		}
	}
	finder.Close();
	return;
}

//c:\myhtml\myfile.txt returns the file name "myfile.txt". 
//c:\myhtml\myfile.txt returns the file path "c:\myhtml\myfile.txt". 
//c:\myhtml\myfile.txt returns the file title "myfile". 
void GetFilePathList(CString strPathName, CString strFileName, OUT CArray<CString> &arFilePaths)
{
	arFilePaths.RemoveAll();

	if(strPathName.GetLength() <= 0 || strFileName.GetLength() <= 0) { return; }
	strPathName += _T("\\");
	strPathName.Replace(_T("\\\\"),_T("\\"));
	strPathName += strFileName;

	CFileFind	finder;
	BOOL bFound = finder.FindFile(strPathName);
	while(bFound)
	{
		bFound = finder.FindNextFile();
		if(!finder.IsDots() && !finder.IsDirectory())
		{
		//	TRACE(_T("FOUND %s\r\n"), (LPCTSTR)finder.GetFilePath());
			arFilePaths.Add(finder.GetFilePath());
		}
	}
	finder.Close();
	return;
}

void GetFilePathListIncludingSubFolder(CString strPathName, CString strFileName, OUT CArray<CString> &arFilePaths)
{
//	arFilePaths.RemoveAll();

	if(strPathName.GetLength() <= 0 || strFileName.GetLength() <= 0) { return; }
	strPathName += _T("\\");
	strPathName.Replace(_T("\\\\"),_T("\\"));
//	strPathName += strFileName;

	CFileFind	finder;
	BOOL bFound = finder.FindFile(strPathName + strFileName);
	if(!bFound)
	{
		CArray<CString> arPaths;
		GetDirPathList(strPathName, _T("*.*"), arPaths);
		int i(0);
		for(i=0; i<arPaths.GetCount(); i++)
		{
			GetFilePathListIncludingSubFolder(arPaths.GetAt(i), strFileName, arFilePaths);
		}
	}
	else
	{
		while(bFound)
		{
			bFound = finder.FindNextFile();
			if(finder.IsDirectory())
			{
				GetFilePathListIncludingSubFolder(finder.GetFilePath(), strFileName, arFilePaths);
			}
			else if(!finder.IsDots())
			{
			//	TRACE(_T("FOUND %s\r\n"), (LPCTSTR)finder.GetFilePath());
				arFilePaths.Add(finder.GetFilePath());
			}
		}
	}
	finder.Close();
	return;
}

//c:\myhtml\myfile.txt returns the file path "c:\myhtml\myfile.txt". 
//c:\myhtml\myfile.txt returns the file name "myfile.txt". 
CString GetFileName(CString strPathName)
{
	int nPos(strPathName.ReverseFind(_T('\\')));
	if(nPos>=0)
		return strPathName.Right(strPathName.GetLength()-nPos-1);
	return strPathName;
}

//c:\myhtml\myfile.txt returns the file path "c:\myhtml\myfile.txt". 
//c:\myhtml\myfile.txt returns the folder name "c:\myhtml\". 
CString GetFolderName(CString strPathName)
{
	int nPos(strPathName.ReverseFind(_T('\\')));
	if(nPos>=0)
		return strPathName.Left(nPos);
	return strPathName;
}

BOOL ExtractSubString(CString& rString, LPCTSTR lpszFullString, int iSubString, TCHAR chSep)
{
	return AfxExtractSubString(rString, lpszFullString, iSubString, chSep);
}

BOOL ExtractSublong(long& lValue, LPCTSTR lpszFullString, int iSubString, TCHAR chSep)
{
	CString strSub;
	if(AfxExtractSubString(strSub, lpszFullString, iSubString, chSep))
	{
		lValue = (_ttoi((LPCTSTR)strSub));
		return TRUE;
	}
	return FALSE;
}

BOOL ResourceToFile(long lResourceID ,CString strResourceType , CString strFullPath)
{
	HANDLE hResInfo(NULL), hRes(NULL);
	LPSTR lpRes(NULL);

	HINSTANCE hInst(GetModuleHandle(NULL));
	if(!hInst) {return FALSE;}

	hResInfo = ::FindResource( hInst, MAKEINTRESOURCE(lResourceID), strResourceType );
	if(!hResInfo) {return FALSE;}
	hRes     = ::LoadResource( (HMODULE)hInst, (HRSRC)hResInfo );
	if(!hRes) {return FALSE;}

	// 리소스를 위한 전역 메모리를 잠근다. 이 때 리턴되는 값은 메모리상에서의
	// 리소스 시작 포인터이다.
	lpRes = (LPSTR)::LockResource(hRes);
	if(!lpRes) {return FALSE;}

	// 파일을 만들고 리소스의 크기를 구해 내용을 모두 기록한다.
	CFile aFile;
	if(aFile.Open( strFullPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		aFile.Write( (void*)lpRes, SizeofResource( hInst, (HRSRC) hResInfo ) );
		aFile.Close();
		::FreeResource(hRes);
	}
	else
	{
		::FreeResource(hRes);
		return FALSE;
	}
	return TRUE;
}

//서브 폴더에 있는 파일까지 지움..
void DeleteOldFiles(CString strPathName, CString strFileName, int nLeaveDaysAgo, BOOL bDeleteSubFolder, BOOL bDeleteEmptyFolder, CTime tmToday)
{
	if(strPathName.GetLength() <= 0) { return; }
	strPathName += _T("\\");
	strPathName.Replace(_T("\\\\"),_T("\\"));
	strPathName += strFileName;//_T("*.*");

	CTime tmLastWrite;
	CTimeSpan tmSpan( nLeaveDaysAgo, 0, 0, 0);

	CFileFind	finder;
	BOOL bFound = finder.FindFile(strPathName);

	while(bFound)
	{
		Sleep(1);
		bFound = finder.FindNextFile();
		if(!finder.IsDots())
		{
			if(finder.IsDirectory())
			{
				if(bDeleteSubFolder)
				{
					DeleteOldFiles(finder.GetFilePath(), strFileName, nLeaveDaysAgo, bDeleteSubFolder, bDeleteEmptyFolder, tmToday);
				}

				if(bDeleteEmptyFolder)
				{
					CArray<CString> arFileTitles;
					GetFileTitleList(finder.GetFilePath(), _T("*.*"), arFileTitles);
					if(arFileTitles.GetCount()==0)
					{
						BOOL bRet = RemoveDirectory(finder.GetFilePath());
						TRACE(_T("%s %s\r\n"), (bRet?_T("FOLDER DELETED"):_T("FAIL TO FOLDER DELETE")), (LPCTSTR)finder.GetFilePath());
					}
				}
			}
			else if(finder.GetLastWriteTime(tmLastWrite))
			{
				if( CTimeSpan(tmToday-tmLastWrite)> tmSpan )
				{
					BOOL bRet = DeleteFile(finder.GetFilePath());
					TRACE(_T("%s %s\r\n"), (bRet?_T("DELETED"):_T("FAIL TO DELETE")), (LPCTSTR)finder.GetFilePath());
				}
			}
		}
	}
	finder.Close();
	return;
}

#pragma comment(lib, "pdh.lib")
#include <pdh.h>
#include <pdhmsg.h>
long GetCpuUsagePercent()
{
	static HQUERY m_hQuery(NULL);
	static HCOUNTER m_hCounter(NULL);
	static PDH_FMT_COUNTERVALUE m_pdh_value = {0};
	static BOOL m_bStart(FALSE);

    DWORD_PTR pArgs[] = { (DWORD_PTR)L"<collectionname>" };
    DWORD dwErrorCode = PDH_PLA_ERROR_ALREADY_EXISTS;
	PDH_STATUS ret;

	CString strBuf;

	if(!m_bStart)
	{
		// PDH 핸들 얻기
		ret = PdhOpenQuery( NULL, 0, &m_hQuery );
		if(ret != ERROR_SUCCESS)
		{
			strBuf.Format(_T("1[LineNum=%d][ErrorCode=0x%x]"), __LINE__,  ret);
			TRACE(strBuf+_T("\r\n"));
		//	g_logger.WriteLog(_T("PdhOpenQuery() FAILED,[LineNum=%d][ErrorCode=0x%x]"), __LINE__,  ret);
		//	AfxMessageBox(strBuf);
		}

		ret = PdhAddCounter( m_hQuery, _T("\\Processor(_Total)\\% Processor Time"), 1, &m_hCounter );
		if(ret != ERROR_SUCCESS)
		{
			strBuf.Format(_T("2[LineNum=%d][ErrorCode=0x%x]"), __LINE__,  ret);
			TRACE(strBuf+_T("\r\n"));
		//	g_logger.WriteLog(_T("PdhAddCounter() FAILED,[LineNum=%d][ErrorCode=0x%x]"), __LINE__,  ret);
		//	AfxMessageBox(strBuf);
		}

	//	GetProcessTimes();

		if(m_hQuery!=NULL && m_hCounter!=NULL)
		{
			m_bStart = TRUE;
		}
		else
		{
			Sleep(100);
		}
	}

	// 사용률 구하기
	if(m_hQuery!=NULL && m_hCounter!=NULL)
	{
		ret = PdhCollectQueryData( m_hQuery );
		if(ret != ERROR_SUCCESS)
		{
			strBuf.Format(_T("3[LineNum=%d][ErrorCode=0x%x]"), __LINE__,  ret);
			TRACE(strBuf+_T("\r\n"));
		//	g_logger.WriteLog(_T("PdhCollectQueryData() FAILED,[LineNum=%d][ErrorCode=0x%x]"), __LINE__,  ret);
		//	AfxMessageBox(strBuf);
		}

		ret = PdhGetFormattedCounterValue( m_hCounter, PDH_FMT_LONG, NULL, &m_pdh_value );
		if(ret != ERROR_SUCCESS)
		{
			strBuf.Format(_T("4[LineNum=%d][ErrorCode=0x%x]"), __LINE__,  ret);
		//	g_logger.WriteLog(_T("PdhGetFormattedCounterValue() FAILED,[LineNum=%d][ErrorCode=0x%x]"), __LINE__,  ret);
			TRACE(strBuf+_T("\r\n"));
		//	AfxMessageBox(strBuf);
		}
		return m_pdh_value.longValue; // 사용률 값
	}
	return 100;
}

//*******************************************************************/
//! Function Name : GetWindowsVersion
//! Function      : 윈도우 버전을 가져옴.
//! Param         : -
//! Return        : void
//! Create        : , 2016/09/12
//! Comment       : 본 함수는 그리드의 모습이 윈도우 버전마다 다른 
//					경우를 처리하기 위해서 추가됨.
//******************************************************************/
eWinVersion GetWindowsVersion()
{
	OSVERSIONINFO osVersionInfo;
	eWinVersion ret = eNotSupportedVersion;

	ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFO));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osVersionInfo);

	if (osVersionInfo.dwMajorVersion == 5 && osVersionInfo.dwMinorVersion == 1)
	{
		ret = eWindowsXP;
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 0)
	{
		ret = eWindows_Vista;
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 1)
	{
		ret = eWindows_7;
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 2)
	{
		ret = eWindows_8;
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 3)
	{
		ret = eWindows_8_1;
	}
	else if (osVersionInfo.dwMajorVersion == 10 && osVersionInfo.dwMinorVersion == 0)
	{
		ret = eWindows_10;
	}

	if (ret == eNotSupportedVersion)
	{
		CString strBuff;
		strBuff.Format("본 버전은 지원하지 않습니다.\nmajor version=%d\nminor version=%d",
						osVersionInfo.dwMajorVersion,
						osVersionInfo.dwMinorVersion);
		AfxMessageBox(strBuff);
	}
	return ret;
}

CString getCurrencyString(CString currencyWithoutComma)
{
	return getCurrencyString(atol((LPSTR)(LPCSTR)currencyWithoutComma));
}

CString getCurrencyString(long currency)
{
	char parm_buffer[100] = { 0 }; // 0이 100개 이상 붙을 경우가 있는가?
	CString strBuff;
	CString currencyString;
	NUMBERFMT fmt = { 0, 0, 3, ".", ",", 1 };

	strBuff.Format("%f", (double)currency);

	// GetNumberFormat의 인자는 LOCALE_SYSTEM_DEFAULT, 플래그, 문자열 숫자, 문자열 포맷,
	// 변환된 문자열 숫자, 버퍼 사이즈이다.
	if (::GetNumberFormat(NULL, NULL, strBuff, &fmt, parm_buffer, strlen(strBuff)) == 0) 
	{
		currencyString = "N/A";
		AfxMessageBox("GetNumberFormat 호출에 실패했습니다.");
	}
	else {
		currencyString.Format("%s", parm_buffer);
	}
	return currencyString;
}
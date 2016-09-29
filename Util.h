#include "ABotDefine.h"

// 프로그램 버젼 읽는다.
CString GetSystemVersion();
// 정수값을 읽는다.
BOOL ReadFromReg_Int(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault, BOOL &bDefault);
// 문자열을 읽는다.
BOOL ReadFromReg_String(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault, LPTSTR lpRet, DWORD nSize, BOOL &bDefault);
// 정수값을 쓴다.
BOOL WriteToReg_Int(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, UINT nData);
// 문자열을 쓴다.
BOOL WriteToReg_String(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData);

void ReadFromIniFile_Int(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, long nDefault, long &nReturn);
void ReadFromIniFile_Float(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, float fDefault, float &fReturn);
void ReadFromIniFile_String(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, const CString &strDefault, CString &strReturn);
void WriteToIniFile_Int(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, long nValue);
void WriteToIniFile_Float(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, float fValue);
void WriteToIniFile_String(LPCTSTR szPathName, LPCTSTR szSection, LPCTSTR szKeyName, const CString &strValue);

BOOL IsFileExist(LPCTSTR lpPathName, BOOL bIsMsg = FALSE);
BOOL MakeDirectory(CString strPathName);
BOOL MakeFolder(CString strPathName);

CString GetFolderName(CString strPathName);
CString GetFileName(CString strPathName);

void GetFileTitleList(CString strPathName, CString strFileName, OUT CArray<CString> &arFileTitles);
void GetFileNameList(CString strPathName, CString strFileName, OUT CArray<CString> &arFileNames);
void GetFilePathList(CString strPathName, CString strFileName, OUT CArray<CString> &arFilePaths);

void GetFilePathListIncludingSubFolder(CString strPathName, CString strFileName, OUT CArray<CString> &arFilePaths);

void GetDirTitleList(CString aszPathName, CString aszFileName, OUT CArray<CString> &arDirTitles);
void GetDirPathList(CString aszPathName, CString aszFileName, OUT CArray<CString> &arDirPaths);

BOOL ExtractSubString(CString& rString, LPCTSTR lpszFullString, long iSubString, TCHAR chSep = '\n');
BOOL ExtractSublong(long& lValue, LPCTSTR lpszFullString, long iSubString, TCHAR chSep = '\n');

BOOL ResourceToFile(long lResourceID ,CString strResourceType , CString strFullPath);

void DeleteOldFiles(CString strPathName, CString strFileName, long nLeaveDaysAgo, BOOL bDeleteSubFolder = FALSE, BOOL bDeleteEmptyFolder = FALSE, CTime tmToday = CTime(CTime::GetCurrentTime().GetYear(), CTime::GetCurrentTime().GetMonth(), CTime::GetCurrentTime().GetDay(),0,0,0) );

long GetCpuUsagePercent();

eWinVersion GetWindowsVersion();

CString GetCurrencyString(CString currencyWithoutComma);
CString GetCurrencyString(long currency);

long CalcBuyAndSellPrice(long price, double fluctuationRatio, BOOL isKOSDAQ); // 입력한 가격, 등락율
long GetAskingPrice(double price, BOOL isKOSDAQ);

COLORREF GetFGColor(CString strData);
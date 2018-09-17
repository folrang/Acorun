//////////////////////////////////////////////////////////////////////
//				IniFile.h
//							Code by DEDE
//
// [Abstract]
// - INI ������ �а�, ���Ǻ� ������ ���� �ϵ��� �ϴ� ���� Ŭ����
//
// [History]
// * 2009/04/09
// 
// * 2010/11/15
//		- 64bit ������ ���� ����
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <map>

#pragma warning( disable : 4996)

// tstring ����
#ifndef tstring
	#ifdef UNICODE
	#define tstring std::wstring
	#else
	#define tstring std::string
	#endif
#endif

using namespace std;

#ifndef MAP_INI_STRKEY_STRVAL
	typedef map<tstring, tstring> MAP_INI_STRKEY_STRVAL;
	typedef map<tstring, tstring>::const_iterator MAP_INI_STRKEY_STRVAL_CIT;
	typedef map<tstring, tstring>::iterator MAP_INI_STRKEY_STRVAL_IT;
#endif

#ifndef MAP_INI_SECSTION
	typedef map<tstring, MAP_INI_STRKEY_STRVAL* > MAP_INI_SECSTION;
	typedef map<tstring, MAP_INI_STRKEY_STRVAL* >::const_iterator MAP_INI_SECSTION_CIT;
	typedef map<tstring, MAP_INI_STRKEY_STRVAL* >::iterator MAP_INI_SECSTION_IT;
#endif


// ini ���ϳ��� [���Ǹ�] �� �������θ� ����
class CIniFile
{
public:
	CIniFile();
	// ���� ini ������ ���� SectionMap���� ������ �ÿ� ���Ǵ� ������ �Լ�
	// [in] pSectionMapPtr: new ������ ��ü
	// [in] bDestroySectionMap: pSectionMap�� Clear �� Destroy ��ų�� ����
	//			FALSE: Close() �� pSectionMap ���� Clear() �۾��� ���ο��� �߻����� ������, 
	//				�Ҹ��� �Լ������� pSectionMap ��ü�� �������� ����
	CIniFile(MAP_INI_SECSTION* pSectionMapPtr, BOOL bDestroySectionMap);
	~CIniFile(void);

	// Ini ������ ��� �ʱ� ����
	// [retVal] �߰��� ���� ����
	int Open(LPCTSTR szFilePath);
	
	// ���� ������ szFilePath ��η� ���� �ۼ��Ѵ� (�ּ��� ����)
	// [in] szFilePath: ������ ���, NULL �� ��쿡�� ���µ� ��ο� ������
	// [retVal] ���� ����
	BOOL ReWrite(LPCTSTR szFilePath = NULL);
	
	// Open �����̸� Close ȣ��
	void Close();
	
	// �ش� ���� �� Ű�� �ش��ϴ� ���� ��ȯ
	// [retVal] ���� ���, tstring("") ��Ʈ���� ��ȯ
	tstring GetStrVal(LPCTSTR szSectionName, LPCTSTR szKey);
	LPCTSTR GetVal(LPCTSTR szSectionName, LPCTSTR szKey);
	
	// ���Ǹ� ����
	void SetSectionName(LPCTSTR szNewName, LPCTSTR szOldName);
	
	// Ű�̸� ����
	void SetKeyName(LPCTSTR szSectionName, LPCTSTR szNewKeyName, LPCTSTR szOldKeyName);

	// �� ����
	void SetStrVal(LPCTSTR szSectionName, LPCTSTR szKey, LPCTSTR szNewVal);

	LPCTSTR GetPath() { return m_szPath; }
	
	// ���� ���� ��ȯ
	int GetSectionSize();

	// ���ǳ� Ű ���� ��ȯ
	int GetKeySize(LPCTSTR szSectionName);

	// ������ ����, ����, Ű�̸��� �ش��ϴ� Ű ���� ã�Ƽ� ��ȯ
	// [out] szKeyVal: Ű��, "" : ���ų� ������ ��� ��ȯ
	// [in] dwKeyValBufSize: szKeyVal ���� ũ��
	// [retVal] szKeyVal�� ��ȯ
	static LPCTSTR GetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName, LPTSTR szKeyVal, DWORD dwKeyValBufSize);
	static tstring GetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName);
	
	// ������ ����, ����, Ű�̸��� Ű�� ����
	//	- ���� �� ����, Ű�̸� ���� �ڵ� ���� ��
	// [retVal] FALSE: ���� ����
	static BOOL SetIniKeyVal(LPCTSTR szFilePath, LPCTSTR szSectionName, LPCTSTR szKeyName, LPCTSTR szKeyVal);

	static int Convert2Int(const tstring& in)
	{
		return Convert2Int(in.c_str());
	}
	static int Convert2Int(LPCTSTR szIn)
	{
		return _ttoi(szIn);
	}

	static UINT Convert2UInt(LPCTSTR szIn)
	{
		return _tcstoul(szIn, NULL, 10);
	}



	MAP_INI_SECSTION* GetInnerMap() { return m_pSectionMap; }

protected:
	enum
	{
		MAX_SECTIONNAMES_SIZE		= 1024,		// ���� �̸� ����Ʈ ���ڿ��� �ִ� ���� ũ��
		DEFAULT_FILE_SIZE			= 2048,		// ini�� ����Ʈ ���� ������ (���ι���ũ�� �Ҵ�)
		MAX_SECTION_CONTENT_SIZE	= 32768,	// �� ���� ������ �ִ� ���� ũ�� (MSDN GetPrivateProfileSection ����)
		MAX_LINE_SIZE				= 1024		// �� ������ �ִ� ���� ����
	};

	tstring& MakeIniFileContext(tstring& out) const;

	// "��ū1\0��ū1\0....\0��ūn\0\0"
	// [�����̸�] ����Ʈ Ȥ�� Ű&��(key=val) ����Ʈ���� �Ѱ��� �������� ���� �� ��ȯ
	// ��ū ����Ʈ ���ڿ����� ù ��ū�� out�� �����ϰ�, ���� ��ġ�� ��ȯ
	// [in] szSrc: "��ū1\0��ū1\0....\0��ūn\0\0" �� ���� ���� ���ڿ�
	// [out] outToken: ����� ��ū�̸�
	// [retVal] NULL �� ���, ���� ��ū ����
	LPTSTR GetToken(LPTSTR szSrc, LPTSTR outToken) const;

	// "key=val" �� ������ ���ڿ��� key �� val �����Ͽ� ��ȯ
	// key �� val ���ڿ��� Trim ����
	void SpliceKeyVal(const tstring& src, tstring& key, tstring& val) const;

	// str ���ڿ��� �յ� ���� ���� �� ��ȯ
	tstring& Trim(tstring& str) const;

private:
	_TCHAR m_szPath[MAX_PATH +1];
	BOOL m_bDestroySectionMap;
	MAP_INI_SECSTION* m_pSectionMap;	// ��Ű: ���Ǹ�, �ʰ�: ���� ���� (ini key : ini value)
};


/*
int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bSuc;
	tstring szVal;
	int nVal;

	CIniFile Ini;
	bSuc = Ini.Open(_T("D:\\test\\test.ini"));
	if (bSuc)
	{
		szVal = Ini.GetStrVal(_T("FTP_LIST"), _T("FTP_C6_DIR"));
		szVal = Ini.GetStrVal(_T("CMOS_DEFINE"), _T("NUM01"));

		szVal = Ini.GetStrVal(_T("FTP_LIST"), _T("Btn_Diplay_Chk2"));
		nVal = Ini.Convert2Int(szVal);

		Ini.SetSectionName(_T("BUFFER_XXXX"), _T("BUFFER_SIZE_DEFINE"));
		Ini.SetKeyName(_T("BUFFER_XXXX"), _T("HOSTRECV_XXXX"), _T("HOSTRECV_MAX_SIZE"));
		Ini.SetStrVal(_T("BUFFER_XXXX"), _T("HOSTRECV_XXXX"), _T("4444"));

		bSuc = Ini.ReWrite(_T("D:\\test\\test2.ini"));

		Ini.Close();
	}
	
	return 0;
}
*/
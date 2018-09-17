#pragma once
/*
 * BLOWFISH를 사용해서 주어진 문자열을 암호화/복호화 한다.
 * 암호화된 바이너리 데이터를 문자열로 처리가 용이하도록 인코딩(base64/hex)한다.
 *  
 *
 * 작성일 : 2007/4/17
 * 작성자 : 김경현
 * 비고 : 
 *   SONM 프로젝트에서 사용된 EncryptString/DecryptString 루틴을 기반으로 수정했음
 *   Online Activation시 URL 파라미터로 전달되는 라이센스 키를 암호화하기 위해 개발
 *
 */
class CKeyCrypt
{
public:
	CKeyCrypt();
	~CKeyCrypt();

	enum Encoding
	{
		ENCODE_BASE64 = 1,
		ENCODE_HEX
	};

	// PlainText를 BF암호화하고, 그 결과를 인코딩해서 반환한다.
	// [in] PlainText: 암호화할 문자열
	// [in] encoding: 인코딩
	// [retval] LPTSTR: 암호화된 문자열을 반환한다.
	//                  리턴된 문자열은 ReleaseString()을 사용해 해제해야한다.
	//                  오류 발생시 NULL을 반환한다.
	LPTSTR EncryptString(LPCTSTR PlainText, Encoding encoding) const;

	// CipherText를 복호한다.
	// [in] CipherText: 암호화된 문자열
	// [in] encoding: 인코딩
	// [retval] LPTSTR: 복화화된 문자열을 반환한다.
	//                  리턴된 문자열은 ReleaseString()을 사용해 해제해야한다.
	//                  오류 발생시 NULL을 반환한다.
	LPTSTR DecryptString(LPCTSTR CipherText, Encoding encoding) const;	

	// EncryptString/DecryptString에서 반환된 문자열을 해제한다.
	// [in] Text: 해제할 문자열
	void ReleaseString(LPCTSTR Text) const;

protected:
	LPTSTR EncryptStringByBase64(LPCTSTR PlainText) const;
	LPTSTR DecryptStringByBase64(LPCTSTR CipherText) const;
	LPTSTR EncryptStringByHex(LPCTSTR PlainText) const;
	LPTSTR DecryptStringByHex(LPCTSTR CipherText) const;

	BYTE HexToBin(TCHAR a) const;
	bool base64_encode(const unsigned char *in,  unsigned long len, unsigned char *out, unsigned long *outlen) const;
	bool base64_decode(const unsigned char *in,  unsigned long len, unsigned char *out, unsigned long *outlen) const;
};
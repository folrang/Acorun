#pragma once
/*
 * BLOWFISH�� ����ؼ� �־��� ���ڿ��� ��ȣȭ/��ȣȭ �Ѵ�.
 * ��ȣȭ�� ���̳ʸ� �����͸� ���ڿ��� ó���� �����ϵ��� ���ڵ�(base64/hex)�Ѵ�.
 *  
 *
 * �ۼ��� : 2007/4/17
 * �ۼ��� : �����
 * ��� : 
 *   SONM ������Ʈ���� ���� EncryptString/DecryptString ��ƾ�� ������� ��������
 *   Online Activation�� URL �Ķ���ͷ� ���޵Ǵ� ���̼��� Ű�� ��ȣȭ�ϱ� ���� ����
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

	// PlainText�� BF��ȣȭ�ϰ�, �� ����� ���ڵ��ؼ� ��ȯ�Ѵ�.
	// [in] PlainText: ��ȣȭ�� ���ڿ�
	// [in] encoding: ���ڵ�
	// [retval] LPTSTR: ��ȣȭ�� ���ڿ��� ��ȯ�Ѵ�.
	//                  ���ϵ� ���ڿ��� ReleaseString()�� ����� �����ؾ��Ѵ�.
	//                  ���� �߻��� NULL�� ��ȯ�Ѵ�.
	LPTSTR EncryptString(LPCTSTR PlainText, Encoding encoding) const;

	// CipherText�� ��ȣ�Ѵ�.
	// [in] CipherText: ��ȣȭ�� ���ڿ�
	// [in] encoding: ���ڵ�
	// [retval] LPTSTR: ��ȭȭ�� ���ڿ��� ��ȯ�Ѵ�.
	//                  ���ϵ� ���ڿ��� ReleaseString()�� ����� �����ؾ��Ѵ�.
	//                  ���� �߻��� NULL�� ��ȯ�Ѵ�.
	LPTSTR DecryptString(LPCTSTR CipherText, Encoding encoding) const;	

	// EncryptString/DecryptString���� ��ȯ�� ���ڿ��� �����Ѵ�.
	// [in] Text: ������ ���ڿ�
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
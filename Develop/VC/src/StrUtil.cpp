#include "StdAfx.h"
#include "StrUtil.h"
#include <algorithm>

namespace StrUtil
{

list<tstring>& Split1(const tstring& src, const tstring& delimeters, BOOL bUnique, BOOL bNullInsert, int nMaxFindCount, list<tstring>& out)
{
	out.clear();

	if (nMaxFindCount == 0)
		return out;

	if( delimeters.empty() )
	{
		out.push_back(src);
		return out;
	}

	
	BOOL bFound = FALSE;
	tstring::size_type sPos, nPos;
	tstring item;
	list<tstring>::iterator find_it;

	sPos = 0;

	nPos = src.find_first_of(delimeters, sPos);
	while (nPos != tstring::npos)
	{
		bFound = TRUE;
		item = src.substr(sPos, nPos-sPos);
		if (bUnique)
		{
			find_it = find(out.begin(), out.end(), item);
			if (find_it != out.end())
				bFound = FALSE;
		}
		
		if (bFound && !bNullInsert && item.empty())
			bFound = FALSE;
		
		if (bFound)
		{
			out.push_back(item);
			if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
				return out;
		}

		sPos = nPos + 1;
		nPos = src.find_first_of(delimeters, sPos);
	}

	if( sPos < src.size() )
	{
		item = src.substr(sPos, src.size() - sPos);
		if(bUnique)
		{
			find_it = find(out.begin(), out.end(), item);
			if( find_it == out.end())
			{
				out.push_back(item);
				if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
					return out;
			}
		}
		else
		{
			out.push_back(item);
			if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
				return out;
		}
	}

	return out;
}

vector<tstring>& Split1(const tstring& src, const tstring& delimeters, BOOL bUnique, BOOL bNullInsert, int nMaxFindCount, vector<tstring>& out)
{
	out.clear();

	if (nMaxFindCount == 0)
		return out;

	if( delimeters.empty() )
	{
		out.push_back(src);
		return out;
	}

	
	BOOL bFound = FALSE;
	tstring::size_type sPos, nPos;
	tstring item;
	vector<tstring>::iterator find_it;

	sPos = 0;

	nPos = src.find_first_of(delimeters, sPos);
	while (nPos != tstring::npos)
	{
		bFound = TRUE;
		item = src.substr(sPos, nPos-sPos);
		if (bUnique)
		{
			find_it = find(out.begin(), out.end(), item);
			if (find_it != out.end())
				bFound = FALSE;
		}
		
		if (bFound && !bNullInsert && item.empty())
			bFound = FALSE;
		
		if (bFound)
		{
			out.push_back(item);
			if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
				return out;
		}

		sPos = nPos + 1;
		nPos = src.find_first_of(delimeters, sPos);
	}

	if( sPos < src.size() )
	{
		item = src.substr(sPos, src.size() - sPos);
		if(bUnique)
		{
			find_it = find(out.begin(), out.end(), item);
			if( find_it == out.end())
			{
				out.push_back(item);
				if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
					return out;
			}
		}
		else
		{
			out.push_back(item);
			if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
				return out;
		}
	}

	return out;
}

list<tstring>& Split2(const tstring& src, const tstring& matchStr, BOOL bUnique, BOOL bNullInsert, int nMaxFindCount, list<tstring>& out)
{
	out.clear();
	
	if (nMaxFindCount == 0)
		return out;

	BOOL bFound = FALSE;
	size_t nMatchStr = matchStr.length();
	tstring::size_type sPos, nPos;
	tstring item;
	list<tstring>::iterator find_it;

	sPos = 0;

	nPos = src.find(matchStr, sPos);
	while (nPos != tstring::npos)
	{
		bFound = TRUE;
		item = src.substr(sPos, nPos-sPos);
		if (bUnique)
		{
			find_it = find(out.begin(), out.end(), item);
			if (find_it != out.end())
				bFound = FALSE;
		}
		
		if (bFound && !bNullInsert && item.empty())
			bFound = FALSE;
		
		if (bFound)
		{
			out.push_back(item);
			if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
				return out;
		}

		sPos = nPos + nMatchStr;
		nPos = src.find(matchStr, sPos);
	}

	if( sPos < src.size() )
	{
		item = src.substr(sPos, src.size() - sPos);
		if(bUnique)
		{
			find_it = find(out.begin(), out.end(), item);
			if( find_it == out.end())
			{
				out.push_back(item);
				if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
					return out;
			}
		}
		else
		{
			out.push_back(item);
			if (nMaxFindCount != ALL_FIND_COUNT && out.size() >= (size_t)nMaxFindCount)
				return out;
		}
	}

	return out;
}

tstring& Replace(tstring& src, LPCTSTR find, LPCTSTR replacewith, BOOL bAll /*= FALSE*/)
{
	size_t nFind = _tcslen(find);
	size_t nReplaceWith = _tcslen(replacewith);
	tstring::size_type sPos = 0, nPos = tstring::npos;

	while ((nPos = src.find(find, sPos)) != tstring::npos)
	{
		src.replace(nPos, nFind, replacewith);
		if (nReplaceWith > 0)
			sPos = nPos + nReplaceWith;
		else
			sPos = nPos;

		if (!bAll)
			break;
	}

	return src;
}

tstring& Replace(const tstring& src, LPCTSTR find, LPCTSTR replacewith, tstring& out, BOOL bAll /*= FALSE*/)
{
	out = src;
	return Replace(out, find, replacewith, bAll);
}

tstring& Replace1(tstring& src, LPCTSTR find, LPCTSTR replacewith, BOOL bAll /*= FALSE*/)
{
	size_t nReplaceWith = _tcslen(replacewith);
	tstring::size_type sPos = 0, nPos = tstring::npos;

	while ((nPos = src.find_first_of(find, sPos)) != tstring::npos)
	{
		src.replace(nPos, 1, replacewith);
		if (nReplaceWith > 0)
			sPos = nPos + nReplaceWith;
		else
			sPos = nPos;

		if (!bAll)
			break;
	}

	return src;
}

tstring& Trim(tstring& str, TCHAR removeChar /*= ' '*/)
{
	if (str.empty())
		return str;
	
	tstring::size_type sPos, ePos;

	if ((sPos = str.find_first_not_of(removeChar)) == tstring::npos)
	{
		// str 전체가 removeChar 인 경우, "" 반환
		str.clear();
		return str;
	}
	
	if ((ePos = str.find_last_not_of(removeChar)) == tstring::npos)
		ePos = str.length();

	return (str = str.substr(sPos, ePos-sPos+1));
}

// "key=val" 로 구성된 문자열을 key 와 val 추출하여 반환
void SpliceKeyVal(const tstring& src, tstring& key, tstring& val, TCHAR aDelemiter)
{
	key.clear(); val.clear();
	
	tstring::size_type pPos; 

	pPos = src.find_first_of(aDelemiter);
	if (pPos != tstring::npos)
	{
		key = src.substr(0, pPos);
		val = src.substr(pPos+1);
	}
	else
	{
		key = src;
	}
	key = Trim(key);
	val = Trim(val);
}

/*
// [out] out: 포맷 문자열, "2.1MByte"
string& StrFormatByteSize(UINT64 nByte, tstring& out)
{
	TCHAR szOut[256];

	if (nByte > 0)
	{
		StrFormatByteSize64(nByte, szOut, _countof(szOut));
		out = szOut;

		Replace(out, _T("바이트"), _T("Byte"), FALSE);
	}
	else
		out = _T("0Byte");	

	return out;
}
*/

tstring& FormatString(tstring& out, LPCTSTR fmt, ...)
{
	out.clear();

	if (!fmt) 
		return out;

	std::vector<TCHAR> buff;
	size_t size = _tcslen(fmt) * 2;
	buff.resize(size);
	va_list ap;
	va_start(ap, fmt);
	while (true) 
	{
		int ret = _vsntprintf_s(buff.data(), size, _TRUNCATE, fmt, ap);
		if (ret != -1)
			break;
		else 
		{
			size *= 2;
			buff.resize(size);
		}
	}
	va_end(ap);
	return (out = buff.data());
}

} // namespace StrUtil
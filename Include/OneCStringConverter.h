#include <cstdint>
#include "AddInDefBase.h"
#pragma once
class OneCStringConverter
{
public:
	static uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0)
	{
		if (!len)
			len = getLenShortWcharStr(Source) + 1;

		if (!*Dest)
			* Dest = new wchar_t[len];

		wchar_t* tmpWChar = *Dest;
		WCHAR_T* tmpShort = (WCHAR_T*)Source;
		uint32_t res = 0;

		::memset(*Dest, 0, len * sizeof(wchar_t));
		do
		{
			*tmpWChar++ = (wchar_t)* tmpShort++;
			++res;
		} while (len-- && *tmpShort);

		return res;
	}
	
	static uint32_t getLenShortWcharStr(const WCHAR_T* Source)
	{
		uint32_t res = 0;
		WCHAR_T* tmpShort = (WCHAR_T*)Source;

		while (*tmpShort++)
			++res;

		return res;
	}

	static uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0)
	{
		if (!len)
			len = ::wcslen(Source) + 1;

		if (!*Dest)
			* Dest = new WCHAR_T[len];

		WCHAR_T* tmpShort = *Dest;
		wchar_t* tmpWChar = (wchar_t*)Source;
		uint32_t res = 0;

		::memset(*Dest, 0, len * sizeof(WCHAR_T));
		do
		{
			*tmpShort++ = (WCHAR_T)* tmpWChar++;
			++res;
		} while (len-- && *tmpWChar);

		return res;
	}
};


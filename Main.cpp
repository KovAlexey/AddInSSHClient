
#include "stdafx.h"

#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#endif

#include <iostream>
#include <array>
#include <wchar.h>
#include <string>
#include "CAddInSSH.h"

class WcharWrapper
{
public:
#ifdef LINUX_OR_MACOS
	WcharWrapper(const WCHAR_T* str);
#endif
	WcharWrapper(const wchar_t* str);
	~WcharWrapper();
#ifdef LINUX_OR_MACOS
	operator const WCHAR_T* () { return m_str_WCHAR; }
	operator WCHAR_T* () { return m_str_WCHAR; }
#endif
	operator const wchar_t* () { return m_str_wchar; }
	operator wchar_t* () { return m_str_wchar; }
private:
	WcharWrapper& operator = (const WcharWrapper& other) { return *this; }
	WcharWrapper(const WcharWrapper& other) { }
private:
#ifdef LINUX_OR_MACOS
	WCHAR_T* m_str_WCHAR;
#endif
	wchar_t* m_str_wchar;
};


static const wchar_t g_kClassNames[] = L"CAddInNativeSSH"; 
static WcharWrapper s_names(g_kClassNames);


//---------------------------------------------------------------------------//
long GetClassObject(const wchar_t* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface= new CAddInNative();
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
	return s_names;
}


//---------------------------------------------------------------------------//

#ifdef LINUX_OR_MACOS
WcharWrapper::WcharWrapper(const WCHAR_T* str) : m_str_WCHAR(NULL),
m_str_wchar(NULL)
{
	if (str)
	{
		int len = getLenShortWcharStr(str);
		m_str_WCHAR = new WCHAR_T[len + 1];
		memset(m_str_WCHAR, 0, sizeof(WCHAR_T) * (len + 1));
		memcpy(m_str_WCHAR, str, sizeof(WCHAR_T) * len);
		CAddInNative::convFromShortWchar(&m_str_wchar, m_str_WCHAR);
	}
}
#endif
//---------------------------------------------------------------------------//
WcharWrapper::WcharWrapper(const wchar_t* str) :
#ifdef LINUX_OR_MACOS
	m_str_WCHAR(NULL),
#endif 
	m_str_wchar(NULL)
{
	if (str)
	{
		const size_t len = wcslen(str);
		m_str_wchar = new wchar_t[len + 1];
		memset(m_str_wchar, 0, sizeof(wchar_t) * (len + 1));
		memcpy(m_str_wchar, str, sizeof(wchar_t) * len);
#ifdef LINUX_OR_MACOS
		::convToShortWchar(&m_str_WCHAR, m_str_wchar);
#endif
	}

}
//---------------------------------------------------------------------------//
WcharWrapper::~WcharWrapper()
{
#ifdef LINUX_OR_MACOS
	if (m_str_WCHAR)
	{
		delete[] m_str_WCHAR;
		m_str_WCHAR = NULL;
	}
#endif
	if (m_str_wchar)
	{
		delete[] m_str_wchar;
		m_str_wchar = NULL;
	}
}
//---------------------------------------------------------------------------//

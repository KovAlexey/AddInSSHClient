
#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#ifndef __linux__
#include <wtypes.h>
#endif //__linux__

#include <map>
#include "CAddInSSH.h"


class Prop {
	bool Readable;
	bool Writable;

	virtual void GetPropVal(tVariant* propValue);
	virtual void SetPropVal(tVariant* propValue);
};




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

#endif //__ADDINNATIVE_H__

#include "CAddInSSH.h"


//---------------------------------------------------------------------------//
//CAddInNative
CAddInNative::CAddInNative()
{
	
}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{
	return true;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{
	return 2000;
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{
	const wchar_t* wsExtennsion = L"AddInSSH";
	const size_t size = wcslen(wsExtennsion) + 1;

	if (p_iMemory)
	{
		if (p_iMemory->AllocMemory((void**)wsExtensionName, (unsigned)sizeof(WCHAR_T) * size))
		{
			convToShortWchar(wsExtensionName, wsExtennsion, size);
		}
		return true;
	}
	return false;
	
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{
	return props.size();
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{
	wchar_t* propName = 0;
	convFromShortWchar(&propName, wsPropName);
	int index = -1;
	for (int i = 0; i < props.size(); i++)
	{
		wchar_t* currPropName = props[i].getName();
		if (!wcscmp(propName, currPropName))
		{
			index = i;
			break;
		}
	}

	return index;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{
	Prop prop = props[lPropNum];
	WCHAR_T* wsPropName = 0;

	wchar_t* currentName = prop.getName();
	size_t lenght = wcslen(currentName) + 1;

	if (p_iMemory->AllocMemory((void**)& wsPropName, lenght * sizeof(WCHAR_T)))
		convToShortWchar(&wsPropName, currentName, lenght);

	return wsPropName;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{
	return props[lPropNum].getPropVal(pvarPropVal, p_iMemory);
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{
	return props[lPropNum].setPropVal(varPropVal);
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{
	return props[lPropNum].isReadable();
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
	return props[lPropNum].isWritable();
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{
	return eLastMethod;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{
	return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum,
	const long lMethodAlias)
{
	return 0;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{
	return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
	tVariant* pvarParamDefValue)
{
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
	tVariant* paParams, const long lSizeArray)
{
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
	tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
	return false;
}
//---------------------------------------------------------------------------//
void CAddInNative::SetLocale(const WCHAR_T* loc)
{
#ifndef __linux__
	_wsetlocale(LC_ALL, loc);
#else
	int size = 0;
	char* mbstr = 0;
	wchar_t* tmpLoc = 0;
	convFromShortWchar(&tmpLoc, loc);
	size = wcstombs(0, tmpLoc, 0) + 1;
	mbstr = new char[size];

	if (!mbstr)
	{
		delete[] tmpLoc;
		return;
	}

	memset(mbstr, 0, size);
	size = wcstombs(mbstr, tmpLoc, wcslen(tmpLoc));
	setlocale(LC_ALL, mbstr);
	delete[] tmpLoc;
	delete[] mbstr;
#endif
}
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
	p_iMemory = (IMemoryManager*)mem;
	return p_iMemory != 0;
}

//---------------------------------------------------------------------------//
uint32_t CAddInNative::convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
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
//---------------------------------------------------------------------------//
uint32_t CAddInNative::convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
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
//---------------------------------------------------------------------------//
uint32_t CAddInNative::getLenShortWcharStr(const WCHAR_T* Source)
{
	uint32_t res = 0;
	WCHAR_T* tmpShort = (WCHAR_T*)Source;

	while (*tmpShort++)
		++res;

	return res;
}

bool Prop::getPropVal(tVariant* varPropVal, IMemoryManager* iMemory)
{
	switch (type)
	{
	case 1:
	{
		bool _value = false;
		memcpy(&_value, value, size);
		TV_VT(varPropVal) = VTYPE_BOOL;
		TV_BOOL(varPropVal) = _value;
		break;
	}
	case 2:
	{
		
		TV_VT(varPropVal) = VTYPE_PWSTR;
		int lenght = wcslen((wchar_t*)value) + 1;
		WCHAR_T* _value = NULL;

		iMemory->AllocMemory((void**)&_value, lenght * sizeof(WCHAR_T));
		CAddInNative::convToShortWchar(&_value, (wchar_t*)value);

		varPropVal->pwstrVal = _value;
		varPropVal->wstrLen = lenght - 1;
		return true;
	}
	case 3:
	{
		int _value = false;
		memcpy(&_value, value, size);
		TV_VT(varPropVal) = VTYPE_I4;
		TV_I4(varPropVal) = _value;
		break;
	}
	default:
		return false;
	}
	return true;
}

//TODO: у свойств должно быть описание допустимых типов. Убрать утечку памяти при смене типов


bool Prop::setPropVal(tVariant* propVal)
{
	switch (TV_VT(propVal))
	{
	case VTYPE_BOOL:
	{
		//if (TV_VT(propVal) != VTYPE_BOOL)
			//return false;
		const bool _value = TV_BOOL(propVal);
		memcpy(value, &_value, size);
		type = 1; //TODO: удалить это говно
		break;
	}
	case VTYPE_PWSTR:
	{
		if (TV_VT(propVal) != VTYPE_PWSTR)
			return false;

		const WCHAR_T* _value = propVal->pwstrVal;
		delete[] value;
		value = malloc(sizeof(wchar_t) * propVal->strLen);
		CAddInNative::convFromShortWchar((wchar_t**)&value, _value);
		type = 2; //TODO: удалить это говно

		break;
	}
	case VTYPE_I4:
	{
		if (TV_VT(propVal) != VTYPE_I4)
			return false;
		const int32_t _value = TV_I4(propVal);
		memcpy(value, &_value, size);
		type = 3; //TODO: удалить это говно
		break;
	}
	default:
		void* _value = 0;
		return false;
	}
	return true;
}

#include "CAddInSSH.h"
#include <locale>




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
			OneCStringConverter::convToShortWchar(wsExtensionName, wsExtennsion, size);
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
	OneCStringConverter::convFromShortWchar(&propName, wsPropName);
	int index = w_FindProp(propName);

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
		OneCStringConverter::convToShortWchar(&wsPropName, currentName, lenght);

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
	long methonNum = -1;
	wchar_t* methodName = 0;
	OneCStringConverter::convFromShortWchar(&methodName, wsMethodName);

	for (int i = 0; i < eLastMethod; i++)
	{
		const wchar_t* currMethodName = methodNames[i];
		if (!wcscmp(methodName, currMethodName))
		{
			methonNum = i;
			break;
		}
	}

	return methonNum;
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
	long paramsCount = 0;
	switch (lMethodNum)
	{
	case methodInitialize:
	case methodConnect:
		paramsCount = 0;
		break;
	}
	return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
	tVariant* pvarParamDefValue)
{
	bool ret = false;
	
	return ret;
}
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{
	bool ret = false;
	switch (lMethodNum)
	{
	case methodInitialize:
	case  methodConnect:
		ret = true;
		break;
	}
	return ret;
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
	bool ret = false;
	switch (lMethodNum)
	{
	case methodInitialize:
	{
		int32_t result = initializeSSH();
		TV_VT(pvarRetValue) = VTYPE_I4;
		TV_I4(pvarRetValue) = result;
		ret = true;
		break;
	}
	case methodConnect:
	{
		int32_t result = connectToSSH();
		TV_VT(pvarRetValue) = VTYPE_I4;
		TV_I4(pvarRetValue) = result;
		ret = true;
		break;
	}
	}
	return true;
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

long CAddInNative::w_FindProp(const wchar_t* propName)
{
	long index = -1;
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
bool CAddInNative::setMemManager(void* mem)
{
	p_iMemory = (IMemoryManager*)mem;
	return p_iMemory != 0;
}



int32_t CAddInNative::initializeSSH()
{
	WSADATA wsadata;
	int32_t err = 0;
	err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if (err != 0)
		return err;

	err = libssh2_init(0);

	return 0;
}

std::string utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

int32_t CAddInNative::connectToSSH()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sockaddr;
	ZeroMemory(&sockaddr, sizeof(sockaddr));
	
	tVariant propValue;
	const int* port = (int*)props[w_FindProp(L"Порт")].getValue();

	//TODO: Обернуть все это в адекватную функцию
	//std::wstring* temp_string = (std::wstring*)props[w_FindProp(L"Адрес")].getValue();
	std::wstring* temp_string = static_cast<std::wstring*>(props[w_FindProp(L"Адрес")].getValue());
	const wchar_t* w_inet_addr = temp_string->c_str();
	int lenght = wcslen(w_inet_addr) + 1;
	char* init_addr_char = new char[lenght];
	wcstombs(init_addr_char, w_inet_addr, lenght);

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(*port);
	sockaddr.sin_addr.s_addr = inet_addr(init_addr_char);

	delete init_addr_char;

	int ret = connect(sock, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in));
	
	if (ret != 0)
		return WSAGetLastError();

	session = libssh2_session_init();
	if (libssh2_session_handshake(session, sock))
	{
		return -1;
	}

	

	const char* fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

	temp_string = (std::wstring*)props[w_FindProp(L"Логин")].getValue();
	const wchar_t* w_user = temp_string->c_str();
	lenght = wcslen(w_user) + 1;
	
	

	int size = WideCharToMultiByte(CP_UTF8, 0, w_user, temp_string->length() + 1, NULL, 0 , NULL, NULL);
	char* user = new char[size];
	WideCharToMultiByte(CP_UTF8, 0, w_user, temp_string->length() + 1, user, size, NULL, NULL);

	temp_string = (std::wstring*)props[w_FindProp(L"Пароль")].getValue();
	const wchar_t* w_pass = temp_string->c_str();
	lenght = wcslen(w_pass) + 1;
	char* pass = new char[lenght];
	wcstombs(pass, w_pass, lenght);
	
	
	ret = libssh2_userauth_password(session, user, pass);
	if (ret)
		return ret;

	delete user;
	delete pass;

	return ret;
}




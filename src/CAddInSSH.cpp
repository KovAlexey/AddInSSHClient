#include <CAddInSSH.h>
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
	p_iConnect = (IAddInDefBase*)pConnection;

	if (p_iConnect)
	{
		SSHClass* ssh = new SSHClass(p_iConnect);
		sshClass = make_shared<SSHClass>(*ssh);

		p_iConnect->SetEventBufferDepth(32);
	}
	
	return p_iConnect != NULL;
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
	const wstring methodName = OneCStringConverter::convFromShortWcharString(wsMethodName);

	for (int i = 0; i < eLastMethod; i++)
	{
		const wstring currMethodName = methodNames[i];
		if (currMethodName == methodName)
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
	const wstring method_name = methodNames[lMethodNum];
	WCHAR_T* _value;

	p_iMemory->AllocMemory((void**)&_value, (method_name.length() + 1) * sizeof(WCHAR_T));
	OneCStringConverter::convToShortWchar(&_value, method_name.c_str());
	
	return _value;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{
	long paramsCount = 0;
	switch (lMethodNum)
	{
	case methodConnect:
	case methodAutorization:
		paramsCount = 2;
		break;
	case methodSend:
	case methodRequestPTY:
		paramsCount = 1;
		break;
	default:
		paramsCount = 0;
	}
	return paramsCount;
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
	default:
		ret = false;
	}
	return ret;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
	tVariant* paParams, const long lSizeArray)
{
	bool ret = true;
	switch (lMethodNum)
	{
	case methodConnect:
	{
		if ((lSizeArray != 2) || TV_VT(&paParams[0]) != VTYPE_PWSTR || TV_VT(&paParams[1]) != VTYPE_I4)
		{
			ret = false;
			break;
		}

		std::wstring address(paParams[0].pwstrVal);
		int port = paParams[1].intVal;
		sshClass->StartConnect(address, port);

		break;
	}
	case methodAutorization:
	{
		if ((lSizeArray != 2) || TV_VT(&paParams[0]) != VTYPE_PWSTR || TV_VT(&paParams[1]) != VTYPE_PWSTR)
		{
			ret = false;
			break;
		}
		if (!sshClass->isConnected())
		{
			ret = false;
			p_iConnect->ExternalEvent(L"Log", L"Error", L"Подключение отсутсвует");
			break;
		}

		wstring login(paParams[0].pwstrVal);
		wstring password(paParams[1].pwstrVal);
		sshClass->StartAutorization(login, password);

		break;
	}
	case methodSend:
	{
		if ((lSizeArray != 1) || TV_VT(&paParams[0]) != VTYPE_PWSTR)
		{
			ret = false;
			break;
		}

		wstring message(paParams[0].pwstrVal);
		sshClass->SendMessageSSH(message);

		break;
	}
	case methodDisconnect:
	{
		sshClass->Disconnect();
		break;
	}
	case methodRequestPTY:
	{
		if ((lSizeArray != 1) || TV_VT(&paParams[0]) != VTYPE_PWSTR)
		{
			ret = false;
			break;
		}

		wstring pty_name(paParams[0].pwstrVal);
		sshClass->request_pty(pty_name);

		break;
	}
	default:
		ret = false;
	}
	return ret;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
	tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
	bool ret = true;
	switch (lMethodNum)
	{
	default:
		ret = false;
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





std::string utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}




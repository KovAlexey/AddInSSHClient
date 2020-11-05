﻿#include <array>
#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"
//#include <libssh2.h>
#include "SSHClass.h"
#include "Prop.h"
#include "OneCStringConverter.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// class CAddInNative
class CAddInNative : public IComponentBase
{
public:
	
	enum Methods
	{
		methodConnect = 0,
		methodAutorization,
		methodSend,
		methodDisconnect,
		methodRequestPTY,
		eLastMethod      // Always last
	};

	wstring methodNames[eLastMethod] = {
		L"Подключиться",
		L"Авторизация",
		L"Отправить",
		L"Отключиться",
		L"ЗапроситьТерминал"
	};

	CAddInNative(void);
	virtual ~CAddInNative();
	// IInitDoneBase
	virtual bool ADDIN_API Init(void*);
	virtual bool ADDIN_API setMemManager(void* mem);
	virtual long ADDIN_API GetInfo();
	virtual void ADDIN_API Done();
	// ILanguageExtenderBase
	virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** wsLanguageExt);
	virtual long ADDIN_API GetNProps();
	virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
	virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
	virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
	virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal);
	virtual bool ADDIN_API IsPropReadable(const long lPropNum);
	virtual bool ADDIN_API IsPropWritable(const long lPropNum);
	virtual long ADDIN_API GetNMethods();
	virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
	virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum,
		const long lMethodAlias);
	virtual long ADDIN_API GetNParams(const long lMethodNum);
	virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum,
		tVariant* pvarParamDefValue);
	virtual bool ADDIN_API HasRetVal(const long lMethodNum);
	virtual bool ADDIN_API CallAsProc(const long lMethodNum,
		tVariant* paParams, const long lSizeArray);
	virtual bool ADDIN_API CallAsFunc(const long lMethodNum,
		tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
	operator IComponentBase* () { return (IComponentBase*)this; }
	// LocaleBase
	virtual void ADDIN_API SetLocale(const WCHAR_T* loc);

	long w_FindProp(const wchar_t* propName);

private:
	// Attributes
	IMemoryManager* p_iMemory;
	IAddInDefBase* p_iConnect;
	std::array<Prop, 4> props = {
		Prop(L"Адрес", true, true, propClassWString),
		Prop(L"Порт", true, true, propClassInt4),
		Prop(L"Логин", true, true, propClassWString),
		Prop(L"Пароль", true, true, propClassWString)
	};
	
	shared_ptr<SSHClass> sshClass = NULL;
	//void Connect(wstring* login,)
};



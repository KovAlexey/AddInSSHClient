#include <array>
#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"
#include <libssh2.h>


class Prop {
public:
	Prop(wchar_t* _name, bool _readable, bool _writable, uint8_t _type)
	{
		name = _name;
		readable = _readable;
		writable = _writable;
		type = _type;


		switch (type)
		{
		case 1:
		{
			const bool _value = false;
			size = sizeof(bool);
			value = malloc(size);
			memcpy(value, &_value, size);
			break;
		}
		case 2:
		{
			wchar_t* _value = L"";
			size = sizeof(wchar_t) * (wcslen(_value) + 1);
			value = malloc(size);
			memcpy(value, _value, size);
			break;
		}
		case 3:
		{
			int _value = 0;
			size = sizeof(int);
			value = malloc(size);
			memcpy(value, &_value, size);
			break;
		}
		}
	}

	~Prop()
	{
		//delete value;
	}

	bool getPropVal(tVariant * varPropVal, IMemoryManager * iMemory);

	bool setPropVal(tVariant * propVal);

	void* getValue();

	bool isReadable()
	{
		return readable;
	}

	bool isWritable()
	{
		return writable;
	}

	uint8_t getPropType()
	{
		return type;
	}

	wchar_t* getName()
	{
		return name;
	}
private:
	uint8_t type = 0;
	bool readable = false;
	bool writable = false;
	wchar_t* name;
	void* value;
	int size;
};

class PropBool : public Prop {
public:
	PropBool(wchar_t* _name, bool _readable, bool _writable) :Prop(_name, _readable, _writable, 1)
	{
	}
};

class PropString : public Prop {
public:
	PropString(wchar_t* _name, bool _readable, bool _writable) :Prop(_name, _readable, _writable, 2)
	{
	}
};




///////////////////////////////////////////////////////////////////////////////
// class CAddInNative
class CAddInNative : public IComponentBase
{
public:
	
	enum Methods
	{
		methodInitialize = 0,
		methodConnect,
		eLastMethod      // Always last
	};

	wchar_t* methodNames[eLastMethod] = {
		L"Инициализация",
		L"Подключиться"
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

	static uint32_t getLenShortWcharStr(const WCHAR_T* Source);
	static uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
	static uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
private:
	// Attributes
	IMemoryManager* p_iMemory;
	std::array<Prop, 4> props = {
		Prop(L"Адрес", true, true, 2),
		Prop(L"Порт", true, true, 3),
		Prop(L"Логин", true, true, 2),
		Prop(L"Пароль", true, true, 2)
	};
	int sock;

	int32_t initializeSSH();
	int32_t connectToSSH();
	LIBSSH2_SESSION* session;

};



#pragma once
#include <stdint.h>
#include <string>
#include "AddInDefBase.h"
#include "IMemoryManager.h"
#include "OneCStringConverter.h"

enum PropClass
{
	propClassBool = 1,
	propClassWString,
	propClassInt4
};

class Prop {
public:
	Prop(wchar_t* _name, bool _readable, bool _writable, uint8_t _type);
	~Prop();

	bool getPropVal(tVariant * varPropVal, IMemoryManager * iMemory);
	bool setPropVal(tVariant * propVal);
	void* getValue();

	bool isReadable()  noexcept
	{
		return readable;
	}

	bool isWritable() noexcept
	{
		return writable;
	}

	uint8_t getPropType() noexcept
	{
		return type;
	}

	wchar_t* getName() noexcept
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

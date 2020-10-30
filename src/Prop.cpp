#include "Prop.h"



Prop::Prop(wchar_t* _name, bool _readable, bool _writable, uint8_t _type)
{
	name = _name;
	readable = _readable;
	writable = _writable;
	type = _type;


	switch (type)
	{
	case propClassBool:
	{
		const bool _value = false;
		size = sizeof(bool);
		value = malloc(size);
		memcpy(value, &_value, size);
		break;
	}
	case propClassWString:
	{
		const wchar_t* _value = L"";
		size = sizeof(wchar_t) * (wcslen(_value) + 1);
		value = malloc(size);
		memcpy(value, _value, size);
		break;
	}
	case propClassInt4:
	{
		int _value = 0;
		size = sizeof(int);
		value = malloc(size);
		memcpy(value, &_value, size);
		break;
	}
	}
}


Prop::~Prop()
{
}

bool Prop::getPropVal(tVariant* varPropVal, IMemoryManager* iMemory)
{
	switch (type)
	{
	case propClassBool:
	{
		bool _value = false;
		memcpy(&_value, value, size);
		TV_VT(varPropVal) = VTYPE_BOOL;
		TV_BOOL(varPropVal) = _value;
		break;
	}
	case propClassWString:
	{

		TV_VT(varPropVal) = VTYPE_PWSTR;
		int lenght = wcslen((wchar_t*)value) + 1;


		std::wstring* p_value = (std::wstring*)value;
		WCHAR_T* _value;

		iMemory->AllocMemory((void**)& _value, lenght * sizeof(WCHAR_T));
		OneCStringConverter::convToShortWchar(&_value, p_value->c_str());

		varPropVal->pwstrVal = _value;
		varPropVal->wstrLen = lenght - 1;
		return true;
	}
	case propClassInt4:
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

bool Prop::setPropVal(tVariant* propVal)
{
	switch (TV_VT(propVal))
	{
	case VTYPE_BOOL:
	{
		//if (TV_VT(propVal) != VTYPE_BOOL)
			//return false;
		delete[] value;
		value = malloc(sizeof(bool));
		const bool _value = TV_BOOL(propVal);
		memcpy(value, &_value, size);
		type = 1; //TODO: удалить это говно
		break;
	}
	case VTYPE_PWSTR:
	{
		if (TV_VT(propVal) != VTYPE_PWSTR)
			return false;

		wchar_t* _value = NULL;
		OneCStringConverter::convFromShortWchar((wchar_t**)& _value, propVal->pwstrVal);

		delete value;
		value = new std::wstring(_value);
		delete[] _value;

		type = 2; //TODO: удалить это говно

		break;
	}
	case VTYPE_I4:
	{
		if (TV_VT(propVal) != VTYPE_I4)
			return false;
		delete[] value;
		value = malloc(sizeof(int32_t));
		const int32_t _value = TV_I4(propVal);
		memcpy(value, &_value, size);
		type = 3; //TODO: удалить это говно
		break;
	}
	default:
		return false;
	}
	return true;
}

void* Prop::getValue()
{
	return value;
}
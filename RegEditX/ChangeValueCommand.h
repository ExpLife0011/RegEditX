#pragma once

#include "AppCommandBase.h"
#include "RegHelper.h"

template<typename T>
class ChangeValueCommand : public AppCommandBase {
public:
	ChangeValueCommand(const CString& path, const CString& name, const T& value, DWORD type) :
		AppCommandBase(L"Change Value"),
		_path(path), _name(name), _value(value), _type(type) {}

	bool Execute() override;
	bool Undo() override {
		return Execute();
	}

private:
	bool ChangeValue(CRegKey& key, const CString& value);
	bool ChangeValue(CRegKey& key, ULONGLONG value);

private:
	T _value;
	CString _path, _name;
	DWORD _type;
};

template<typename T>
bool ChangeValueCommand<T>::Execute() {
	CRegKey key((HKEY)RegHelper::OpenKey(_path, KEY_WRITE | KEY_READ));
	if (!key)
		return false;
	return ChangeValue(key, _value);
}

template<typename T>
bool ChangeValueCommand<T>::ChangeValue(CRegKey& key, const CString& value) {
	WCHAR oldValue[2048];
	ULONG chars = 2048;
	auto status = key.QueryStringValue(_name, oldValue, &chars);
	if (status != ERROR_SUCCESS)
		return false;

	status =  key.SetStringValue(_name, _value, _type);
	if (status == ERROR_SUCCESS) {
		_value = oldValue;
		return true;
	}
	return false;
}

template<typename T>
inline bool ChangeValueCommand<T>::ChangeValue(CRegKey & key, ULONGLONG value) {
	ULONGLONG oldValue = 0;
	ATLASSERT(_type == REG_DWORD || _type == REG_QWORD);
	auto status = (_type == REG_DWORD ? key.QueryDWORDValue(_name, (DWORD&)oldValue) : key.QueryQWORDValue(_name, oldValue));
	if (status != ERROR_SUCCESS)
		return false;

	status = _type == REG_DWORD ? key.SetDWORDValue(_name, (DWORD)_value) : key.SetQWORDValue(_name, _value);
	if (status == ERROR_SUCCESS) {
		_value = oldValue;
		return true;
	}
	return false;
}

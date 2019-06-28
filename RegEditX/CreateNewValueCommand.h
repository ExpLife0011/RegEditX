#pragma once

#include "AppCommandBase.h"
#include "RegistryManager.h"

template<typename T>
class CreateNewValueCommand : public AppCommandBase {
public:
	CreateNewValueCommand(const CString& path, const CString& name, const T& value, DWORD type)
		: AppCommandBase(L"Create New Value"), _path(path), _name(name), _value(value), _type(type) {
	}

	bool Execute() override;
	bool Undo() override;

private:
	CString _path, _name;
	T _value;
	DWORD _type;
};

template<typename T>
inline bool CreateNewValueCommand<T>::Execute() {
	auto status = RegistryManager::Get().CreateValue(_path, _name, _value, _type);
	return status == ERROR_SUCCESS;
}

template<typename T>
inline bool CreateNewValueCommand<T>::Undo() {
	return false;
}

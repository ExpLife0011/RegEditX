#include "stdafx.h"
#include "resource.h"
#include "RenameKeyCommand.h"
#include "RegistryManager.h"
#include "RegHelper.h"

RenameKeyCommand::RenameKeyCommand(const CString& path, const CString& newname)
	: AppCommandBase(L"Rename Key"), _path(path), _name(newname) {
}

bool RenameKeyCommand::Execute() {
	auto hKey = RegHelper::OpenKey(_path, KEY_WRITE);
	if (!hKey)
		return false;

	auto index = _path.ReverseFind(L'\\');
	CString parent = _path.Left(index);
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, _name);
	auto status = ::NtRenameKey(hKey, &name);
	auto success = NT_SUCCESS(status);
	if (success) {
		_name = _path.Right(index + 1);
		_path = parent + _name;
	}
	return success;
}

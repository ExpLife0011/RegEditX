#include "stdafx.h"
#include "RenameKeyCommand.h"
#include "RegistryManager.h"

RenameKeyCommand::RenameKeyCommand(const CString& path, const CString& newname)
	: AppCommandBase(L"Rename Key"), _path(path), _name(newname) {
}

bool RenameKeyCommand::Execute() {
	return false;
}

#include "stdafx.h"
#include "RenameKeyCommand.h"

RenameKeyCommand::RenameKeyCommand(const CString& path, const CString& newname)
	: AppCommandBase(L"Rename Key"), _path(path), _name(newname) {
}

void RenameKeyCommand::Execute() {
}

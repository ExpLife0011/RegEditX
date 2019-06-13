#pragma once

#include "AppCommandBase.h"

class RenameKeyCommand : AppCommandBase {
	RenameKeyCommand(const CString& path, const CString& newname);

	bool Execute() override;
	bool Undo() override {
		return Execute();
	}

private:
	CString _path, _name;
};


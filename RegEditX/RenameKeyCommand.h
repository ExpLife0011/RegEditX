#pragma once

#include "AppCommandBase.h"

class RenameKeyCommand : AppCommandBase {
	RenameKeyCommand(const CString& path, const CString& newname);

	void Execute() override;
	void Undo() override {
		Execute();
	}

private:
	CString _path, _name;
};


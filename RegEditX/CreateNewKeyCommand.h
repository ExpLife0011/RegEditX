#pragma once

#include "AppCommandBase.h"

class CreateNewKeyCommand : public AppCommandBase {
public:
	CreateNewKeyCommand(const CString& parentPath, PCWSTR keyName);

	void Execute() override;
	void Undo() override;

private:
	CString _keyName;
	CString _parentName;
};


#pragma once

#include "AppCommandBase.h"

struct ITreeOperations {
	virtual bool SelectNode(TreeNodeBase* parent, PCWSTR name) = 0;
};

struct IMainApp {
	virtual void AddCommand(std::shared_ptr<AppCommandBase> cmd, bool execute = true) = 0;
};

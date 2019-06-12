#include "stdafx.h"
#include "AppCommandBase.h"

AppCommandList::AppCommandList() : AppCommandBase(L"") {
	_commands.reserve(4);
}

void AppCommandList::AddCommand(std::shared_ptr<AppCommandBase> command) {
	_commands.push_back(command);
}

void AppCommandList::Execute() {
	for (auto& cmd : _commands)
		cmd->Execute();
}

void AppCommandList::Undo() {
	for (size_t i = _commands.size() - 1; i >= 0; --i)
		_commands[i]->Undo();
}

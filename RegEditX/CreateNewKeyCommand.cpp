#include "stdafx.h"
#include "CreateNewKeyCommand.h"
#include "RegistryManager.h"

CreateNewKeyCommand::CreateNewKeyCommand(const CString& parentPath, PCWSTR keyName)
	: AppCommandBase(L"New Key"), _parentName(parentPath), _keyName(keyName) {
}

void CreateNewKeyCommand::Execute() {
	auto status = RegistryManager::Get().CreateKey(_parentName, _keyName);
	if (status == ERROR_SUCCESS) {

	}
}

void CreateNewKeyCommand::Undo() {
	RegistryManager::Get().DeleteKey(_parentName, _keyName);
}


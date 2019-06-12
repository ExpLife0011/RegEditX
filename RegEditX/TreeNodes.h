#pragma once

#include "TreeNode.h"

class RegKeyTreeNode final : public TreeNodeBase {
public:
	RegKeyTreeNode(HKEY hive, const CString& text, HKEY hKey = nullptr) : TreeNodeBase(text, nullptr), _key(hKey ? hKey : hive), _root(hive) {}

	virtual TreeNodeType GetNodeType() const {
		return TreeNodeType::RegistryKey;
	}

	virtual int GetContextMenuIndex() const override {
		return 0;
	}

	bool Expand(bool expand) override;
	bool IsExpanded() const override;
	bool HasChildren() const override;
	CRegKey* GetKey() {
		return &_key;
	}

private:
	mutable CRegKey _key;
	HKEY _root;
	bool _expanded{ false };
};


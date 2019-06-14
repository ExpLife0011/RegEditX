#pragma once

#include "TreeNode.h"

class RegKeyTreeNode : public TreeNodeBase {
public:
	RegKeyTreeNode(HKEY hive, const CString& text, HKEY hKey) : TreeNodeBase(text, nullptr), _key(hKey), _root(hive) {}

	virtual TreeNodeType GetNodeType() const {
		return TreeNodeType::RegistryKey;
	}

	virtual int GetContextMenuIndex() const override {
		return 0;
	}

	bool Expand(bool expand) override;
	bool IsExpanded() const override;
	bool HasChildren() const override;
	bool CanDelete() const override;

	int GetImage() const override {
		return _hive ? 4 : 0;
	}

	int GetSelectedImage() const override {
		return _hive ? 4 : 1;
	}

	CRegKey* GetKey() {
		return &_key;
	}

	void SetHive(bool hive) {
		_hive = hive;
	}

private:
	mutable CRegKey _key;
	HKEY _root;
	bool _expanded{ false };
	bool _hive{ false };
};


#include "stdafx.h"
#include "TreeNode.h"

TreeNodeBase* TreeNodeBase::AddChild(TreeNodeBase* child) {
	_childNodes.push_back(child);
	child->SetParent(this);
	return child;
}

TreeNodeBase* TreeNodeBase::RemoveChild(const CString& name) {
	for (size_t i = 0; i < _childNodes.size(); ++i) {
		auto node = _childNodes[i];
		if (node->GetText().CompareNoCase(name) == 0) {
			_childNodes.erase(_childNodes.begin() + i);
			return node;
		}
	}
	return nullptr;
}

void TreeNodeBase::SetParent(TreeNodeBase* parent) {
	_parentNode = parent;
	_full = parent->GetFullPath() + L"\\" + GetText();
}

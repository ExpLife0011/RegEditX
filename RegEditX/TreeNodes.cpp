#include "stdafx.h"
#include "TreeNodes.h"

bool RegKeyTreeNode::Expand(bool expand) {
	if (expand && GetChildNodes().empty()) {
		WCHAR name[128];
		DWORD len;
		FILETIME lastWrite;
		for (DWORD index = 0; ; index++) {
			len = 128;
			if (_key.m_hKey == nullptr || _key.EnumKey(index, name, &len, &lastWrite) != ERROR_SUCCESS)
				break;

			CRegKey key;
			key.Open(_key.m_hKey, name, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS);
			auto node = new RegKeyTreeNode(_root, name, key.Detach());
			AddChild(node);
		}
		_expanded = true;
	}
	return expand;
}

bool RegKeyTreeNode::IsExpanded() const {
	return _expanded;
}

bool RegKeyTreeNode::HasChildren() const {
	WCHAR name[4];
	DWORD len = 4;
	if (!_key)
		return false;

	auto status = _key.EnumKey(0, name, &len);
	if(status == ERROR_SUCCESS || status == ERROR_MORE_DATA)
		return true;
	return false;
}

bool RegKeyTreeNode::CanDelete() const {
	return (ULONG_PTR)_key.m_hKey < (ULONG_PTR)(1 << 24);
}

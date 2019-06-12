#include "stdafx.h"
#include "RegistryManager.h"
#include "TreeNodes.h"
#include "View.h"

RegistryManager* RegistryManager::_instance;

RegistryManager::RegistryManager(CTreeViewCtrl& tree, CView& view) : _tree(tree), _view(view) {
	ATLASSERT(_instance == nullptr);
	_instance = this;
	_registryRoot = new TreeNodeBase(L"Registry");
	_registryRoot->AddChild(_HKCR = new RegKeyTreeNode(HKEY_CLASSES_ROOT, L"HKEY_CLASSES_ROOT"));
	_registryRoot->AddChild(_HKCU = new RegKeyTreeNode(HKEY_CURRENT_USER, L"HKEY_CURRENT_USER"));
	_registryRoot->AddChild(_HKLM = new RegKeyTreeNode(HKEY_LOCAL_MACHINE, L"HKEY_LOCAL_MACHINE"));
	_registryRoot->AddChild(_HKUsers = new RegKeyTreeNode(HKEY_USERS, L"HKEY_USERS"));
	_registryRoot->AddChild(_HKCC = new RegKeyTreeNode(HKEY_CURRENT_CONFIG, L"HKEY_CURENT_CONFIG"));
}

void RegistryManager::BuildTreeView() {
	auto hRoot = AddItem(_registryRoot, TVI_ROOT);
	for (const auto& node : _registryRoot->GetChildNodes()) {
		AddItem(node, hRoot);
	}

	_tree.Expand(hRoot, TVE_EXPAND);
	_tree.SelectItem(hRoot);
}

LRESULT RegistryManager::HandleNotification(NMHDR* nmhdr) {
	auto code = nmhdr->code;
	auto tv = reinterpret_cast<NMTREEVIEW*>(nmhdr);
	auto hItem = tv->itemNew.hItem;

	switch (code) {
		case TVN_ITEMEXPANDING:
			if ((tv->action & TVE_EXPAND) == 0)
				break;

			auto node = reinterpret_cast<TreeNodeBase*>(_tree.GetItemData(hItem));
			ATLASSERT(node);
			if(node->HasChildren())
				ExpandItem(node);
			break;
	}

	return 0;
}

void RegistryManager::ExpandItem(TreeNodeBase* node) {
	auto hItem = node->GetHItem();
	ATLASSERT(hItem);

	auto hChild = _tree.GetChildItem(hItem);
	if (hChild == nullptr)
		return;

	if (_tree.GetItemData(hChild) == 0) {
		_tree.DeleteItem(hChild);
		//_tree.SetItemState(hItem, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);

		_tree.LockWindowUpdate();
		CWaitCursor wait;
		node->Expand(true);
		for (auto& n : node->GetChildNodes()) {
			AddItem(n, hItem);
		}
		_tree.SortChildren(hItem);
		_tree.LockWindowUpdate(FALSE);
	}
}

LSTATUS RegistryManager::CreateKey(const CString& parent, const CString& name) {
	CString hive, path;
	GetHiveAndPath(parent, hive, path);

	auto root = GetHiveNode(hive);
	ATLASSERT(root);

	CRegKey key;
	auto status = key.Create(*root->GetKey(), path + L"\\" + name);
	if (status != ERROR_SUCCESS)
		return status;

	auto parentNode = FindNode(root, parent);
	ATLASSERT(parentNode);
	auto node = new RegKeyTreeNode(*root->GetKey(), name, key.Detach());
	parentNode->AddChild(node);
	if(IsExpanded(parentNode))
		AddItem(node, parentNode->GetHItem());
	_view.Update(parentNode, true);

	return ERROR_SUCCESS;
}

LSTATUS RegistryManager::DeleteKey(const CString& parent, const CString& name) {
	CString hive, path;
	GetHiveAndPath(parent, hive, path);
	auto root = GetHiveNode(hive);
	ATLASSERT(root);

	CRegKey key;
	auto status = key.Open(*root->GetKey(), path, DELETE);
	if (status != ERROR_SUCCESS)
		return status;

	status = key.DeleteSubKey(name);
	if (status != ERROR_SUCCESS)
		return status;

	auto parentNode = FindNode(root, parent);
	ATLASSERT(parentNode);
	auto node = parentNode->RemoveChild(name);
	ATLASSERT(node);
	_tree.DeleteItem(node->GetHItem());
	_view.Update(parentNode, true);

	return status;
}

TreeNodeBase* RegistryManager::FindNode(TreeNodeBase* root, const CString& path) const {
	int index = 1;
	auto name = path.Tokenize(L"\\", index);
	for(;;) {
		name = path.Tokenize(L"\\", index);
		if (index < 0)
			break;

		auto old = root;
		for (auto& node : root->GetChildNodes()) {
			if (node->GetText().CompareNoCase(name) == 0) {
				root = node;
				break;
			}
		}
		if (old == root)	// not found
			return nullptr;
	}
	return root;
}

RegKeyTreeNode* RegistryManager::GetHiveNode(const CString& name) const {
	static const struct {
		PCWSTR Name;
		RegKeyTreeNode* Node;
	} hives[] = {
		{ L"HKEY_LOCAL_MACHINE", _HKLM },
		{ L"HKEY_CURRENT_USER", _HKCU },
		{ L"HKEY_CLASSES_ROOT", _HKCR },
		{ L"HKEY_USERS", _HKUsers },
		{ L"HKEY_CURRENT_CONFIG", _HKCC },
	};

	for (auto& hive : hives)
		if (hive.Name == name)
			return hive.Node;
	return nullptr;
}

bool RegistryManager::IsExpanded(TreeNodeBase* node) const {
	auto hChild = _tree.GetChildItem(node->GetHItem());
	if (hChild == nullptr)
		return false;

	return _tree.GetItemData(hChild) != 0;
}

HTREEITEM RegistryManager::AddItem(TreeNodeBase* item, HTREEITEM hParent, HTREEITEM hAfter) {
	auto hItem = _tree.InsertItem(item->GetText(), item->GetImage(), item->GetSelectedImage(), hParent, hAfter);
	_tree.SetItemData(hItem, reinterpret_cast<LPARAM>(item));
	item->_hItem = hItem;
	if (item->HasChildren()) {
		_tree.InsertItem(L"***", hItem, TVI_LAST);
	}

	return hItem;
}

void RegistryManager::GetHiveAndPath(const CString& parent, CString& hive, CString& path) {
	hive = parent.Mid(1, parent.Find(L'\\', 6) - 1);
	path = parent.Mid(hive.GetLength() + 2);
}

bool RegistryManager::SelectNode(TreeNodeBase* parent, PCWSTR name) {
	const auto& nodes = parent->GetChildNodes();
	_tree.Expand(parent->GetHItem(), TVE_EXPAND);
	for(const auto& node : nodes)
		if (node->GetText().Compare(name) == 0) {
			ATLASSERT(node->GetHItem());
			if (_tree.SelectItem(node->GetHItem())) {
				_tree.Expand(node->GetHItem(), TVE_EXPAND);
				ExpandItem(node);
			}
			return true;
		}

	return false;
}


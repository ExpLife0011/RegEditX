#pragma once

#include "TreeNodes.h"
#include "UICommon.h"
#include "View.h"

class RegistryManager : public ITreeOperations {
public:
	inline static const WCHAR* DeletedKey = L"__deleted__";

	static RegistryManager& Get() {
		return *_instance;
	}

	void BuildTreeView();
	LRESULT HandleNotification(NMHDR* nmhdr);
	void ExpandItem(TreeNodeBase* node);
	CRegKey& GetDeletedKey() {
		return _deletedKey;
	}

	LSTATUS CreateKey(const CString& parent, const CString& name);
	LSTATUS DeleteKey(const CString& parent, const CString& name);
	LSTATUS RenameKey(const CString& parent, const CString& name);
	LSTATUS RenameValue(const CString& path, const CString& oldName, const CString& newName);
	LSTATUS DeleteValue(const CString& path, const CString& name);

	RegKeyTreeNode* GetRoot(const CString& parent, CString& path);

	template<typename T>
	LSTATUS CreateValue(const CString& path, const CString& name, const T& value, DWORD type);

	TreeNodeBase* FindNode(TreeNodeBase* root, const CString& path) const;
	RegKeyTreeNode* GetHiveNode(const CString& name) const;
	bool IsExpanded(TreeNodeBase* node) const;
	bool IsHive(TreeNodeBase* node) const;
	void GetHiveAndPath(const CString& parent, CString& hive, CString& path);
	void Refresh();

	void Destroy();

private:
	friend class CMainFrame;
	RegistryManager(CTreeViewCtrl& tree, CView& view);
	void BuildHiveList();

	HTREEITEM AddItem(TreeNodeBase* item, HTREEITEM hParent, HTREEITEM hAfter = TVI_LAST);
	void Refresh(TreeNodeBase* node);
	void AddNewKeys(RegKeyTreeNode* node);

	LRESULT SetValue(CRegKey& key, const CString& name, const ULONGLONG& value, DWORD type);
	LRESULT SetValue(CRegKey& key, const CString& name, const CString& value, DWORD type);

	RegKeyTreeNode* _registryRoot;
	TreeNodeBase* _stdRegistryRoot;

	RegKeyTreeNode* _HKLM;
	RegKeyTreeNode* _HKCR;
	RegKeyTreeNode* _HKCU;
	RegKeyTreeNode* _HKUsers;
	RegKeyTreeNode* _HKCC;

	CAtlMap<CString, CString> _hiveList;

	CTreeViewCtrl& _tree;
	CView& _view;
	CRegKey _deletedKey;

	// Inherited via ITreeOperations
	bool SelectNode(TreeNodeBase* parent, PCWSTR name) override;

	static RegistryManager* _instance;
};

template<typename T>
inline LSTATUS RegistryManager::CreateValue(const CString & path, const CString & name, const T & value, DWORD type) {
	CString realpath;
	auto root = GetRoot(path, realpath);
	ATLASSERT(root);

	CRegKey key;
	auto status = key.Open(*root->GetKey(), realpath, KEY_WRITE);
	if (status != ERROR_SUCCESS)
		return status;

	status = (LSTATUS)SetValue(key, name, value, type);
	if (status == ERROR_SUCCESS)
		_view.Update(nullptr, true);
	return status;
}

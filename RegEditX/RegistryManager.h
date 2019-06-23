#pragma once

#include "TreeNodes.h"
#include "UICommon.h"
#include "View.h"

class RegistryManager : public ITreeOperations {
public:
	static RegistryManager& Get() {
		return *_instance;
	}

	void BuildTreeView();
	LRESULT HandleNotification(NMHDR* nmhdr);
	void ExpandItem(TreeNodeBase* node);

	LSTATUS CreateKey(const CString& parent, const CString& name);
	LSTATUS DeleteKey(const CString& parent, const CString& name);
	LSTATUS RenameKey(const CString& parent, const CString& name);
	LSTATUS RenameValue(const CString& path, const CString& oldName, const CString& newName);

	TreeNodeBase* FindNode(TreeNodeBase* root, const CString& path) const;
	RegKeyTreeNode* GetHiveNode(const CString& name) const;
	bool IsExpanded(TreeNodeBase* node) const;
	bool IsHive(TreeNodeBase* node) const;
	void GetHiveAndPath(const CString& parent, CString& hive, CString& path);
	void Refresh();

private:
	friend class CMainFrame;
	RegistryManager(CTreeViewCtrl& tree, CView& view);
	void BuildHiveList();

	HTREEITEM AddItem(TreeNodeBase* item, HTREEITEM hParent, HTREEITEM hAfter = TVI_LAST);
	void Refresh(TreeNodeBase* node);
	void AddNewKeys(RegKeyTreeNode* node);
	RegKeyTreeNode* GetRoot(const CString& parent, CString& path);

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

	// Inherited via ITreeOperations
	bool SelectNode(TreeNodeBase* parent, PCWSTR name) override;

	static RegistryManager* _instance;
};


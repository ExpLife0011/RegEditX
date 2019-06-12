#pragma once

#include "TreeNodes.h"
#include "ITreeOperations.h"
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
	TreeNodeBase* FindNode(TreeNodeBase* root, const CString& path) const;
	RegKeyTreeNode* GetHiveNode(const CString& name) const;
	bool IsExpanded(TreeNodeBase* node) const;

private:
	friend class CMainFrame;
	RegistryManager(CTreeViewCtrl& tree, CView& view);

	HTREEITEM AddItem(TreeNodeBase* item, HTREEITEM hParent, HTREEITEM hAfter = TVI_LAST);
	void GetHiveAndPath(const CString& parent, CString& hive, CString& path);

	TreeNodeBase* _registryRoot;
	RegKeyTreeNode* _HKLM;
	RegKeyTreeNode* _HKCR;
	RegKeyTreeNode* _HKCU;
	RegKeyTreeNode* _HKUsers;
	RegKeyTreeNode* _HKCC;

	CTreeViewCtrl& _tree;
	CView& _view;

	// Inherited via ITreeOperations
	bool SelectNode(TreeNodeBase* parent, PCWSTR name) override;

	static RegistryManager* _instance;
};


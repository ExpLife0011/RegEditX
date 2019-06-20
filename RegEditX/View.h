// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"

struct TreeNodeBase;
struct ITreeOperations;
struct IMainApp;

struct ListItem {
	TreeNodeBase* TreeNode{ nullptr };
	CString ValueName;
	DWORD ValueType, ValueSize;
	LARGE_INTEGER LastWriteTime = { 0 };
};

class CView : public CWindowImpl<CView, CListViewCtrl> {
public:
	DECLARE_WND_SUPERCLASS(nullptr, CListViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg);
	static PCWSTR GetRegTypeAsString(DWORD type);
	static int GetRegTypeIcon(DWORD type);
	static CString GetKeyDetails(TreeNodeBase*);
	CString GetDataAsString(const ListItem& item);
	bool CanDeleteSelected() const;
	bool CanEditValue() const;
	ListItem& GetItem(int index);
	const ListItem& GetItem(int index) const;

	void Update(TreeNodeBase* node, bool onlyIfTheSame = false);
	void Init(ITreeOperations*, IMainApp*);

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnGetDispInfo)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_DBLCLK, OnDoubleClick)
		DEFAULT_REFLECTION_HANDLER()
	ALT_MSG_MAP(1)
		COMMAND_ID_HANDLER(ID_EDIT_DELETE, OnDelete)
		COMMAND_ID_HANDLER(ID_EDIT_RENAME, OnEditRename)
		COMMAND_ID_HANDLER(ID_EDIT_MODIFYVALUE, OnModifyValue)
	END_MSG_MAP()

	LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnDoubleClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditRename(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnModifyValue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
	CEdit m_Edit;
	TreeNodeBase* m_CurrentNode{ nullptr };
	ITreeOperations* m_TreeOperations;
	IMainApp* m_App;
	std::vector<ListItem> m_Items;
};

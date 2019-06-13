// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "View.h"
#include "MainFrm.h"
#include "CreateNewKeyCommand.h"
#include "NewKeyDlg.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
	if (m_view.PreTranslateMessage(pMsg))
		return TRUE;

	return (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg));
}

BOOL CMainFrame::OnIdle() {
	UpdateUI();
	UIUpdateToolBar();

	return FALSE;
}

void CMainFrame::UpdateUI() {
	UIEnable(ID_EDIT_UNDO, m_CmdMgr.CanUndo());
	UIEnable(ID_EDIT_REDO, m_CmdMgr.CanRedo());
	if (m_splitter.GetActivePane() == 0)
		UIEnable(ID_EDIT_DELETE, m_SelectedNode && m_SelectedNode->CanDelete());
	else
		UIEnable(ID_EDIT_DELETE, m_view.CanDeleteSelected());
	UIEnable(ID_NEW_KEY, m_SelectedNode->GetNodeType() == TreeNodeType::RegistryKey);
	UISetText(ID_EDIT_UNDO, (m_CmdMgr.CanUndo() ? L"Undo " + m_CmdMgr.GetUndoCommand()->GetName() : L"Undo") + CString(L"\tCtrl+Z"));
	UISetText(ID_EDIT_REDO, (m_CmdMgr.CanRedo() ? L"Redo " + m_CmdMgr.GetRedoCommand()->GetName() : L"Redo") + CString(L"\tCtrl+Y"));
}

LRESULT CMainFrame::OnTreeContextMenu(int, LPNMHDR, BOOL&) {
	POINT pt;
	::GetCursorPos(&pt);
	POINT screen(pt);
	::ScreenToClient(m_treeview, &pt);
	auto hItem = m_treeview.HitTest(pt, nullptr);
	if (hItem == nullptr)
		return 0;

	auto current = reinterpret_cast<TreeNodeBase*>(m_treeview.GetItemData(hItem));
	if (current == nullptr)
		return 0;

	auto index = current->GetContextMenuIndex();
	if (index < 0)
		return 0;

	CMenu menu;
	menu.LoadMenuW(IDR_CONTEXT);
	m_CmdBar.TrackPopupMenu(menu.GetSubMenu(index), TPM_LEFTALIGN | TPM_RIGHTBUTTON, screen.x, screen.y);

	return 0;
}

LRESULT CMainFrame::OnTreeSelectionChanged(int, LPNMHDR nmhdr, BOOL&) {
	auto item = reinterpret_cast<NMTREEVIEW*>(nmhdr);
	auto node = reinterpret_cast<TreeNodeBase*>(m_treeview.GetItemData(item->itemNew.hItem));
	m_SelectedNode = node;
	m_view.Update(node);

	return 0;
}

LRESULT CMainFrame::OnTreeDeleteItem(int, LPNMHDR nmhdr, BOOL&) {
	auto item = reinterpret_cast<NMTREEVIEW*>(nmhdr);
	auto node = reinterpret_cast<TreeNodeBase*>(m_treeview.GetItemData(item->itemOld.hItem));
	if(node)
		node->Delete();

	return 0;
}

LRESULT CMainFrame::OnTreeItemExpanded(int, LPNMHDR, BOOL&) {
	return LRESULT();
}

LRESULT CMainFrame::OnTreeItemExpanding(int, LPNMHDR nmhdr, BOOL&) {
	return m_RegMgr.HandleNotification(nmhdr);

	return 0;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, nullptr, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_COMMANDBAR);
	// remove old menu
	SetMenu(nullptr);

	m_SmallImages.Create(16, 16, ILC_COLOR32 | ILC_MASK, 6, 4);
	m_LargeImages.Create(32, 32, ILC_COLOR32 | ILC_MASK, 6, 4);

	UINT icons[] = {
		IDI_FOLDER, IDI_FOLDER_OPEN, IDI_TEXT, IDI_BINARY
	};

	for (auto id : icons) {
		auto hIcon = AtlLoadIcon(id);
		m_SmallImages.AddIcon(hIcon);
		m_LargeImages.AddIcon(hIcon);
	}

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, nullptr, TRUE);

	CReBarCtrl rebar(m_hWndToolBar);
	rebar.LockBands(TRUE);

	CreateSimpleStatusBar();
	CStatusBarCtrl sb(m_hWndStatusBar);

	m_hWndClient = m_splitter.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	//m_pane.SetPaneContainerExtendedStyle(PANECNT_NOBORDER);
	//m_pane.Create(m_splitter, _T("Local System"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_treeview.Create(m_splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
	//m_pane.SetClient(m_treeview);
	m_treeview.SetExtendedStyle(TVS_EX_DOUBLEBUFFER | 0*TVS_EX_MULTISELECT, 0);
	m_treeview.SetImageList(m_SmallImages.m_hImageList, TVSIL_NORMAL);

	m_view.Create(m_splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | LVS_SINGLESEL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA, WS_EX_CLIENTEDGE);
	m_view.SetImageList(m_SmallImages.m_hImageList, LVSIL_SMALL);
	m_view.SetImageList(m_LargeImages.m_hImageList, LVSIL_NORMAL);

	m_splitter.SetSplitterPanes(m_treeview, m_view);
	UpdateLayout();
	m_splitter.SetSplitterPosPct(25);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);
	UISetCheck(ID_VIEW_TREEPANE, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	m_RegMgr.BuildTreeView();
	m_view.Init(&m_RegMgr);



	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnAlwaysOnTop(WORD, WORD, HWND, BOOL&) {
	auto style = GetWindowLongPtr(GWL_EXSTYLE);
	bool topmost = (style & WS_EX_TOPMOST) == 0;
	SetWindowPos(topmost ? HWND_TOPMOST : HWND_NOTOPMOST, &RECT(), SWP_NOREPOSITION | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOMOVE);

	UISetCheck(ID_OPTIONS_ALWAYSONTOP, topmost);
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnEditRedo(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_CmdMgr.CanRedo());
	if (!m_CmdMgr.CanRedo())
		return 0;
	m_CmdMgr.Redo();


	return 0;
}

LRESULT CMainFrame::OnEditUndo(WORD, WORD, HWND, BOOL&) {
	ATLASSERT(m_CmdMgr.CanUndo());
	if (!m_CmdMgr.CanUndo())
		return 0;
	m_CmdMgr.Undo();


	return 0;
}

LRESULT CMainFrame::OnNewKey(WORD, WORD, HWND, BOOL&) {
	auto hItem = m_treeview.GetSelectedItem();
	auto node = reinterpret_cast<TreeNodeBase*>(m_treeview.GetItemData(hItem));
	ATLASSERT(node);

	CNewKeyDlg dlg;
	if (dlg.DoModal() == IDOK) {
		if (node->FindChild(dlg.GetKeyName())) {
			AtlMessageBox(m_hWnd, L"Key name already exists", IDR_MAINFRAME, MB_ICONERROR);
			return 0;
		}
		auto cmd = std::make_shared<CreateNewKeyCommand>(node->GetFullPath(), dlg.GetKeyName());
		m_CmdMgr.AddCommand(cmd);

	}

	return 0;
}

LRESULT CMainFrame::OnViewTreePane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	bool bShow = (m_splitter.GetSinglePaneMode() != SPLIT_PANE_NONE);
	m_splitter.SetSinglePaneMode(bShow ? SPLIT_PANE_NONE : SPLIT_PANE_RIGHT);
	UISetCheck(ID_VIEW_TREEPANE, bShow);

	return 0;
}

LRESULT CMainFrame::OnTreePaneClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/) {
	if (hWndCtl == m_pane.m_hWnd) {
		m_splitter.SetSinglePaneMode(SPLIT_PANE_RIGHT);
		UISetCheck(ID_VIEW_TREEPANE, 0);
	}

	return 0;
}

LRESULT CMainFrame::OnDelete(WORD, WORD, HWND, BOOL&) {
	return LRESULT();
}

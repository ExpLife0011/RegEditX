#include "stdafx.h"
#include "resource.h"
#include "BinaryValueDlg.h"

void CBinaryValueDlg::SetName(const CString& name, bool readonly) {
	m_Name = name;
	m_ReadOnlyName = readonly;
}

void CBinaryValueDlg::SetValue(BYTE* data, DWORD size) {
	m_DataSource.SetData(data, size);
}

LRESULT CBinaryValueDlg::OnTextChanged(WORD, WORD, HWND, BOOL&) {
	return LRESULT();
}

LRESULT CBinaryValueDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&) {
	DoDataExchange(FALSE);
	RECT rc;
	GetDlgItem(IDC_BUTTON1).GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_HexEdit.SetDataSource(&m_DataSource);
	m_HexEdit.Create(m_hWnd, &rc, nullptr,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VSCROLL | WS_GROUP, WS_EX_CLIENTEDGE, IDC_HEX);
	m_HexEdit.SetFocus();

	if (m_ReadOnlyName) {
		GetDlgItem(IDC_NAME).SendMessage(EM_SETREADONLY, TRUE);
	}

	if (!m_CanModify)
		m_HexEdit.SetReadOnly(TRUE);

	CenterWindow(GetParent());
	return TRUE;
}

LRESULT CBinaryValueDlg::OnCloseCmd(WORD, WORD wID, HWND, BOOL&) {
	if (wID == IDCANCEL) {
		EndDialog(IDCANCEL);
		return 0;
	}
	if (DoDataExchange(TRUE)) {
		EndDialog(IDOK);
	}
	return 0;
}

#pragma once

#include "..\HexEditCtrl\HexEditCtrl.h"
#include "DynamicDataSource.h"

class CBinaryValueDlg :
	public CDialogImpl<CBinaryValueDlg>,
	public CWinDataExchange<CBinaryValueDlg> {
public:
	enum { IDD = IDD_BINVALUE };

	CBinaryValueDlg(bool canModify) : m_CanModify(canModify) {}

	void SetName(const CString& name, bool readonly);
	const CString& GetName() const {
		return m_Name;
	}
	void SetValue(BYTE* data, DWORD size);

	BEGIN_MSG_MAP(CBinValueDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnTextChanged)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CBinValueDlg)
		DDX_TEXT_LEN(IDC_NAME, m_Name, 64)
		DDX_TEXT(IDC_NAME, m_Name)
	END_DDX_MAP()

protected:
	LRESULT OnTextChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CHexEditorCtrl m_HexEdit;
	DynamicDataSource m_DataSource;
	CString m_Name;
	bool m_ReadOnlyName{ false };
	bool m_CanModify;
};


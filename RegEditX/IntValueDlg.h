#pragma once

class CIntValueDlg : 
	public CDialogImpl<CIntValueDlg>,
	public CWinDataExchange<CIntValueDlg> {
public:
	enum { ID = IDD_INTVALUE };

	BEGIN_MSG_MAP(CIntValueDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnTextChanged)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CIntValueDlg)
		DDX_TEXT(IDC_VALUE, m_Value)
	END_DDX_MAP()

private:
	LRESULT OnTextChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CString m_Value;
};


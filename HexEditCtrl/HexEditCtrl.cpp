// HexEditCtrl.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "HexEditCtrl.h"

#define ROUND(x) (x / m_iDataSize * m_iDataSize)

CHexEditorCtrl::CHexEditorCtrl() :
	m_dwPos(0),
	m_bShowAddress(true),
	m_bShowData(true),
	m_bShowAscii(true),
	m_bInDataPane(true),
	m_iDataSize(1),
	m_dwDigitOffset(0),
	m_cInvalidAscii('.'),
	m_dwSelStart((DWORD)-1),
	m_dwSelEnd((DWORD)-1),
	m_bReadOnly(FALSE) {
	::ZeroMemory(&m_rcData, sizeof(m_rcData));
	::ZeroMemory(&m_rcAscii, sizeof(m_rcAscii));
	m_szMargin.cx = 3;
	m_szMargin.cy = 2;
}

BOOL CHexEditorCtrl::PreTranslateMessage(MSG* /*pMsg*/) {
	return FALSE;
}

void CHexEditorCtrl::SetDataSource(IDataSource* pSrc) {
	m_pDataSource = pSrc;
}

BOOL CHexEditorCtrl::GetModify() const {
	return CanUndo();
}

BOOL CHexEditorCtrl::Undo() {
	if (m_aUndostack.GetSize() == 0) return FALSE;
	// Undo all changes to this position. This solves problems
	// where only one digit gets undone at a time.
	DWORD dwPos = ROUND(m_aUndostack[m_aUndostack.GetSize() - 1].dwPos);
	while (m_aUndostack.GetSize() > 0
		&& ROUND(m_aUndostack[m_aUndostack.GetSize() - 1].dwPos) == dwPos) {
		UNDOENTRY entry = m_aUndostack[m_aUndostack.GetSize() - 1];
		m_aUndostack.RemoveAt(m_aUndostack.GetSize() - 1);
		auto pData = m_pDataSource->GetData();
		*(pData + entry.dwPos) = entry.bValue;
	}
	SetSel(dwPos, dwPos);
	return Invalidate();
}

BOOL CHexEditorCtrl::CanUndo() const {
	return m_aUndostack.GetSize() > 0;
}

BOOL CHexEditorCtrl::CanCopy() const {
	return m_dwSelStart != m_dwSelEnd;
}

BOOL CHexEditorCtrl::GetReadOnly() const {
	return m_bReadOnly;
}

void CHexEditorCtrl::SetReadOnly(BOOL bReadOnly) {
	ATLASSERT(::IsWindow(m_hWnd));
	m_bReadOnly = bReadOnly;
	Invalidate();
}

int CHexEditorCtrl::GetLineHeight() const {
	ATLASSERT(::IsWindow(m_hWnd));
	return m_tmEditor.tmHeight;
}

int CHexEditorCtrl::GetLinesPrPage() const {
	ATLASSERT(::IsWindow(m_hWnd));
	RECT rcClient;
	GetClientRect(&rcClient);
	return (rcClient.bottom - rcClient.top) / GetLineHeight();
}

void CHexEditorCtrl::SetDataSize(int iSize) {
	ATLASSERT(::IsWindow(m_hWnd));
	ATLASSERT(iSize == 1 || iSize == 2 || iSize == 4);   // BYTE / WORD / DWORD
	ATLASSERT((m_BytesPerLine % iSize) == 0);

	m_iDataSize = (BYTE)iSize;
	Invalidate();
}

void CHexEditorCtrl::SetInvalidChar(TCHAR ch) {
	ATLASSERT(::IsWindow(m_hWnd));
	m_cInvalidAscii = ch;
	Invalidate();
}

SIZE CHexEditorCtrl::GetMargins() const {
	return m_szMargin;
}

void CHexEditorCtrl::SetMargins(SIZE szMargin) {
	ATLASSERT(::IsWindow(m_hWnd));
	m_szMargin = szMargin;
	Invalidate();
}

void CHexEditorCtrl::SetDisplayOptions(BOOL bShowAddress, BOOL bShowData, BOOL bShowAscii) {
	ATLASSERT(::IsWindow(m_hWnd));
	m_bShowAddress = bShowAddress == TRUE;
	m_bShowData = bShowData == TRUE;
	m_bShowAscii = bShowAscii == TRUE;
	Invalidate();
}

bool CHexEditorCtrl::SetSel(DWORD dwStart, DWORD dwEnd, BOOL bNoScroll /*= FALSE*/) {
	ATLASSERT(IsWindow());
	ATLASSERT(m_pDataSource);
	if (!m_pDataSource)
		return false;

	auto size = m_pDataSource->GetSize();

	if (dwEnd == (DWORD)-1) dwEnd = size;
	if (dwStart == dwEnd && dwEnd >= size) dwEnd = size - 1;
	if (dwStart != dwEnd && dwEnd > size) dwEnd = size;
	if (dwStart >= size) dwStart = size - 1;
	dwStart = ROUND(dwStart);
	dwEnd = ROUND(dwEnd);
	if (dwEnd == dwStart && m_dwSelStart != m_dwSelEnd) ShowCaret();
	if (dwEnd != dwStart && m_dwSelStart == m_dwSelEnd) HideCaret();
	m_dwSelStart = dwStart;
	m_dwSelEnd = dwEnd;
	m_dwDigitOffset = 0;
	if (!bNoScroll) RecalcPosition(dwEnd);
	if (dwStart == dwEnd) RecalcCaret();
	return Invalidate();
}

void CHexEditorCtrl::GetSel(DWORD& dwStart, DWORD& dwEnd) const {
	dwStart = m_dwSelStart;
	dwEnd = m_dwSelEnd;
	if (dwStart > dwEnd) {        // Return values in normalized form
		DWORD dwTemp = dwStart;
		dwStart = dwEnd;
		dwEnd = dwTemp;
	}
}

int CHexEditorCtrl::GetDataSize() const {
	return m_iDataSize;
}

LRESULT CHexEditorCtrl::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return 1;	// suppress erase
}

LRESULT CHexEditorCtrl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CLogFont lf = AtlGetStockFont(OEM_FIXED_FONT);
	_tcscpy_s(lf.lfFaceName, _T("Consolas"));     // This is the font we prefer
	lf.lfPitchAndFamily = FIXED_PITCH;                // Make sure Windows finds a fixed-width font
	lf.lfHeight = 16;
	m_fontEditor.CreateFontIndirect(&lf);

	CClientDC dc = m_hWnd;
	HFONT hOldFont = dc.SelectFont(m_fontEditor);
	dc.GetTextMetrics(&m_tmEditor);
	dc.SelectFont(hOldFont);

	ModifyStyle(0, WS_VSCROLL);
	SetScrollPos(SB_VERT, 0, TRUE);

	return 0;
}

LRESULT CHexEditorCtrl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CPaintDC dc(m_hWnd);
	CDCHandle h(dc);
	DoPaint(h);

	return 0;
}

LRESULT CHexEditorCtrl::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	if (m_pDataSource) {
		CreateSolidCaret(2, m_tmEditor.tmHeight - 2);
		ShowCaret();
	}
	return 0;
}

LRESULT CHexEditorCtrl::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	HideCaret();
	DestroyCaret();
	return 0;
}

LRESULT CHexEditorCtrl::OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	if (m_rcAscii.left == 0 && m_rcData.left == 0) return 0; // Not ready yet!
	// Determine what area the cursor is over and change
	// cursor shape...
	DWORD dwPos = GetMessagePos();
	POINT pt = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
	ScreenToClient(&pt);
	DWORD dwDummy = 0;
	bool bDummy = false;
	::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(GetPosFromPoint(pt, dwDummy, bDummy) ? IDC_IBEAM : IDC_ARROW)));
	return TRUE;
}

LRESULT CHexEditorCtrl::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	if (m_bReadOnly) {
		::MessageBeep((UINT)-1);
		return 0;
	}
	if (wParam < VK_SPACE) return 0;
	if (m_dwSelStart != m_dwSelEnd) SetSel(m_dwSelStart, m_dwSelStart);
	if (m_bInDataPane) {
		if (isdigit(wParam) || (toupper(wParam) >= 'A' && toupper(wParam) <= 'F')) {
			BYTE b = (BYTE)(isdigit(wParam) ? wParam - '0' : toupper(wParam) - 'A' + 10);
			AssignDigitValue(m_dwSelStart, m_dwDigitOffset, b);
		}
	}
	else {
		AssignCharValue(m_dwSelStart, m_dwDigitOffset, LOBYTE(wParam));
		if (HIBYTE(wParam) != 0) AssignCharValue(m_dwSelStart, m_dwDigitOffset, HIBYTE(wParam));
	}
	return 0;
}

LRESULT CHexEditorCtrl::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	if (GetCapture() == m_hWnd) {
		ReleaseCapture();
		return 0;
	}
	DWORD dwPage = m_BytesPerLine * (DWORD)GetLinesPrPage();
	bool bCtrl = (::GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	bool bShift = (::GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	switch (wParam) {
		case VK_F6:
			PostMessage(WM_COMMAND, MAKEWPARAM(ID_NEXT_PANE, 0));
			return 0;
		case VK_DELETE:
			if (m_bInDataPane && !m_bReadOnly) {
				m_dwDigitOffset = 0;
				for (int i = 0; i < m_iDataSize * 2; i++) PostMessage(WM_CHAR, '0');
			}
			return 0;
		case VK_LEFT:
			if (m_dwSelEnd < m_iDataSize) return 0;
			SetSel(bShift ? m_dwSelStart : m_dwSelEnd - m_iDataSize, m_dwSelEnd - m_iDataSize);
			return 0;
		case VK_RIGHT:
			if (m_dwSelStart + m_iDataSize > m_pDataSource->GetSize()) return 0;
			SetSel(bShift ? m_dwSelStart : m_dwSelEnd + m_iDataSize, m_dwSelEnd + m_iDataSize);
			return 0;
		case VK_UP:
			if (bCtrl) return SendMessage(WM_VSCROLL, SB_LINEUP);
			if (m_dwSelEnd < m_BytesPerLine) return 0;
			SetSel(bShift ? m_dwSelStart : m_dwSelEnd - m_BytesPerLine, m_dwSelEnd - m_BytesPerLine);
			return 0;
		case VK_DOWN:
			if (bCtrl) return SendMessage(WM_VSCROLL, SB_LINEDOWN);
			if (m_dwSelStart + m_BytesPerLine > m_pDataSource->GetSize()) return 0;
			SetSel(bShift ? m_dwSelStart : m_dwSelEnd + m_BytesPerLine, m_dwSelEnd + m_BytesPerLine);
			return 0;
		case VK_HOME:
			if (bCtrl) SetSel(bShift ? m_dwSelStart : 0, 0);
			else SetSel(bShift ? m_dwSelStart : m_dwSelEnd - (m_dwSelEnd % m_BytesPerLine), m_dwSelEnd - (m_dwSelEnd % m_BytesPerLine));
			return 0;
		case VK_END:
			if (bCtrl) SetSel(bShift ? m_dwSelStart : m_pDataSource->GetSize() - 1, m_pDataSource->GetSize() - (bShift ? 0 : 1));
			else SetSel(bShift ? m_dwSelStart : (m_dwSelEnd | 0xF) + (bShift ? 1 : 0), (m_dwSelEnd | 0xF) + (bShift ? 1 : 0));
			return 0;
		case VK_PRIOR:
			if (bCtrl) return SendMessage(WM_VSCROLL, SB_PAGEUP);
			if (m_dwSelEnd < dwPage) SetSel(bShift ? m_dwSelStart : 0, 0);
			else SetSel(bShift ? m_dwSelStart : m_dwSelEnd - dwPage, m_dwSelEnd - dwPage);
			return 0;
		case VK_NEXT:
			if (bCtrl) return SendMessage(WM_VSCROLL, SB_PAGEDOWN);
			SetSel(bShift ? m_dwSelStart : m_dwSelEnd + dwPage, m_dwSelEnd + dwPage);
			return 0;
	}
	return 0;
}

LRESULT CHexEditorCtrl::OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) {
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	bool bInDataPane = false;
	DWORD dwPos = 0;
	if (!GetPosFromPoint(pt, dwPos, bInDataPane)) return 0;
	m_bInDataPane = bInDataPane;
	SetSel(dwPos, dwPos + m_iDataSize);
	return 0;
}

LRESULT CHexEditorCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) {
	SetFocus();
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	bool bInDataPane = false;
	DWORD dwPos = 0;
	if (!GetPosFromPoint(pt, dwPos, bInDataPane)) return 0;
	m_bInDataPane = bInDataPane;
	SetSel(dwPos, dwPos);
	// If user is dragging the mouse, we'll initiate a selection...
	ClientToScreen(&pt);
	if (::DragDetect(m_hWnd, pt)) SetCapture();
	return 0;
}

LRESULT CHexEditorCtrl::OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	if (GetCapture() != m_hWnd) return 0;
	SendMessage(WM_MOUSEMOVE, wParam, lParam);
	ReleaseCapture();
	return 0;
}

LRESULT CHexEditorCtrl::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) {
	if (GetCapture() != m_hWnd) return 0;
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	// Mouse outside client rectangle? Let's scroll the view
	RECT rcClient;
	GetClientRect(&rcClient);
	if (pt.y < 0 && m_dwPos > 0) SendMessage(WM_VSCROLL, SB_LINEUP);
	if (pt.y > rcClient.bottom - rcClient.top) SendMessage(WM_VSCROLL, SB_LINEDOWN);
	// Expand the selection if mouse is over a valid position?
	bool bInDataPane = false;
	DWORD dwPos = 0;
	if (!GetPosFromPoint(pt, dwPos, bInDataPane)) return 0;
	if (m_bInDataPane != bInDataPane) return 0;
	SetSel(m_dwSelStart, dwPos == 0 ? 0 : dwPos + m_iDataSize);
	return 0;
}

LRESULT CHexEditorCtrl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	SetScrollRange(SB_VERT, 0, (int)(m_pDataSource->GetSize() / m_BytesPerLine) - GetLinesPrPage() + 1, TRUE);
	return 0;
}

LRESULT CHexEditorCtrl::OnVScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_POS | SIF_RANGE | SIF_TRACKPOS };
	GetScrollInfo(SB_VERT, &si);
	int nPos = m_dwPos / m_BytesPerLine;
	switch (LOWORD(wParam)) {
		case SB_TOP:
			nPos = 0;
			break;
		case SB_BOTTOM:
			nPos = si.nMax;
			break;
		case SB_LINEUP:
			if (nPos > 0) nPos -= 1;
			break;
		case SB_LINEDOWN:
			nPos += 1;
			break;
		case SB_PAGEUP:
			if (nPos > GetLinesPrPage()) nPos -= GetLinesPrPage(); else nPos = 0;
			break;
		case SB_PAGEDOWN:
			nPos += GetLinesPrPage();
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			nPos = si.nTrackPos;
			break;
	}
	if (nPos < si.nMin) nPos = si.nMin;
	if (nPos > si.nMax) nPos = si.nMax;
	if (nPos == si.nPos) return 0;
	SetScrollPos(SB_VERT, nPos, TRUE);
	m_dwPos = nPos * m_BytesPerLine;
	RecalcCaret();
	Invalidate();
	return 0;
}

LRESULT CHexEditorCtrl::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400) || defined(_WIN32_WCE)
	uMsg;
	int zDelta = (int)(short)HIWORD(wParam);
#else
	int zDelta = (uMsg == WM_MOUSEWHEEL) ? (int)(short)HIWORD(wParam) : (int)wParam;
#endif //!((_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400) || defined(_WIN32_WCE))
	for (int i = 0; i < abs(zDelta / WHEEL_DELTA * 2); i++) SendMessage(WM_VSCROLL, zDelta > 0 ? SB_LINEUP : SB_LINEDOWN);
	return 0;
}

LRESULT CHexEditorCtrl::OnNextPane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	m_bInDataPane = !m_bInDataPane;
	RecalcCaret();
	Invalidate();
	return 0;
}

LRESULT CHexEditorCtrl::OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	Undo();
	return 0;
}

LRESULT CHexEditorCtrl::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	DWORD dwStart = 0;
	DWORD dwEnd = 0;
	GetSel(dwStart, dwEnd);
	if (dwStart >= dwEnd) return 0;
	if (dwEnd - dwStart > 0x10000) {
//		if (IDNO == AtlMessageBox(m_hWnd, IDS_ERR_LARGECLIPBOARD, IDS_CAPTION_ERROR, MB_ICONQUESTION | MB_YESNO)) return 0;
	}

	CWaitCursor cursor;
	if (!::OpenClipboard(m_hWnd)) return 0;
	// Rescale string buffer
	CString sText;
	sText.GetBuffer((dwEnd - dwStart) * 4);
	sText.ReleaseBuffer(0);
	// Generate text for clipboard
	LPBYTE pData = m_pDataSource->GetData();
	if (m_bInDataPane) {
		TCHAR szBuffer[32];
		DWORD nCount = 0;
		for (DWORD i = dwStart; i < dwEnd; i++) {
			::wsprintf(szBuffer, _T("%02X "), (long) * (pData + i));
			sText += szBuffer;
			if ((++nCount % m_BytesPerLine) == 0) sText += _T("\r\n");
		}
	}
	else {
		for (DWORD i = dwStart; i < dwEnd; i++) {
			TCHAR ch = *(pData + i);
			ch = isprint((char)ch) ? ch : m_cInvalidAscii;
			sText += ch;
		}
	}
#ifdef _UNICODE
	LPSTR p = (LPSTR)malloc((sText.GetLength() + 1) * 2);
	AtlW2AHelper(p, sText, sText.GetLength() + 1);
#else
	LPCSTR p = sText;
#endif
	::EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, strlen(p) + 1);
	LPSTR lptstrCopy = (LPSTR)GlobalLock(hglbCopy);
	strcpy_s(lptstrCopy, strlen(p) + 1, p);
	GlobalUnlock(hglbCopy);
	::SetClipboardData(CF_TEXT, lptstrCopy);
	::CloseClipboard();
#ifdef _UNICODE
	free(p);
#endif
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Implementation

BOOL CHexEditorCtrl::GetPosFromPoint(POINT pt, DWORD& dwPos, bool& bInDataPane) {
	ATLASSERT(::IsWindow(m_hWnd));
	ATLASSERT(m_rcData.left > 0);

	RECT rcData = m_rcData;
	RECT rcAscii = m_rcAscii;
	::InflateRect(&rcData, 4, 0);
	::InflateRect(&rcAscii, 4, 0);
	if (::PtInRect(&rcData, pt)) {
		::OffsetRect(&rcData, -2, -2);
		auto xpos = (pt.x - rcData.left) / (((m_iDataSize * 2) + 1) * m_tmEditor.tmAveCharWidth) * m_iDataSize;
		if (xpos < 0) xpos = 0;
		if (xpos > (int)m_BytesPerLine - 1) xpos = m_BytesPerLine - 1;
		auto ypos = (pt.y - rcData.top) / GetLineHeight();
		dwPos = m_dwPos + (DWORD)xpos + ((DWORD)ypos * m_BytesPerLine);
		bInDataPane = true;
		return TRUE;
	}
	if (::PtInRect(&rcAscii, pt)) {
		::OffsetRect(&rcAscii, 4, 0);
		int xpos = (pt.x - rcAscii.left) / m_tmEditor.tmAveCharWidth;
		if (xpos < 0) xpos = 0;
		if (xpos > (int)m_BytesPerLine - 1) xpos = m_BytesPerLine - 1;
		int ypos = (pt.y - rcAscii.top) / GetLineHeight();
		dwPos = m_dwPos + (DWORD)xpos + ((DWORD)ypos * m_BytesPerLine);
		bInDataPane = false;
		return TRUE;
	}
	return FALSE;
}

void CHexEditorCtrl::RecalcCaret() {
	ATLASSERT(::IsWindow(m_hWnd));
	ATLASSERT(m_rcData.left > 0 || m_rcAscii.left > 0);   // Oops! Not initialized! Call UpdateWindow() or delay the call!!!
	// Selection-mode does not display a caret
	if (m_dwSelStart != m_dwSelEnd) return;
	// We'll try to determine where to place the caret
	DWORD dwPos = m_dwSelStart;
	int ypos = m_szMargin.cy + (((dwPos - m_dwPos) / m_BytesPerLine) * GetLineHeight());
	if (m_bInDataPane) {
		int xpos = m_rcData.left;
		xpos += (dwPos % m_BytesPerLine) / m_iDataSize * m_tmEditor.tmAveCharWidth * ((m_iDataSize * 2) + 1);
		if (m_dwDigitOffset > 0) xpos += m_tmEditor.tmAveCharWidth * m_dwDigitOffset;
		::SetCaretPos(xpos, ypos);
	}
	else {
		int xpos = m_rcAscii.left;
		xpos += ((dwPos % m_BytesPerLine) + (m_dwDigitOffset / 2)) * m_tmEditor.tmAveCharWidth;
		::SetCaretPos(xpos, ypos);
	}
}

void CHexEditorCtrl::RecalcPosition(DWORD dwPos) {
	// Is new selection-position out of bounds?
	// If so, we need to set a new view position.
	DWORD dwPage = (GetLinesPrPage() - 1) * m_BytesPerLine;
	if (dwPos < m_dwPos) {
		m_dwPos = dwPos - (dwPos % m_BytesPerLine);
		SetScrollPos(SB_VERT, m_dwPos / m_BytesPerLine);
		Invalidate();
	}
	else if (dwPos > m_dwPos + dwPage) {
		m_dwPos = dwPos - (dwPos % m_BytesPerLine);
		if (m_dwPos >= dwPage) m_dwPos -= dwPage;
		else m_dwPos = 0;
		SetScrollPos(SB_VERT, m_dwPos / m_BytesPerLine);
		Invalidate();
	}
}

void CHexEditorCtrl::AssignDigitValue(DWORD& dwPos, DWORD& dwCursorPos, BYTE bValue) {
	ATLASSERT(m_pDataSource && m_pDataSource->GetData() != nullptr);
	ATLASSERT(bValue < 0x10);
	if (dwPos == m_pDataSource->GetSize()) {
		if (!m_pDataSource->GrowBy(m_iDataSize))
			return;
	}
	ATLASSERT(dwPos < m_pDataSource->GetSize());

	// Calculate new data value (byte oriented)
	LPBYTE pData = m_pDataSource->GetData();
	DWORD dwOffset = dwPos + (m_iDataSize - 1 - (dwCursorPos / 2));
	if ((dwCursorPos % 2) == 0) bValue = (BYTE)((*(pData + dwOffset) & 0x0F) | (bValue << 4));
	else bValue = (BYTE)((*(pData + dwOffset) & 0xF0) | bValue);
	// Create undo action
	UNDOENTRY undo = { dwOffset, *(pData + dwOffset) };
	m_aUndostack.Add(undo);
	// Assign value
	*(pData + dwOffset) = bValue;
	// Advance cursor (might need to only move the caret to next digit).
	DWORD dwTotalDigits = m_iDataSize * 2;
	if (++m_dwDigitOffset >= dwTotalDigits) SetSel(dwPos + m_iDataSize, dwPos + m_iDataSize);
	else RecalcCaret();
	Invalidate(FALSE);
}

void CHexEditorCtrl::AssignCharValue(DWORD& dwPos, DWORD& dwCursorPos, BYTE bValue) {
	ATLASSERT(m_pDataSource->GetData() != nullptr);
	ATLASSERT(dwPos < m_pDataSource->GetSize());
	// Calculate new data value (cursor moves one digit; a byte i 2 digits)
	LPBYTE pData = m_pDataSource->GetData();
	DWORD dwOffset = dwPos + (dwCursorPos / 2);
	// Create undo action
	UNDOENTRY undo = { dwOffset, *(pData + dwOffset) };
	m_aUndostack.Add(undo);
	// Assign value
	*(pData + dwOffset) = bValue;
	// Advance cursor (probably to next 'char' only)
	dwCursorPos += 2;
	DWORD dwTotalDigits = m_iDataSize * 2;
	if (dwCursorPos >= dwTotalDigits) SetSel(dwPos + m_iDataSize, dwPos + m_iDataSize);
	else RecalcCaret();
	Invalidate();
}

void CHexEditorCtrl::DoPaint(CDCHandle dc) {
	RECT rcClient;
	GetClientRect(&rcClient);

	dc.FillSolidRect(&rcClient, ::GetSysColor(COLOR_WINDOW));

	rcClient.left += m_szMargin.cx;
	rcClient.top += m_szMargin.cy;

	HFONT hOldFont = dc.SelectFont(m_fontEditor);
	int nLines = GetLinesPrPage() + 1;
	int iHeight = GetLineHeight();

	LPBYTE pData = m_pDataSource->GetData();
	DWORD dwSize = m_pDataSource->GetSize();
	DWORD dwPos = m_dwPos;

	::ZeroMemory(&m_rcData, sizeof(m_rcData));
	::ZeroMemory(&m_rcAscii, sizeof(m_rcAscii));
	m_rcData.top = m_rcAscii.top = m_szMargin.cy;

	COLORREF clrTextH, clrBackH;
	COLORREF clrTextN = ::GetSysColor(m_bReadOnly ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT);
	COLORREF clrBackN = ::GetSysColor(COLOR_WINDOW);
	bool bHighlighted = false;

	dc.SetBkMode(OPAQUE);

	DWORD dwSelStart = 0;
	DWORD dwSelEnd = 0;
	GetSel(dwSelStart, dwSelEnd);

	int ypos = rcClient.top;
	TCHAR szBuffer[64] = { 0 };
	for (int i = 0; i < nLines; i++) {
		int xpos = rcClient.left;
		// Draw address text
		if (m_bShowAddress && dwPos < dwSize) {
			dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
			dc.SetBkColor(clrBackN);
			::wsprintf(szBuffer, _T("%08X  "), dwPos);
			RECT rcAddress = { xpos, ypos, xpos + 200, ypos + iHeight };
			dc.DrawText(szBuffer, -1, &rcAddress, DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOCLIP);
			xpos += m_tmEditor.tmAveCharWidth * static_cast<DWORD>(_tcslen(szBuffer));
		}
		// Draw hex values
		if (m_bShowData) {
			if (m_rcData.left == 0) m_rcData.left = xpos;

			clrBackH = ::GetSysColor(m_bInDataPane ? COLOR_HIGHLIGHT : COLOR_BTNFACE);
			clrTextH = ::GetSysColor(m_bInDataPane ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);

			dc.SetTextColor(clrTextN);
			dc.SetBkColor(clrBackN);
			bHighlighted = false;

			for (DWORD j = 0; j < m_BytesPerLine; j += m_iDataSize) {
				if (dwPos + j >= dwSize) break;

				if (dwPos + j >= dwSelStart && dwPos + j < dwSelEnd) {
					if (!bHighlighted) {
						dc.SetTextColor(clrTextH);
						dc.SetBkColor(clrBackH);
						bHighlighted = true;
					}
				}
				else {
					if (bHighlighted) {
						dc.SetTextColor(clrTextN);
						dc.SetBkColor(clrBackN);
						bHighlighted = false;
					}
				}

				LPTSTR p = szBuffer + (j == 0 ? 0 : 1);
				szBuffer[0] = ' ';
				switch (m_iDataSize) {
					case 1:
						::wsprintf(p, _T("%02X"), (long) * (pData + dwPos + j));
						break;
					case 2:
						::wsprintf(p, _T("%04X"), (long) * (LPWORD)(pData + dwPos + j));
						break;
					case 4:
						::wsprintf(p, _T("%08X"), (long) * (LPDWORD)(pData + dwPos + j));
						break;
					default:
						ATLASSERT(false);
				}
				RECT rcData = { xpos, ypos, xpos + 200, ypos + iHeight };
				dc.DrawText(szBuffer, -1, &rcData, DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOCLIP);
				xpos += m_tmEditor.tmAveCharWidth * static_cast<DWORD>(_tcslen(szBuffer));
			}

			if (m_rcData.right == 0) m_rcData.right = xpos;
		}
		// Draw ASCII representation
		if (m_bShowAscii) {
			xpos += m_tmEditor.tmAveCharWidth * 3;

			if (m_rcAscii.left == 0) m_rcAscii.left = xpos;
			xpos = m_rcAscii.left;

			clrBackH = ::GetSysColor(m_bInDataPane ? COLOR_BTNFACE : COLOR_HIGHLIGHT);
			clrTextH = ::GetSysColor(m_bInDataPane ? COLOR_WINDOWTEXT : COLOR_HIGHLIGHTTEXT);

			dc.SetTextColor(clrTextN);
			dc.SetBkColor(clrBackN);
			bHighlighted = false;

			DWORD j = 0;
			for (; j < m_BytesPerLine; j++) {
				if (dwPos + j >= dwSize) break;

				if (dwPos + j >= dwSelStart && dwPos + j < dwSelEnd) {
					if (!bHighlighted) {
						dc.SetTextColor(clrTextH);
						dc.SetBkColor(clrBackH);
						bHighlighted = true;
					}
				}
				else {
					if (bHighlighted) {
						dc.SetTextColor(clrTextN);
						dc.SetBkColor(clrBackN);
						bHighlighted = false;
					}
				}

				TCHAR ch = *(pData + dwPos + j);
				ch = isprint((char)ch) ? ch : m_cInvalidAscii;
				RECT rcAscii = { xpos, ypos, xpos + 100, ypos + iHeight };
				dc.DrawText(&ch, 1, &rcAscii, DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOCLIP);
				xpos += m_tmEditor.tmAveCharWidth;
			}

			if (m_rcAscii.right == 0) m_rcAscii.right = xpos;
		}
		dwPos += m_BytesPerLine;
		ypos += iHeight;
	}

	dc.SelectFont(hOldFont);

	m_rcData.bottom = m_rcAscii.bottom = ypos;

	// HACK: Delayed activation of first caret position.
	//       We need the sizes of m_rcData and m_rcAscii before
	//       we can set selection (position caret)!
	if (m_dwSelStart == (DWORD)-1) SetSel(0, 0);
}


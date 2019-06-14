// RegEditX.cpp : main source file for RegEditX.exe
//

#include "stdafx.h"

#include "resource.h"

#include "View.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "Internals.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT) {
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if (wndMain.CreateEx() == NULL) {
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow) {
	//OBJECT_ATTRIBUTES keyAttr;
	//UNICODE_STRING keyName;
	//RtlInitUnicodeString(&keyName, L"\\REGISTRY\\MACHINE\\SYSTEM");

	//InitializeObjectAttributes(&keyAttr, &keyName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);
	//HANDLE h;
	//auto status = NtOpenKey(&h, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &keyAttr);
	//auto info = (KEY_BASIC_INFORMATION*)malloc(1 << 12);
	////ULONG len;
	//WCHAR name[256];
	//for (DWORD i = 0; ; i++) {
	//	DWORD len = 256;
	//	//if (ERROR_SUCCESS != ::RegEnumKeyEx((HKEY)h, i, name, &len, nullptr, nullptr, nullptr, nullptr))
	//	//	break;
	//	status = NtEnumerateKey(h, i, KeyBasicInformation, info, 1 << 12, &len);
	//	if (!NT_SUCCESS(status))
	//		break;
	//	//::wcscat_s(name, L"\n");
	//	//ATLTRACE(name);
	//}

	HRESULT hRes = ::CoInitialize(nullptr);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES);

	hRes = _Module.Init(nullptr, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}

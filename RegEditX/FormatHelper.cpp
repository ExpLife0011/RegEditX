#include "stdafx.h"
#include "FormatHelper.h"


CString FormatHelper::ToBinary(ULONGLONG value) {
	CString svalue;

	while (value) {
		svalue = ((value & 1) ? L"1" : L"0") + svalue;
		value >>= 1;
	}
	if (svalue.IsEmpty())
		svalue = L"0";
	return svalue;
}

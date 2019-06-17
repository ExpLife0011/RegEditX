#include "stdafx.h"
#include "ServiceDetailsProvider.h"

CString ServiceDetailsProvider::GetDetailsForKey(RegKeyTreeNode* node) const {
	CString result;
	if (IsServicesNode(node)) {
		result = L"Services and Drivers";
	}
	return result;
}

CString ServiceDetailsProvider::GetDetailsForValue(RegKeyTreeNode* node, const CString& value) const {
	return L"";
}

bool ServiceDetailsProvider::IsServicesNode(RegKeyTreeNode* node) const {
	return (node->GetFullPath().CompareNoCase(L"\\REGISTRY\\MACHINE\\System\\CurrentControlSet\\Services") == 0 ||
		node->GetFullPath().CompareNoCase(L"\\HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Services"));
}


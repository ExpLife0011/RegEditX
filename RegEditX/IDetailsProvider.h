#pragma once

#include "TreeNodes.h"

struct IDetailsProvider {
	virtual CString GetDetailsForKey(RegKeyTreeNode* node) const = 0;
	virtual CString GetDetailsForValue(RegKeyTreeNode* node, const CString& value) const = 0;
};

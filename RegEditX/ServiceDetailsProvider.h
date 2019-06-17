#pragma once

#include "IDetailsProvider.h"

class ServiceDetailsProvider : public IDetailsProvider {
	// Inherited via IDetailsProvider
	CString GetDetailsForKey(RegKeyTreeNode* node) const override;
	CString GetDetailsForValue(RegKeyTreeNode* node, const CString& value) const override;

	bool IsServicesNode(RegKeyTreeNode* node) const;
	bool IsServiceNode(RegKeyTreeNode* node) const;
};


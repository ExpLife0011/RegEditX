#pragma once

struct ITreeOperations {
	virtual bool SelectNode(TreeNodeBase* parent, PCWSTR name) = 0;
};


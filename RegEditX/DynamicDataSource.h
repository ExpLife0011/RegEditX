#pragma once

#include "..\HexEditCtrl\HexEditCtrl.h"

class DynamicDataSource : public IDataSource {
public:
	void SetData(BYTE* data, DWORD size);

	// Inherited via IDataSource
	virtual BYTE* GetData() override;
	virtual DWORD GetSize() override;
	virtual bool GrowBy(int size) override;

private:
	std::vector<BYTE> _data;
};


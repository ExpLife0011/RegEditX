#include "stdafx.h"
#include "DynamicDataSource.h"

void DynamicDataSource::SetData(BYTE* data, DWORD size) {
	_data.resize(size);
	::memcpy(_data.data(), data, size);
}

BYTE* DynamicDataSource::GetData() {
	return _data.data();
}

DWORD DynamicDataSource::GetSize() {
	return static_cast<DWORD>(_data.size());
}

bool DynamicDataSource::GrowBy(int size) {
	_data.resize(_data.size() + size);
	return true;
}

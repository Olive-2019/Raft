#pragma once
#include "FileOperator.h"
#include "source.h"
class ServerAddressReader : public FileOperator {
	pair<int, NetWorkAddress> getOneNetWorkAddress();
public:
	ServerAddressReader(string filePath);
	// ��ȡ�����ļ��е������ַ
	map<int, NetWorkAddress> getNetWorkAddresses();
};


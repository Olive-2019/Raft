#pragma once
#include "source.h"
#include "Command.h"
class RPC
{
public:
	// ��Clerk��������
	void invokeGetData(NetWorkAddress address, int commandID, string value);
};


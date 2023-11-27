#pragma once
#include "source.h"
#include "Command.h"
class RPC
{
public:
	// ��Clerk��������
	void invokeGetData(NetWorkAddress address, int commandID, string value);
	// ��KVCtrler����join
	int invokeJoin(NetWorkAddress address, NetWorkAddress selfAddress);
	// ��KVCtrler��Ҫ��ǰgroup��Ӧ��shardid
	vector<int> invokeQueryShardID(NetWorkAddress address, int groupID);
	// ��KVCtrler��Ҫ��ǰshardid��Ӧ���µ�ַ
	NetWorkAddress invokeQueryNewGroup(NetWorkAddress address, int shardID);
	// ���õײ�kv���ݿ��acceptCommand��������kv���ݿ⴫����
	int invokeRemoteAcceptCommand(NetWorkAddress address, Command command);
};


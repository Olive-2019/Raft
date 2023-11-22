#pragma once
#include "source.h"
#include "Command.h"
class RPC
{
public:
	// ���õײ�kv���ݿ��acceptCommand��������kv���ݿ⴫����
	int invokeRemoteAcceptCommand(NetWorkAddress address, Command command);
	// ����shardCtrler��ȡ�����ַ
	NetWorkAddress invokeQueryGroupAddressByShardID(NetWorkAddress address, int shardID);
	// ����shardCtrler��ȡ�Լ��ķ��䵽��shard
	vector<int> invokeQueryShardIDByGroupID(NetWorkAddress address, int groupID);
};


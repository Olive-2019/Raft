#pragma once
#include "KVserver.h"
class ShardKV :
    public KVserver
{
	// ϵͳ�е�shard����������ȫ�ֳ�ʼ��
	const int shardNum;
	// ��ǰgroup������shard
	set<int> shardIDs;
	// ��ǰgroup��Ψһ��־
	int groupID;

	RPC rpc;


	// shardCtrler�Ǽ�join�ĵ�ַ
	NetWorkAddress shardCtrlerJoinAddress;
	// shardCtrler��ѯ����shardID�Ľӿڵ�ַ
	NetWorkAddress shardCtrlerQueryShardIDAddress;
	// shardCtrler��ѯ��group��ַ�Ľӿڵ�ַ
	NetWorkAddress shardCtrlerQueryNewGroupAddress;

	// �����key��Ӧ�ķ�Ƭid
	int getShardID(string key);
	// ���ʧȥ����Щshard
	set<int> checkDeleted(vector<int> newShardIDs);
	// ����Put����
	void sendPutShardWithShardID(NetWorkAddress address, int shardID);
	// ����AddShard����
	void sendAddShard();
	// ��ȡ������Ϣ
	void getConfig();
	// ��shardCtrler�Ǽ�join
	void join();
public:
	ShardKV(NetWorkAddress raftServerAddress, NetWorkAddress shardCtrlerJoinAddress, NetWorkAddress shardCtrlerQueryShardIDAddress,
		NetWorkAddress shardCtrlerQueryNewGroupAddress, int applyMsgPort = 8001, int acceptCommandPort = 8011, 
		string snapshotFilePath = "snapshot.data", int shardNum = 10);
	void execute(const Command& command);

};


#pragma once
#include "source.h"
class ShardCtrler
{
	// ��Ƭ����
	const int shardNum;
	// ��������
	int groupNum;

	// �����key��Ӧ�ķ�Ƭid
	int getShardID(string key);

	// ��ȡϵͳ�������е�group
	vector<int> getGroupID();

	// ���·���
	void redistribute();

	// ID�����
	int decodeGroupID(string groupID);
	int decodeShardID(string shardID);
	string codeGroupID(int groupID);
	string codeShardID(int shardID);

	// server�߳�
	vector<thread*> threads;
	vector<int> ports;
	bool debug;


public:
	ShardCtrler(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, const int shardNum, vector<int> ports);
	~ShardCtrler();
	void regsiterFunc(int index);
	//Join : �¼����Group��Ϣ��PUT��Append��,����Ψһ��ʶgroupid
	int join_(NetWorkAddress groupAdress);
	int join(rpc_conn conn, NetWorkAddress groupAdress);
	//Leave : ��ЩGroupҪ�뿪��Delete��
	void deleteGroup_(int groupID);
	void deleteGroup(rpc_conn conn, int groupID);
	//Move : ��Shard�����GID��Group, ������ԭ�����ģ�Delete Put��
	void move(int shardID, int groupID);
	//void move(rpc_conn conn, int shardID, int groupID);


	// �����ã���ӡ�����ж���shard�������Ӧ��group
	void printShardDistribution();
	//Query : ��ѯ���µ�Config��Ϣ��Get��
	// ��clientʹ�ã���ȡkey��Ӧ�ķ�Ƭ���ݿ⼯Ⱥip
	NetWorkAddress queryByKey_(string key);
	NetWorkAddress queryByKey(rpc_conn conn, string key);
	// ��ShardKVʹ�ã���ȡgroupid��Ӧ��shardIDs
	vector<int> queryShardIDByGroupID_(int groupID);
	vector<int> queryShardIDByGroupID(rpc_conn conn, int groupID);
	// ��ShardKVʹ�ã���ȡshardID��Ӧ��group��ַ
	NetWorkAddress queryGroupAddressByShardID_(int shardID);
	NetWorkAddress queryGroupAddressByShardID(rpc_conn conn, int shardID);

};


#pragma once
#include "Clerk.h"
class ShardCtrler
{
	// �ײ�KV���ݿ�
	Clerk clerk;
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
	int getGroupID(string groupID);
	int getShardID(string shardID);
	string codeGroupID(int groupID);
	string codeShardID(int shardID);

	// server�߳�
	vector<thread*> threads;
	

public:
	ShardCtrler(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, const int shardNum);

	//Join : �¼����Group��Ϣ��PUT��Append��,����Ψһ��ʶgroupid
	int join(NetWorkAddress groupAdress);
	int join(rpc_conn conn, NetWorkAddress groupAdress);
	//Leave : ��ЩGroupҪ�뿪��Delete��
	void deleteGroup(int groupID);
	void deleteGroup(rpc_conn conn, int groupID);
	//Move : ��Shard�����GID��Group, ������ԭ�����ģ�Delete Put��
	void move(int shardID, int groupID);
	//void move(rpc_conn conn, int shardID, int groupID);


	//Query : ��ѯ���µ�Config��Ϣ��Get��
	// ��clientʹ�ã���ȡkey��Ӧ�ķ�Ƭ���ݿ⼯Ⱥip
	NetWorkAddress queryByKey(string key);
	NetWorkAddress queryByKey(rpc_conn conn, string key);
	// ��ShardKVʹ�ã���ȡgroupid��Ӧ��shardIDs
	vector<int> queryShardIDByGroupID(int groupID);
	vector<int> queryShardIDByGroupID(rpc_conn conn, int groupID);
	// ��ShardKVʹ�ã���ȡshardID��Ӧ��group��ַ
	NetWorkAddress queryGroupAddressByShardID(int shardID);
	NetWorkAddress queryGroupAddressByShardID(rpc_conn conn, int shardID);

};


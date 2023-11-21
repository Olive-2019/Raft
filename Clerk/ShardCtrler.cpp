#include "ShardCtrler.h"
ShardCtrler::ShardCtrler(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, const int shardNum) 
	: clerk(kvServerAddress, getDataAddress), shardNum(shardNum), groupNum(0) {

}
// ��ȡϵͳ�������е�group
vector<int> ShardCtrler::getGroupID() {
	vector<int> groupIDs;
	for (int groupID = 1; groupID <= groupNum; ++groupID) 
		if (clerk.get(codeGroupID(groupID)).size()) groupIDs.push_back(groupID);
	return groupIDs;
}
// ��������config��������Ϊshardָ��group
void ShardCtrler::redistribute() {
	vector<int> groupIDs = getGroupID();
	int groupIDIndex = 0;
	for (int shardID = 0; shardID < shardNum; ++shardID) {
		move(shardID, groupIDs[groupIDIndex++]);
		groupIDIndex %= groupIDs.size();
	}
}
//Join : �¼����Group��Ϣ��PUT��Append��,����Ψһ��ʶgroupid
int ShardCtrler::join(NetWorkAddress groupAdress) {
	int groupID = ++groupNum;
	clerk.put(codeGroupID(groupID), groupAdress.first + "_" + to_string(groupAdress.second));
	// ���·���shard��λ��
	redistribute();
	return groupID;
}
int ShardCtrler::join(rpc_conn conn, NetWorkAddress groupAdress) {
	return join(groupAdress);
}
//Leave : ��ЩGroupҪ�뿪��Delete��
void ShardCtrler::deleteGroup(rpc_conn conn, int groupID) {
	return deleteGroup(groupID);
}
void ShardCtrler::deleteGroup(int groupID) {
	clerk.deleteData(codeGroupID(groupID));
}
//Move : ��Shard�����GID��Group, ������ԭ�����ģ�Delete Put��
void ShardCtrler::move(int shardID, int groupID) {
	clerk.put(codeShardID(shardID), codeGroupID(groupID));
}


//Query : ��ѯ���µ�Config��Ϣ��Get��
// ��clientʹ�ã���ȡkey��Ӧ�ķ�Ƭ���ݿ⼯Ⱥip
NetWorkAddress ShardCtrler::queryByKey(rpc_conn conn, string key) {
	return queryByKey(key);
}
NetWorkAddress ShardCtrler::queryByKey(string key) {
	int shardID = getShardID(key);
	return queryGroupAddressByShardID(shardID);
}
// ��ShardKVʹ�ã���ȡgroupid��Ӧ��shardIDs
vector<int> ShardCtrler::queryShardIDByGroupID(rpc_conn conn, int groupID) {
	return queryShardIDByGroupID(groupID);
}
vector<int> ShardCtrler::queryShardIDByGroupID(int groupID) {
	vector<int> shardIDs;
	for (int shardID = 0; shardID < shardNum; ++shardID) {
		int curGroupID = getGroupID(clerk.get(codeShardID(shardID)));
		if (curGroupID == groupID) shardIDs.push_back(shardID);
	}
	return shardIDs;
}
// ��ShardKVʹ�ã���ȡshardID��Ӧ��group��ַ
NetWorkAddress ShardCtrler::queryGroupAddressByShardID(rpc_conn conn, int shardID) {
	return queryGroupAddressByShardID(shardID);
}
NetWorkAddress ShardCtrler::queryGroupAddressByShardID(int shardID) {
	string groupIDStr = clerk.get(codeShardID(shardID));
	string address = clerk.get(groupIDStr);
	int len_ = address.find('_');
	string ip = address.substr(0, len_);
	int port = atoi(address.substr(len_).c_str());
	NetWorkAddress shardKVAddress(ip, port);
	return shardKVAddress;
}

// �����key��Ӧ�ķ�Ƭid
int ShardCtrler::getShardID(string key) {
	if (!key.size()) return 0;
	return key[0] % shardNum;
}
int ShardCtrler::getGroupID(string groupID) {
	return atoi(groupID.c_str()) * -1;
}
int ShardCtrler::getShardID(string shardID) {
	return atoi(shardID.c_str());
}
string ShardCtrler::codeGroupID(int groupID) {
	return to_string(groupID * -1);
}
string ShardCtrler::codeShardID(int shardID) {
	return to_string(shardID);
}
#include "ShardKV.h"
ShardKV::ShardKV(NetWorkAddress raftServerAddress, NetWorkAddress shardCtrlerJoinAddress, NetWorkAddress shardCtrlerQueryShardIDAddress,
	NetWorkAddress shardCtrlerQueryNewGroupAddress, int applyMsgPort, int acceptCommandPort, string snapshotFilePath, int shardNum)
	: KVserver(raftServerAddress, applyMsgPort, acceptCommandPort, snapshotFilePath), shardNum(shardNum), shardCtrlerJoinAddress(shardCtrlerJoinAddress),
	shardCtrlerQueryNewGroupAddress(shardCtrlerQueryNewGroupAddress), shardCtrlerQueryShardIDAddress(shardCtrlerQueryShardIDAddress), groupID(-1){
	for (int i = 0; i < shardNum; ++i) shardIDs.insert(i);
	join();
}
int ShardKV::getShardID(string key) {
	// �����key��Ӧ�ķ�Ƭid
	if (!key.size()) return 0;
	return key[0] % shardNum;
}

void ShardKV::execute(const Command& command) {
	int shardID = getShardID(command.getKey());
	if (debug) cout << "ShardKV::execute accept a comand with shard " << shardID << endl;
	if (shardIDs.find(shardID) == shardIDs.end()) return;
	if (command.getType() == CommandType::AddShard) shardIDs.insert(atoi(command.getKey().c_str()));
	else if (command.getType() == CommandType::PutShard) data[command.getKey()] = command.getValue();
	else if (shardIDs.find(shardID) != shardIDs.end()) KVserver::execute(command);
}

// ���ʧȥ����Щshard
unordered_set<int> ShardKV::checkDeleted(vector<int> newShardIDs) {
	unordered_set<int> deletedShardIDs;
	unordered_set<int> newShardIDSet;
	for (int id : newShardIDs) newShardIDSet.insert(id);
	for (auto it = shardIDs.begin(); it != shardIDs.end(); ++it) {
		if (newShardIDSet.find(*it) == newShardIDSet.end()) {
			deletedShardIDs.insert(*it);
		}
	}
	return deletedShardIDs;
}
// ����Put����
void ShardKV::sendPutShardWithShardID(int shardID) {
	NetWorkAddress newAddress = rpc.invokeQueryNewGroup(shardCtrlerQueryNewGroupAddress, shardID);
	unordered_set<pair<string, string>> shardData;
	for (auto it = data.begin(); it != data.end(); ++it) {
		if (getShardID(it->first) == shardID) {
			shardData.insert(*it);
			Command command(CommandType::Delete, NetWorkAddress("127.0.0.1", 8080), it->first);
			KVserver::acceptCommand_(command);
		}
	}
	for (auto it = shardData.begin(); it != shardData.end(); ++it) {
		Command command(CommandType::PutShard, newAddress, it->first, it->second);
		

	}
}
// ����AddShard����
void ShardKV::sendAddShard() {

}
// ��ȡ������Ϣ
void ShardKV::getConfig() {
	vector<int> newShardIDs = rpc.invokeQueryShardID(shardCtrlerQueryShardIDAddress, groupID);
	unordered_set<int> deletedShardIDs = checkDeleted(newShardIDs);

}
// ��shardCtrler�Ǽ�join
void ShardKV::join() {
	NetWorkAddress selfAddress(string("127.0.0.1"), acceptCommandPort);
	groupID = rpc.invokeJoin(shardCtrlerJoinAddress, selfAddress);
}

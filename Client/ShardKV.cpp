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
set<int> ShardKV::checkDeleted(vector<int> newShardIDs) {

}
// ����Put����
void ShardKV::sendPutShardWithShardID(NetWorkAddress address, int shardID) {

}
// ����AddShard����
void ShardKV::sendAddShard() {

}
// ��ȡ������Ϣ
void ShardKV::getConfig() {

}
// ��shardCtrler�Ǽ�join
void ShardKV::join() {
	NetWorkAddress selfAddress(to_string)
}

#include "KVserver.h"

KVserver::KVserver(NetWorkAddress raftServerAddress, int applyMsgPort, string snapshotFilePath): 
	raftServerAddress(raftServerAddress), snapshotPersistence(snapshotFilePath) {
	//NetWorkAddress raftServerAddress("127.0.0.1", 8291);
	//Raft raft;
	raft = new Raft(raftServerAddress, applyMsgPort, this);
	data = snapshotPersistence.read();
}
// ����ִ�������Raft����
void KVserver::execute(const Command& command) {
	switch (command.getType()) {
	case CommandType::Append:
		// ���ݿ��д�����׷�Ӻ�����
		if (data.find(command.getKey()) != data.end()) {
			data[command.getKey()] += command.getValue();
			break;
		}
		// ���ݿ��в����������put����
	case CommandType::Put:
		// д��
		data[command.getKey()] = command.getValue();
		break;
	default:
		// ���ݿ��в����ڣ��򷵻�һ�����ַ�����������д��cache��ȥ
		if (data.find(command.getKey()) == data.end()) readCache[command.getID()] = "";
		else readCache[command.getID()] = data[command.getKey()];
	}
}
// д����
void KVserver::snapshot() {
	snapshotPersistence.write(data);
}
// �����������Raftϵͳ
void KVserver::acceptCommand(const Command& command) {
	raft->start(command);
}
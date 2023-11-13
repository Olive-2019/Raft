#include "KVserver.h"

KVserver::KVserver(NetWorkAddress raftServerAddress, int applyMsgPort, string snapshotFilePath): 
	raftServerAddress(raftServerAddress), snapshotPersistence(snapshotFilePath),debug(false) {
	//NetWorkAddress raftServerAddress("127.0.0.1", 8291);
	//Raft raft;
	setDebug();
	raft = new Raft(raftServerAddress, applyMsgPort, this);
	try {
		data = snapshotPersistence.read();
	}
	catch (exception e) {
		cout << "welcome to KV Server with Raft" << endl;
	}
}
KVserver::~KVserver() {
	snapshotPersistence.write(data);
	delete raft;
}
void KVserver::setDebug() {
	debug = true;
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
	if (debug) cout << "KVserver::execute " << endl;
	printState();
}
// д����
void KVserver::snapshot() {
	if (debug) cout << "KVserver::snapshot" << endl;
	snapshotPersistence.write(data);
}
// �����������Raftϵͳ
void KVserver::acceptCommand(const Command& command) {
	raft->start(command);
}
bool KVserver::getData(int commandID, string& value) {
	
	if (readCache.find(commandID) == readCache.end()) return false;
	value = readCache[commandID];
	readCache.erase(commandID);
	return true;
}
void KVserver::printState() const {
	for (auto it = data.begin(); it != data.end(); ++it) 
		cout << it->first << ' ' << it->second << endl;
}

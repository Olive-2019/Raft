#include "KVserver.h"

KVserver::KVserver(NetWorkAddress raftServerAddress, int applyMsgPort, int acceptCommandPort, string snapshotFilePath):
	raftServerAddress(raftServerAddress), snapshotPersistence(snapshotFilePath),debug(false), acceptCommandPort(acceptCommandPort) {
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
	acceptCommandThread = new thread(&KVserver::registerAcceptCommand, this);
}
KVserver::~KVserver() {
	snapshotPersistence.write(data);
	delete raft;
	acceptCommandThread->join();
	delete acceptCommandThread;
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
		string value;
		if (data.find(command.getKey()) != data.end()) value = data[command.getKey()];
		rpc.invokeGetData(command.getClerkAddress(), command.getID(), value);
	}
	if (debug) {
		cout << "KVserver::execute " << endl;
		printState();
		cout << endl;
	}
	

}
// д����
void KVserver::snapshot() {
	if (debug) cout << "KVserver::snapshot" << endl;
	snapshotPersistence.write(data);
}

void KVserver::registerAcceptCommand() {
	rpc_server server(acceptCommandPort, 6);
	server.register_handler("acceptCommand", &KVserver::acceptCommand, this);
	server.run();
}

// �����������Raftϵͳ�������±�
int KVserver::acceptCommand(rpc_conn conn, const Command& command) {
	StartAnswer startAnswer = raft->start(command);
	return startAnswer.index;
}

void KVserver::printState() const {
	for (auto it = data.begin(); it != data.end(); ++it) 
		cout << it->first << ' ' << it->second << endl;
}

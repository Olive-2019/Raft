#pragma once
#include "source.h"
#include "Command.h"
#include "SnapshotPersistence.h"
#include "Raft.h"
class Raft;
class KVserver
{
	map<string, string> data;
	SnapshotPersistence snapshotPersistence;
	Raft* raft;
	NetWorkAddress raftServerAddress;
	map<int, string> readCache;
	bool debug;
	void setDebug();
public:
	KVserver(NetWorkAddress raftServerAddress, int applyMsgPort = 8001, string snapshotFilePath = "snapshot.data");
	~KVserver();
	// ����ִ�������Raft����
	void execute(const Command& command);
	// д����
	void snapshot();
	// �����������Raftϵͳ
	void acceptCommand(const Command& command);
	// ��ѯcache�е����ݣ�����鵽�ˣ�����cache��ɾ��������ֵ�����Ƿ�鵽
	bool getData(int commandID, string& value);
	// print the data in kv server
	void printState() const;
};


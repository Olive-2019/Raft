#pragma once
#include "source.h"
#include "Command.h"
#include "SnapshotPersistence.h"
#include "Raft.h"
#include "RPC.h"
class Raft;
class KVserver
{
	map<string, string> data;
	SnapshotPersistence snapshotPersistence;
	Raft* raft;
	NetWorkAddress raftServerAddress;
	RPC rpc;
	// ��ӡ������Ϣ�Ŀ���
	bool debug;
	void setDebug();
	// ��Clerk���ŵĽӿ�
	int acceptCommandPort;
	// ע�����Clerk���õĺ���
	void registerAcceptCommand();
	// Clerk accept command �ܵ��߳�
	thread* acceptCommandThread;
	// print the data in kv server
	void printState() const;
public:
	KVserver(NetWorkAddress raftServerAddress, int applyMsgPort = 8001, int acceptCommandPort = 8011, string snapshotFilePath = "snapshot.data");
	~KVserver();
	// ����ִ�������Raft����
	void execute(const Command& command);
	// д���գ���Raft����
	void snapshot();
	// �����������Raftϵͳ
	int acceptCommand(rpc_conn conn, const Command& command);
};


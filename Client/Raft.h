#pragma once
#include "StartAnswer.h"
#include "ApplyMsg.h"
#include "source.h"
#include "Command.h"
#include "KVserver.h"
using namespace rest_rpc;
class KVserver;
class Raft
{
	// ����raft��Ⱥ�ĵ�ַ
	NetWorkAddress raftServerAddress;
	// ����applyMsg�Ķ˿ں�
	int applyMsgPort;
	// ���
	vector<Command> commands;
	// �����commit�������±�
	int commitedIndex;
	// ����applyMsg���߳�
	thread* applyMsgThread;
	// debug��־λ
	bool debug;
	// applyMsg������֤ϵͳ״̬���������޸ģ�commands��commitedIndex��
	mutex stateLock;

	// KV���ݿ�ָ��
	KVserver* kvServer;

	// ִ�е�newCommitIndex
	void execute(int newCommitIndex);
	// д����
	void snapshot(int snapshotIndex);
	// ��������Ƚ��������ԭ���������������ͬ��д������commitedIndex��commitedIndex = min(commitedIndex, the first index is different from new commands)
	void updateCommands(vector<Command> newCommands);
	// ע����շ���commit��Ϣ�ĺ���
	void registerApplyMsg();
	// ����������Ϣ
	void setDebug();

	
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort, KVserver* kvServer);
	~Raft();
	// ��Raftϵͳ��������,������������Ҫ�ȴ�����ֵ
	StartAnswer start(Command command);
	// ����Raftϵͳ��applyMsg��Ϣ
	void applyMsg(rpc_conn conn, ApplyMsg applyMsg);
	void run();
};
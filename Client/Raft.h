#pragma once
#include "StartAnswer.h"
#include "ApplyMsg.h"
#include "source.h"
using namespace rest_rpc;
class Raft
{
	// ����raft��Ⱥ�ĵ�ַ
	NetWorkAddress raftServerAddress;
	// ����applyMsg�Ķ˿ں�
	int applyMsgPort;
	// ���
	vector<string> commands;
	// �����commit�������±�
	int commitedIndex;
	// ����applyMsg���߳�
	thread* applyMsgThread;
	// debug��־λ
	bool debug;
	// applyMsg������֤ϵͳ״̬���������޸ģ�commands��commitedIndex��
	mutex stateLock;

	// ִ�е�newCommitIndex
	void execute(int newCommitIndex);
	// д����
	void snapshot(int snapshotIndex);
	// ��������Ƚ��������ԭ���������������ͬ��д������commitedIndex��commitedIndex = min(commitedIndex, the first index is different from new commands)
	void updateCommands(vector<string> newCommands);
	// ע����շ���commit��Ϣ�ĺ���
	void registerApplyMsg();
	// ����������Ϣ
	void setDebug();
	// ��Raftϵͳ��������,������������Ҫ�ȴ�����ֵ
	StartAnswer start(string command);
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort);
	~Raft();
	// ����Raftϵͳ��applyMsg��Ϣ
	void applyMsg(rpc_conn conn, ApplyMsg applyMsg);
	void run();
};
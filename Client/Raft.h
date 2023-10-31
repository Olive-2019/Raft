#pragma once
#include "StartAnswer.h"
#include "ApplyMsg.h"
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

	// ִ�е�newCommitIndex
	void execute(int newCommitIndex);
	// д����
	void snapshot();
	// ��������Ƚ��������ԭ���������������ͬ��д������commitedIndex��commitedIndex = min(commitedIndex, the first index is different from new commands)
	void updateCommands(vector<string> newCommands);
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort);
	~Raft();
	// ��Raftϵͳ��������
	// ������������Ҫ�ȴ�����ֵ
	StartAnswer start(string command);
	// ����Raftϵͳ��applyMsg��Ϣ
	void applyMsg(rpc_conn conn, ApplyMsg applyMsg);
	// ע����շ���commit��Ϣ�ĺ���
	void registerApplyMsg();
	void setDebug();
	void run();
};


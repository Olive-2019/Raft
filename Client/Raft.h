#pragma once
#include "StartAnswer.h"
using namespace rest_rpc;
class Raft
{
	NetWorkAddress raftServerAddress;
	int applyMsgPort;
	vector<string> commands;
	int commitedIndex;
	thread* applyMsgThread;
	bool debug;
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort);
	~Raft();
	// ��Raftϵͳ��������
	// ������������Ҫ�ȴ�����ֵ
	StartAnswer start(string command);
	// ����Raftϵͳ��applyMsg��Ϣ
	bool applyMsg(string command, int index);
	// ע����շ���commit��Ϣ�ĺ���
	void registerApplyMsg();
	void setDebug();
	void run();
};


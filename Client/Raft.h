#pragma once
#include "StartAnswer.h"
using namespace rest_rpc;
class Raft
{
	NetWorkAddress raftServerAddress;
	NetWorkAddress applyMsgAddress;
	vector<string> commands;
	int commitedIndex;
public:
	Raft(NetWorkAddress raftServerAddress);
	// ��Raftϵͳ��������
	// ������������Ҫ�ȴ�����ֵ
	StartAnswer start(string command);
	// ����Raftϵͳ��applyMsg��Ϣ
	bool applyMsg(string command, int index);
	// ע����շ���commit��Ϣ�ĺ���
	void registerApplyMsg();
};


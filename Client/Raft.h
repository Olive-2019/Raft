#pragma once
#include "StartAnswer.h"
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
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort);
	~Raft();
	// ��Raftϵͳ��������
	// ������������Ҫ�ȴ�����ֵ
	StartAnswer start(string command);
	// ����Raftϵͳ��applyMsg��Ϣ
	void applyMsg(rpc_conn conn, string command, int index);
	// ע����շ���commit��Ϣ�ĺ���
	void registerApplyMsg();
	void setDebug();
	void run();
};


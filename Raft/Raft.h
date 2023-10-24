#pragma once
#include "State.h"
#include "Follower.h"
#include "Leader.h"
#include "Candidate.h"
class Raft
{
	
	State* state;
public:
	
	Raft() : state(NULL){
	}
	// ��client���ã���״̬�������������ֵ��index��term���Կո���
	string start(string command);
	// ��ɺ���client���Ϳ���ִ�и����������
	void applyMsg(string command, int index);
	// ����״̬��������״̬���ӿ��л�״̬
	void run(int serverID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		 NetWorkAddress startAddress);
};


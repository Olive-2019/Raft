#pragma once
#include "State.h"
#include "Follower.h"
#include "Leader.h"
#include "Candidate.h"
#include "PersistenceInfoReaderAndWriter.h"
class Raft
{
	
	State* state;
public:
	
	Raft() : state(NULL){
	}
	// ����״̬��������״̬���ӿ��л�״̬
	void run(int serverID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		 NetWorkAddress startAddress, NetWorkAddress applyMessageAddress);
};


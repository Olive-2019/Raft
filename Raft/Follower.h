#pragma once
#include "State.h"
#include "Candidate.h"
class Follower : public State
{
	// �Ƚϴ����¼���Լ��ĸ�����
	bool isNewerThanMe(int lastLogIndex, int lastLogTerm) const;
	// ��һ��״̬
	State* nextState;
	// ���㳬ʱ���߳�
	void timeoutCounterThread();
public:
	Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// ���иû���������ֵ����һ��״̬
	State* run();
};


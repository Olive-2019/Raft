#pragma once
#include "State.h"
#include "Candidate.h"
class Follower : public State
{
	// ��¼leaderID
	int leaderID;
	// �Ƚϴ����¼���Լ��ĸ�����
	bool isNewerThanMe(int lastLogIndex, int lastLogTerm) const;
	
public:
	Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	~Follower();
	// start����,ת����leader
	void start(AppendEntries newEntries);
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// ���иû���������ֵ����һ��״̬
	State* run();
};


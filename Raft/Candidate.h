#pragma once
#include "State.h"
class Candidate : public State
{
	// ͶƱ��� 0:û���յ� 1��Ӯ�ø�ѡƱ -1������
	vector<int> voteResult;
	int getVote;
public:
	Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
};


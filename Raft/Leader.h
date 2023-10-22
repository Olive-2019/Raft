#pragma once
#include "State.h"
class Leader : public State
{
	// ��Ҫ����ÿһ��follower����һ��log entry����ʼ��ֵ��leader�����һ��log entry����һ��ֵ��
	vector<int> nextIndex;
	// ÿ��follower��ǰƥ�䵽��һ��log entry����ʼ��ֵΪ0��
	vector<int> matchIndex;

	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// ���иû���������ֵ����һ��״̬
	State* run();
};


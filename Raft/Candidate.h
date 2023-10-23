#pragma once
#include "State.h"

class Candidate : public State
{
	// ͶƱ��� 0:û���յ� 1��Ӯ�ø�ѡƱ -1������
	map<int, int> voteResult;
	//vector<int> voteResult;
	int getVoteCounter;
	// ���м��ͶƱ
	void work();
	// ����ͶƱ��Ϣ
	void checkRequestVote();
	
	// ���ͶƱ�����ֻ�з���trueʱ֪��ѡ�ٳɹ�������false��ζ��δ֪����ѡ��ʧ�ܿ�appendEntries
	bool checkVoteResult();
public:
	Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// ���иû���������ֵ����һ��״̬
	State* run();

};


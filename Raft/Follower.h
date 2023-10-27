#pragma once
#include "State.h"
#include "Candidate.h"
class Follower : public State
{
	// ���ڼ��㳬ʱ����
	TimeoutCounter timeoutCounter;
	// ��ʱ���߳�
	thread* timeoutThread;
	// ���㳬ʱ���̣߳�����ʱ����������������̶߳��˳������������Ҫ������ǰ�����̣߳����Ե��ý�����ʱ���ĺ���
	void timeoutCounterThread();

	// ��¼leaderID
	int leaderID;
	// �Ƚϴ����¼���Լ��ĸ�����
	bool isNewerThanMe(int lastLogIndex, int lastLogTerm) const;
	// ����ͳһ�ӿڣ���follower�������һ���ػ��߳�
	void work();
public:
	Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
		int votedFor = -1);
	~Follower();
	// ����start����,ת����leader
	void start(AppendEntries newEntries);
	// ����RequestVote
	Answer requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	Answer appendEntries(string appendEntriesCodedIntoString);
};


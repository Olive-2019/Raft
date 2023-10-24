#pragma once
#include "State.h"

class Candidate : public State
{

	// ���ڼ��㳬ʱ����
	TimeoutCounter timeoutCounter;
	// ��ʱ���߳�
	thread* timeoutThread;
	// ���㳬ʱ���̣߳�����ʱ����������������̶߳��˳������������Ҫ������ǰ�����̣߳����Ե��ý�����ʱ���ĺ���
	void timeoutCounterThread();

	// ͶƱ��� 0:û���յ� 1��Ӯ�ø�ѡƱ -1������
	map<int, int> voteResult;
	// ��õ�ѡƱ����
	int getVoteCounter;
	
	// ���̣߳����м��ͶƱ
	void work();
	// ��鷢�͵�ͶƱ��Ϣ������ֵΪtrue���������� false������δ���ص�ֵ
	bool checkRequestVote();
	// ���ݵ�ǰ��Ϣ������requestVote
	void sendRequestVote(int followerID);
	
	// ���ͶƱ�����ֻ�з���trueʱ֪��ѡ�ٳɹ�������false��ζ��δ֪����ѡ��ʧ�ܿ�appendEntries
	bool checkVoteResult();

	// �����첽��������/����ֵ��future
	map<int, shared_future<string>> followerReturnVal;
	
public:
	Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor = -1);
	~Candidate();
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// ���иû���������ֵ����һ��״̬
	State* run();

};


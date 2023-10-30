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

	// ע��ȴ�����AppendEntries���
	void registerHandleAppendEntries();
	// ע��ͶƱ�߳�RequestVote���
	void registerHandleRequestVote();
	// ע��start�������
	void registerHandleStart();
public:
	Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
		int votedFor = -1);
	~Follower();
	// ����start����,ת����leader
	StartAnswer start(rpc_conn conn, string command);
	// ����RequestVote
	Answer requestVote(rpc_conn conn, RequestVote requestVote);
	// ����AppendEntries
	Answer appendEntries(rpc_conn conn, AppendEntries appendEntries);
};


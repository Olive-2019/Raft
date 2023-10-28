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
	// ���ܾ��Ĵ���
	int rejectCounter;
	
	// ���̣߳����м��ͶƱ
	void work();
	// ��鷢�͵�ͶƱ��Ϣ������ֵΪtrue���������� false������δ���ص�ֵ
	bool checkRequestVote();
	// ���ݵ�ǰ��Ϣ�����Է���requestVote������ֵ��ʾ�Ƿ��ͳɹ�
	bool sendRequestVote(int followerID);

	// ��鵥��follower�����ɹ���true�������ɹ������ط�
	bool checkOneFollowerReturnValue(int followerID, Answer& ans);


	// ���ͶƱ�����ֻ�з���trueʱ֪��ѡ�ٳɹ�������false��ζ��δ֪����ѡ��ʧ�ܿ�appendEntries
	bool checkVoteResult();

	// �����첽��������/����ֵ��future
	map<int, vector<shared_future<Answer>>> followerReturnVal;
	// �����ط�����
	int maxResendNum;

	// ע��ȴ�����AppendEntries���
	void registerHandleAppendEntries();
	// ע��ͶƱ�߳�RequestVote���
	void registerHandleRequestVote();
	// ע��start�������
	void registerHandleStart();
public:
	Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
		int votedFor = -1, int maxResendNum = 3);
	~Candidate();
	// ����RequestVote
	Answer requestVote(rpc_conn conn, string requestVoteCodedIntoString);
	// ����AppendEntries
	Answer appendEntries(rpc_conn conn, string appendEntriesCodedIntoString);

};


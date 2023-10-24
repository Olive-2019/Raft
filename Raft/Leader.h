#pragma once
#include "State.h"

class Leader : public State
{
	// ��Ҫ����ÿһ��follower����һ��log entry����ʼ��ֵ��leader�����һ��log entry����һ��ֵ��
	map<int, int> nextIndex;
	// ÿ��follower��ǰƥ�䵽��һ��log entry����ʼ��ֵΪ0��
	map<int, int> matchIndex;
	// �����첽��������/����ֵ��future
	map<int, shared_future<string>> followerReturnVal;
	// ��¼��һ�����������ط�
	map<int, AppendEntries> lastAppendEntries;

	

	

	// ���̣߳���� ����appendEntries commit
	void work();
	// ����commitIndex
	void updateCommit();
	// �������follower���ط����·���
	void checkFollowers();

	// ��ָ��ID��follower����appendEntries������Ϊ��״̬����[start,end]�����ݣ���start<0��Ϊ�յ�������Ϣ
	void sendAppendEntries(int followerID, int start, int end);
	// �ط�ָ��follower�İ�
	void resendAppendEntries(int followerID);
	
	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// ������������߳�join��delete���̶߳��������
	~Leader();
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// ���иû���������ֵ����һ��״̬
	State* run();
};


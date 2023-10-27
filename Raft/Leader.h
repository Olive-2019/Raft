#pragma once
#include "State.h"

class Leader : public State
{
	// ��Ҫ����ÿһ��follower����һ��log entry����ʼ��ֵ��leader�����һ��log entry����һ��ֵ��
	map<int, int> nextIndex;
	// ÿ��follower��ǰƥ�䵽��һ��log entry����ʼ��ֵΪ0��
	map<int, int> matchIndex;
	
	// �����첽��������/����ֵ��future
	map<int, vector<shared_future<string>>> followerReturnVal;
	// ����ط�����
	int maxResendNum;

	// ��¼��һ�����������ط�
	map<int, AppendEntries> lastAppendEntries;

	

	

	// ���̣߳���� ����appendEntries commit
	void work();
	// ����commitIndex
	void updateCommit();
	// �������follower���ط����·���
	void checkFollowers();
	// ��鵥��follower�����ɹ���true�������ɹ������ط�
	bool checkOneFollowerReturnValue(int followerID);
	// ��ȡ����follower�ķ���ֵ
	Answer getOneFollowerReturnValue(int followerID);

	// ��ָ��ID��follower����appendEntries������Ϊ��״̬����[start,end]�����ݣ���start<0��Ϊ�յ�������Ϣ(��װ��AppendEntries)
	void sendAppendEntries(int followerID, int start, int end);
	// ��ָ��follower�İ�������ֵ�����ܲ��ܷ�������һ����AppendEntries�ط���
	bool sendAppendEntries(int followerID);
	
	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
		int votedFor = -1, int maxResendNum = 3);
	// ������������߳�join��delete���̶߳��������
	~Leader();
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
};


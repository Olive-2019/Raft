#pragma once
#include "State.h"

class Leader : public State
{
	// ��Ҫ����ÿһ��follower����һ��log entry����ʼ��ֵ��leader�����һ��log entry����һ��ֵ��
	map<int, int> nextIndex;
	// ÿ��follower��ǰƥ�䵽��һ��log entry����ʼ��ֵΪ0��
	map<int, int> matchIndex;
	
	// �����첽��������/����ֵ��future
	map<int, vector<shared_future<Answer>>> followerReturnVal;
	// �����첽����snapshot��future
	map<int, vector<shared_future<Answer>>> snapshotReturnVal;
	// ����ط�����
	int maxResendNum;
	// ������ֵ
	int snapshotThreshold;

	// ��¼��һ�����������ط�
	map<int, AppendEntries> lastAppendEntries;
	map<int, AppendEntries> snapshotLastAppendEntries;

	

	

	// ���̣߳���� ����appendEntries commit
	void work();
	// ����commitIndex
	void updateCommit();


	// �������follower���ط����·���
	void checkFollowers();
	// ��鵥��follower�����ɹ���true�������ɹ������ط�
	bool checkOneFollowerReturnValue(int followerID, bool snapshot = false);
	// ��ȡ����follower�ķ���ֵ
	Answer getOneFollowerReturnValue(int followerID);

	// ��ָ��ID��follower����appendEntries������Ϊ��״̬����[start,end]�����ݣ���start<0��Ϊ�յ�������Ϣ(��װ��AppendEntries)
	void sendAppendEntries(int followerID, int start, int end, bool snapshot = false, int snapshotIndex = -1);
	// ��ָ��follower�İ�������ֵ�����ܲ��ܷ�������һ����AppendEntries�ط���
	bool sendAppendEntries(int followerID, bool snapshot = false);

	// ע��ȴ�����AppendEntries���
	void registerHandleAppendEntries();
	// ע��ͶƱ�߳�RequestVote���
	void registerHandleRequestVote();
	// ע��start�������
	void registerHandleStart();

	// ���սӿ�
	void snapshot();
	// ������֪ͨfollowerд����
	void informSnapshot(int snapshotIndex);
	// ���ն�ϵͳ״̬�ĸı�
	void snapShotModifyState(int snapshotIndex);
	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, 
		vector<LogEntry> logEntries, int votedFor = -1, int maxResendNum = 3, int snapshotThreshold = 10);
	// ������������߳�join��delete���̶߳��������
	~Leader();
	// ����RequestVote
	Answer requestVote(rpc_conn conn, RequestVote requestVote);
	// ����AppendEntries
	Answer appendEntries(rpc_conn conn, AppendEntries appendEntries);
};


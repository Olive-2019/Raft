#pragma once
#include "State.h"
class Leader : public State
{
	// ��Ҫ����ÿһ��follower����һ��log entry����ʼ��ֵ��leader�����һ��log entry����һ��ֵ��
	vector<int> nextIndex;
	// ÿ��follower��ǰƥ�䵽��һ��log entry����ʼ��ֵΪ0��
	vector<int> matchIndex;

	// ״̬�����ڽ���start��ip��port
	NetWorkAddress startAddress;

	/*�����̵߳�����ָ��*/
	// ���ڿ��ƽ���AppendEntries�߳�
	unique_ptr<rpc_server> startRpcServer;

	// leader�Ĺ�������
	void work();
	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// ����RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// ����AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// start���ã����һ���µ�entries
	void start(AppendEntries newEntries);
	// ע��start����
	void registerStart();
	// ���иû���������ֵ����һ��״̬
	State* run();
};


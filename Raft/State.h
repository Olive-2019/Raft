#pragma once
#include "source.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include "ServerAddressReader.h"
#include "RequestVote.h"
#include "AppendEntries.h"
#include <map>
using std::pair;
using std::map;

class State
{
protected:
	/*��Ҫ�־û���state*/
	//server������term��Ҳ�Ǹ�server����֪������term
	int currentTerm;
	//�����-1�����term����δͶƱ�������candidateID�����term�е�ѡ���Ѿ�ͶƱ���˸�candidate
	int votedFor;
	// ��ǰserver������log entries��ÿһ��log entry���������term���
	vector<LogEntry> logEntries;
	//״̬��id
	int ID;
	// ״̬�����ڽ���appendEntries��ip��port
	NetWorkAddress appendEntriesAddress;
	// ״̬�����ڽ���requestVote��ip��port
	NetWorkAddress requestVoteAddress;
	// ��Ⱥ�и��������ĵ�ַ(leader���ڷ���AppendEntries��candidate���ڷ���requestVote��follower����ת������)
	map<int, NetWorkAddress> serverAddress;


	/*��ʧ״̬������Ҫ�־û�*/
	// ��ǰ״̬
	ServerState state;
	//�����Ѿ�commit��log entries index
	int commitIndex;
	//���¼����log entries index
	int lastApplied;
	
	/*���й�������Ҫ�õ��ı���*/
	// ���ڼ��㳬ʱ����
	TimeoutCounter timeoutCounter;




	// ���㳬ʱ���߳�
	void timeoutCounterThread();
	// �ȴ�����AppendEntries
	virtual string appendEntries(string appendEntriesCodedIntoString) = 0;
	// ע��ȴ�����AppendEntries
	void registerAppendEntries();
	// ͶƱ�߳�RequestVote
	virtual string requestVote(string requestVoteCodedIntoString) = 0;
	// ע��ͶƱ�߳�RequestVote
	void registerRequestVote();
public:
	State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,NetWorkAddress requestVoteAddress, 
		ServerState state, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// ���иû���������ֵ����һ��״̬
	virtual State* run() = 0;
};


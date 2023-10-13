#pragma once
#include "ServerState.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include <map>
using std::pair;
class State
{
protected:
	typedef pair<string, int> psi;
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
	psi appendEntriesAddress;
	// ״̬�����ڽ���requestVote��ip��port
	psi requestVoteAddress;

	/*��ʧ״̬������Ҫ�־û�*/
	// ��ǰ״̬
	ServerState state;
	//�����Ѿ�commit��log entries index
	int commitIndex;
	//���¼����log entries index
	int lastApplied;
	// ���ڼ��㳬ʱ����
	TimeoutCounter timeoutCounter;


	// ���㳬ʱ���߳�
	void timeoutCounterThread();
	// �ȴ�����AppendEntries
	void receiveAppendEntries();
	// ע��ȴ�����AppendEntries
	void registerReceiveAppendEntries();
	// ע��ͶƱ�߳�RequestVote
	void registerReceiveRequestVote();
	// ͶƱ�߳�RequestVote
	void receiveRequestVote();
public:
	// ���иû���������ֵ����һ��״̬
	virtual State* run();
};


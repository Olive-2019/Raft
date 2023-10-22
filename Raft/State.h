#pragma once
#include "source.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include "ServerAddressReader.h"
#include "RequestVote.h"
#include "AppendEntries.h"
#include "Answer.h"
#include <thread>
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using std::unique_ptr;
using namespace rest_rpc;
using std::thread;
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
	//�����Ѿ�commit��log entries index
	int commitIndex;
	//���¼����log entries index
	int lastApplied;
	
	/*���й�������Ҫ�õ��ı���*/
	// ���ڼ��㳬ʱ����
	TimeoutCounter timeoutCounter;
	// ��ʱ���߳�
	thread* timeoutThread;
	// AppendEntries�߳�
	thread* appendEntriesThread;
	// RequestVote�߳�
	thread* requestVoteThread;

	/*�����̵߳�����ָ��*/
	// ���ڿ��ƽ���AppendEntries�߳�
	unique_ptr<rpc_server> appendEntriesRpcServer;
	// ���ڿ��ƽ���RequestVote�߳�
	unique_ptr<rpc_server> requestVoteRpcServer;

	// ��һ��״̬���ǿ�˵��Ҫ������һ��״̬�ˣ���������ѭ����stop
	State* nextState;
	// ������Ϣ�Ĵ�����������ͬʱ��������һ�����յ���Ϣ������appendEntries��requestVote
	mutex receiveInfoLock;

	// ���㳬ʱ���߳�
	virtual void timeoutCounterThread();
	// ע��ȴ�����AppendEntries
	void registerAppendEntries();
	// ע��ͶƱ�߳�RequestVote
	void registerRequestVote();

	// ֹͣ����ͶƱ�������߳�
	virtual void stopThread();
	// �ȴ�����ͶƱ�������߳�join
	virtual void waitThread();
	// ���entries������ֵ��ʾ�Ƿ�ɹ����
	bool appendEntriesReal(int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries);
public:
	State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,NetWorkAddress requestVoteAddress, 
	 int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// ��ȡ��ǰcurrentTerm
	int getCurrentTerm() const;
	// �ȴ�����AppendEntries
	virtual string appendEntries(string appendEntriesCodedIntoString) = 0;
	// ͶƱ�߳�RequestVote
	virtual string requestVote(string requestVoteCodedIntoString) = 0;
	// ���иû���������ֵ����һ��״̬
	virtual State* run();
};


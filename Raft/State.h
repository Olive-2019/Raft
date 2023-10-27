#pragma once
#include "source.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include "ServerAddressReader.h"
#include "RequestVote.h"
#include "AppendEntries.h"
#include "Answer.h"
#include <thread>
#include "RPC.h"
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using std::unique_ptr;
using namespace rest_rpc;
using std::thread;
using std::lock_guard;
using std::future_status;

class State
{
	// ���վ������
	int handleNum;
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

	// ״̬�����ڽ���start��ip��port
	NetWorkAddress startAddress;
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
	
	// AppendEntries�߳�
	thread* appendEntriesThread;
	// RequestVote�߳�
	thread* requestVoteThread;
	// ����start��Ϣ���߳�ָ��
	thread* startThread;

	/*�����̵߳�����ָ��*/
	// ���ڿ��ƽ���AppendEntries�߳�
	unique_ptr<rpc_server> startRpcServer;
	// ���ڿ��ƽ���AppendEntries�߳�
	unique_ptr<rpc_server> appendEntriesRpcServer;
	// ���ڿ��ƽ���RequestVote�߳�
	unique_ptr<rpc_server> requestVoteRpcServer;

	// ��һ��״̬���ǿ�˵��Ҫ������һ��״̬�ˣ���������ѭ����stop
	State* nextState;
	// ������Ϣ�Ĵ�����������ͬʱ��������һ�����յ���Ϣ������appendEntries��requestVote
	mutex receiveInfoLock;
	// ����rpc��Ϣ
	RPC rpc;

	// ע����������
	void registerServer();
	// ע��start����
	void registerStart();
	// ע��ȴ�����AppendEntries
	void registerAppendEntries();
	// ע��ͶƱ�߳�RequestVote
	void registerRequestVote();

	
	// ���entries������ֵ��ʾ�Ƿ�ɹ����
	bool appendEntriesReal(int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries);
	// ���߳� leader��candidate��������follower�ػ�
	virtual void work() = 0;

	// debug�������
	bool debug;
public:
	// ���캯����ɳ�ʼ�����������̺߳ͼ�ʱ���̵߳�����
	State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,NetWorkAddress requestVoteAddress, 
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor = -1, int handleNum = 10);
	// ������������߳�join��delete���̶߳��������
	virtual ~State();

	// ����debugģʽ
	void setDebug() { debug = true; };
	// ��ȡ��ǰcurrentTerm
	int getCurrentTerm() const;

	// start���ã�leader��candidate���һ���µ�entries��followerת����leader
	virtual void start(AppendEntries newEntries);

	// �ȴ�����AppendEntries
	virtual string appendEntries(string appendEntriesCodedIntoString) = 0;

	// ͶƱ�߳�RequestVote
	virtual string requestVote(string requestVoteCodedIntoString) = 0;

	// ���иû���������ֵ����һ��״̬
	virtual State* run();
};


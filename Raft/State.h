#pragma once
#include "source.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include "ServerAddressReader.h"
#include "RequestVote.h"
#include "AppendEntries.h"
#include "Answer.h"
#include "StartAnswer.h"
#include <thread>
#include "RPC.h"
#include "PersistenceInfoReaderAndWriter.h"


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

	/*�����û����������ж���*/
	//״̬��id
	int ID;

	/*��ʵֻ��Ҫport����ʱ�佫�ӿڸ���*/
	// ״̬�����ڽ���start��ip��port
	NetWorkAddress startAddress;
	// ״̬�����ڽ���appendEntries��ip��port
	NetWorkAddress appendEntriesAddress;
	// ״̬�����ڽ���requestVote��ip��port
	NetWorkAddress requestVoteAddress;
	// ��Ⱥ�и��������ĵ�ַ(leader���ڷ���AppendEntries��candidate���ڷ���requestVote��follower����ת������)
	map<int, NetWorkAddress> serverAddress;
	// ״̬�����ڷ���applyMsg�ĵ�ַ
	NetWorkAddress applyMessageAddress;


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
	// ע��start�������
	virtual void registerHandleStart() = 0;
	// ע��ȴ�����AppendEntries
	void registerAppendEntries();
	// ע��ȴ�����AppendEntries���
	virtual void registerHandleAppendEntries() = 0;
	// ע��ͶƱ�߳�RequestVote
	void registerRequestVote();
	// ע��ͶƱ�߳�RequestVote���
	virtual void registerHandleRequestVote() = 0;
	
	// ���entries������ֵ��ʾ�Ƿ�ɹ����
	bool appendEntriesReal(int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries);
	// ���߳� leader��candidate��������follower�ػ�
	virtual void work() = 0;

	// debug�������
	bool debug;
	// ��ӡ״̬��Ϣ
	void printState();

	// ģ��������ҵ�
	bool crush(double rate) const;

	// �־û�
	void persistence() const;



	// ���ն�ϵͳ״̬�ĸı�
	void snapShotModifyState(int snapshotIndex);
	// ���͸��µ�commit��Ϣ���첽֪ͨ�ϲ�Ӧ��
	void applyMsg(bool snapshot = false, int snapshotIndex = -1);
public:
	// ���캯����ɳ�ʼ�����������̺߳ͼ�ʱ���̵߳�����
	State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,NetWorkAddress requestVoteAddress, 
		NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor = -1, int handleNum = 10);
	// ������������߳�join��delete���̶߳��������
	virtual ~State();

	// ����debugģʽ
	void setDebug() { debug = true; };
	// ��ȡ��ǰcurrentTerm
	int getCurrentTerm() const;

	// start���ã�leader��candidate���һ���µ�entries��followerת����leader
	virtual StartAnswer start(rpc_conn conn, Command command);

	// �ȴ�����AppendEntries
	virtual Answer appendEntries(rpc_conn conn, AppendEntries appendEntries) = 0;

	// ͶƱ�߳�RequestVote
	virtual Answer requestVote(rpc_conn conn, RequestVote requestVote) = 0;

	// ���иû���������ֵ����һ��״̬
	virtual State* run();
};


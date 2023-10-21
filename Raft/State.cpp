#include "State.h"

State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	 int commitIndex, int lastApplied, vector<LogEntry> logEntries):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress),
	requestVoteAddress(requestVoteAddress), commitIndex(commitIndex), lastApplied(lastApplied), 
	logEntries(logEntries){
	// ͶƱ�����Ϊ-1����˭��ûͶ
	votedFor = -1;
	// ���뼯Ⱥ������server�ĵ�ַ
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
}
State* State::run() {
	// ������ʱ��
	timeoutThread = new thread(timeoutCounterThread, this);
	// ����AppendEntries
	appendEntriesThread = new thread(registerAppendEntries, this);
	// ����RequestVote
	requestVoteThread = new thread(registerRequestVote, this);
	return NULL;
}
void State::timeoutCounterThread() {
	timeoutCounter.run();
	// �������߳���ִ�е�ָ���ÿ�
	stopThread();
}

// ע��ȴ�����AppendEntries
void State::registerAppendEntries() {
	appendEntriesRpcServer.reset(new rpc_server(appendEntriesAddress.second, 6));
	appendEntriesRpcServer->register_handler("appendEntries", appendEntries);
	appendEntriesRpcServer->run();//���������
	cout << "State::registerAppendEntries close AppendEntries" << endl;
}

// ע��ͶƱ�߳�RequestVote
void State::registerRequestVote() {
	requestVoteRpcServer.reset(new rpc_server(requestVoteAddress.second, 6));
	requestVoteRpcServer->register_handler("requestVote", requestVote);
	requestVoteRpcServer->run();//���������
	cout << "State::registerRequestVote close RequestVote" << endl;
}

void State::stopThread() {
	requestVoteRpcServer.reset(nullptr);
	appendEntriesRpcServer.reset(nullptr);
}
void State::waitThread() {
	timeoutThread->join();
	appendEntriesThread->join();
	requestVoteThread->join();
}
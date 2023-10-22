#include "State.h"
#include "Candidate.h"
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
	timeoutThread = new thread(&State::timeoutCounterThread, this);
	// ����AppendEntries
	appendEntriesThread = new thread(&State::registerAppendEntries, this);
	// ����RequestVote
	requestVoteThread = new thread(&State::registerRequestVote, this);
	return NULL;
}
void State::timeoutCounterThread() {
	// ��ʱ���أ�ת����candidate
	if (timeoutCounter.run())
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			commitIndex, lastApplied, logEntries);
	// δ��ʱ���������أ���nextState�ĳ�ʼ������stop�ĵ��ô�
	// �������߳���ִ�е�ָ���ÿ�
	stopThread();
}

// ע��ȴ�����AppendEntries
void State::registerAppendEntries() {
	appendEntriesRpcServer.reset(new rpc_server(appendEntriesAddress.second, 6));
	appendEntriesRpcServer->register_handler("appendEntries", [this](rpc_conn conn,
		string appendEntriesCodedIntoString) {
			this->appendEntries(std::move(appendEntriesCodedIntoString));
		});
	//appendEntriesRpcServer->register_handler("appendEntries", appendEntries);
	appendEntriesRpcServer->run();//���������
	cout << "State::registerAppendEntries close AppendEntries" << endl;
}

// ע��ͶƱ�߳�RequestVote
void State::registerRequestVote() {
	requestVoteRpcServer.reset(new rpc_server(requestVoteAddress.second, 6));
	requestVoteRpcServer->register_handler("requestVote", [this](rpc_conn conn,
		string requestVoteCodedIntoString) {
			this->requestVote(std::move(requestVoteCodedIntoString));
		});
	//requestVoteRpcServer->register_handler("requestVote", requestVote);
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
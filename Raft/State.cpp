#include "State.h"
#include "Candidate.h"
State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	 int commitIndex, int lastApplied, vector<LogEntry> logEntries):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress),
	requestVoteAddress(requestVoteAddress), commitIndex(commitIndex), lastApplied(lastApplied), 
	logEntries(logEntries), nextState(NULL){
	// ͶƱ�����Ϊ-1����˭��ûͶ
	votedFor = -1;
	// ���뼯Ⱥ������server�ĵ�ַ
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();

	// ������ʱ��
	timeoutThread = new thread(&State::timeoutCounterThread, this);
	// ����AppendEntries
	appendEntriesThread = new thread(&State::registerAppendEntries, this);
	// ����RequestVote
	requestVoteThread = new thread(&State::registerRequestVote, this);
}
State::~State() {
	timeoutThread->join();
	appendEntriesThread->join();
	requestVoteThread->join();
	delete timeoutThread;
	delete appendEntriesThread;
	delete requestVoteThread;
}

void State::timeoutCounterThread() {
	// ��ʱ���أ�ת����candidate
	if (timeoutCounter.run())
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			commitIndex, lastApplied, logEntries);
	// δ��ʱ���������أ���nextState�ĳ�ʼ������stop�ĵ��ô�
	// �������Ľ����̶߳�ͣ�ˣ�����������������ͨ������ֹͣtimeout�ĺ������������������߳�
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
	requestVoteRpcServer->run();//���������
	cout << "State::registerRequestVote close RequestVote" << endl;
}

void State::stopThread() {
	requestVoteRpcServer.reset(nullptr);
	appendEntriesRpcServer.reset(nullptr);
}

int State::getCurrentTerm() const {
	return currentTerm;
}
bool State::appendEntriesReal(int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries) {
	// ���prevLogIndex�ȵ�ǰentries�б��󣬿���ֱ�ӷ���false��
	if (prevLogIndex >= logEntries.size()) return false;
	// ���prevLogTerm�Բ��ϣ�Ҳ����ֱ�ӷ���false����prevLogIndexС��0����-1�����򲻱ؼ��
	if (prevLogIndex >= 0 && logEntries[prevLogIndex].getTerm() != prevLogTerm) return false;
	// �ڶ�Ӧλ�ü���entries
	lastApplied = prevLogIndex + 1;
	// ��д
	for (; lastApplied < logEntries.size() && lastApplied - prevLogIndex - 1 < entries.size(); ++lastApplied) 
		logEntries[lastApplied] = entries[lastApplied - prevLogIndex - 1];
	// ׷��
	while (lastApplied - prevLogIndex - 1 < entries.size()) {
		logEntries.push_back(entries[lastApplied - prevLogIndex - 1]);
		lastApplied++;
	}
	// ����leaderCommit
	commitIndex = leaderCommit;
	// ����ִ�гɹ�����Ϣ
	return true;
}
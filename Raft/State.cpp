#include "State.h"
#include "Candidate.h"
State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress, 
	NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor, int handleNum):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress), votedFor(votedFor),  handleNum(handleNum), 
	requestVoteAddress(requestVoteAddress), startAddress(startAddress), commitIndex(commitIndex), lastApplied(lastApplied),
	logEntries(logEntries), nextState(NULL), debug(false){
	setDebug();
	appendEntriesThread = requestVoteThread = startThread = NULL;
}
State::~State() {
	//lock_guard<mutex> lockGuard(receiveInfoLock);
	// �˳���Щserver�߳�
	requestVoteRpcServer.reset(nullptr);
	appendEntriesRpcServer.reset(nullptr);
	startRpcServer.reset(nullptr);
	// ���߳�joinһ��
	if (appendEntriesThread) appendEntriesThread->join();
	if (requestVoteThread) requestVoteThread->join();
	if (startThread) startThread->join();
	// �ͷ��̶߳���
	delete startThread;
	// delete timeoutThread;
	delete appendEntriesThread;
	delete requestVoteThread;
}



// ע��ȴ�����AppendEntries
void State::registerAppendEntries() {
	appendEntriesRpcServer.reset(new rpc_server(appendEntriesAddress.second, handleNum));
	appendEntriesRpcServer->register_handler("appendEntries", [this](rpc_conn conn,
		string appendEntriesCodedIntoString) {
			this->appendEntries(std::move(appendEntriesCodedIntoString));
		});
	//appendEntriesRpcServer->register_handler("appendEntries", appendEntries);
	appendEntriesRpcServer->run();//���������
	if (debug) cout << "State::registerAppendEntries close AppendEntries" << endl;
}

// ע��ͶƱ�߳�RequestVote
void State::registerRequestVote() {
	requestVoteRpcServer.reset(new rpc_server(requestVoteAddress.second, handleNum));
	requestVoteRpcServer->register_handler("requestVote", [this](rpc_conn conn, string requestVoteCodedIntoString) {
			this->requestVote(std::move(requestVoteCodedIntoString));
		});
	requestVoteRpcServer->run();//���������
	if (debug) cout << "State::registerRequestVote close RequestVote" << endl;
}

void State::registerServer() {
	// ��������start���߳�
	startThread = new thread(&State::registerStart, this);
	// ����AppendEntries
	appendEntriesThread = new thread(&State::registerAppendEntries, this);
	// ����RequestVote
	requestVoteThread = new thread(&State::registerRequestVote, this);
}

// ע��start����
void State::registerStart() {
	startRpcServer.reset(nullptr);
	startRpcServer.reset(new rpc_server(startAddress.second, handleNum));
	startRpcServer->register_handler("start", [this](rpc_conn conn, string newEntries) {
		this->start(std::move(newEntries));
		});
	startRpcServer->run();//���������
	if (debug) cout << "State::registerStart close start" << endl;
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

void State::start(AppendEntries newEntries) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//��client�������ݼ��뵱ǰ�б���
	for (LogEntry entry : newEntries.getEntries()) logEntries.push_back(entry);
	// �������ӵ�entries������lastApplied
	lastApplied += newEntries.getEntries().size();
}
State* State::run() {
	registerServer();
	work();
	return nextState;
}
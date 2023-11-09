#include "State.h"
#include "Candidate.h"
State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress, 
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
	int votedFor, int handleNum):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress), votedFor(votedFor),  handleNum(handleNum), 
	requestVoteAddress(requestVoteAddress), startAddress(startAddress), commitIndex(commitIndex), lastApplied(lastApplied),
	logEntries(logEntries), nextState(NULL), debug(false), applyMessageAddress(applyMessageAddress) {
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

bool State::crush(double rate) const {
	return TimeoutCounter().getRandom(0, 100) < 100 * rate;
}

void State::persistence() const {
	PersistenceInfoReaderAndWriter persistenceInfoReaderAndWriter(ID);
	persistenceInfoReaderAndWriter.setCurrentTerm(currentTerm);
	persistenceInfoReaderAndWriter.setEntries(logEntries);
	persistenceInfoReaderAndWriter.setVotedFor(votedFor);
	// д���̶����첽ִ�У�����Ӱ�����߳�
	async(&PersistenceInfoReaderAndWriter::write, &persistenceInfoReaderAndWriter);
	//persistenceInfoReaderAndWriter.write();
}

void State::applyMsg(bool snapshot, int snapshotIndex) {
	//if (debug) cout << "Leader::applyMsg content logEntries.size() " << logEntries.size() << " commitIndex " << commitIndex << endl;
	/*
	* snapshot��Ϊд���ձ�־����Ϊд���գ���indexΪ�����±�
	* ��Ϊ��ͨapplyMsg����ΪcommitedIndex
	*/
	int applyIndex = commitIndex;
	if (debug) cout << "State::applyMsg applyIndex: " << applyIndex << " snapshot " << snapshot << endl;
	if (snapshot) applyIndex = snapshotIndex;
	if (applyIndex < 0 || applyIndex >= logEntries.size()) throw exception("State::applyMsg logical error: index is negative or greater than the log");
	vector<Command> commands;
	// ����index��������������
	for (int i = 0; i <= applyIndex; ++i) commands.push_back(logEntries[i].getCommand());
	// �첽֪ͨ�ϲ�Ӧ��д����/ִ������ǿ��Ž�����
	async(&RPC::invokeRemoteApplyMsg, &rpc, applyMessageAddress, ApplyMsg(commands, applyIndex, snapshot));
	//rpc.invokeRemoteApplyMsg(applyMessageAddress, ApplyMsg(commands, applyIndex, snapshot));
}
void State::snapShotModifyState(int snapshotIndex) {
	// ֪ͨ�ϲ�Ӧ�ÿ���д����
	applyMsg(true, snapshotIndex);
	// ɾ������
	logEntries.erase(logEntries.begin(), logEntries.begin() + snapshotIndex);
	// �޸�commit״̬
	commitIndex -= (snapshotIndex + 1);
}


void State::printState() {
	cout << endl << endl;
	cout << "There are " << logEntries.size() << " log entries in this state." << endl;
	for (LogEntry entry : logEntries) {
		cout << "term: " << entry.getTerm() << " command: " << entry.getCommand().getType() << endl;
	}
	cout << endl << endl;
}


// ע��ȴ�����AppendEntries
void State::registerAppendEntries() {
	appendEntriesRpcServer.reset(new rpc_server(appendEntriesAddress.second, handleNum));
	registerHandleAppendEntries();
	appendEntriesRpcServer->run();//���������
	if (debug) cout << "State::registerAppendEntries close AppendEntries" << endl;
}

// ע��ͶƱ�߳�RequestVote
void State::registerRequestVote() {
	requestVoteRpcServer.reset(new rpc_server(requestVoteAddress.second, handleNum));
	registerHandleRequestVote();
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
	registerHandleStart();
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

StartAnswer State::start(rpc_conn conn, Command command) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive the command from client, content: " << command.getKey() << endl;
	//��client�������ݼ��뵱ǰ�б���
	logEntries.push_back(LogEntry(currentTerm, command));
	// �������ӵ�entries������lastApplied
	lastApplied = logEntries.size() - 1;
	return StartAnswer{ currentTerm, lastApplied };
}
State* State::run() {
	registerServer();
	work();
	
	return nextState;
}
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
	// 退出这些server线程
	requestVoteRpcServer.reset(nullptr);
	appendEntriesRpcServer.reset(nullptr);
	startRpcServer.reset(nullptr);
	// 将线程join一下
	if (appendEntriesThread) appendEntriesThread->join();
	if (requestVoteThread) requestVoteThread->join();
	if (startThread) startThread->join();
	
	// 释放线程对象
	delete startThread;
	// delete timeoutThread;
	delete appendEntriesThread;
	delete requestVoteThread;
	PersistenceInfoReaderAndWriter persistenceInfoReaderAndWriter(ID);
	persistenceInfoReaderAndWriter.setCurrentTerm(currentTerm);
	persistenceInfoReaderAndWriter.setEntries(logEntries);
	persistenceInfoReaderAndWriter.setVotedFor(votedFor);
	persistenceInfoReaderAndWriter.write();

}

bool State::crush(double rate) const {
	return TimeoutCounter().getRandom(0, 100) < 100 * rate;
}
void State::printState() {
	cout << endl << endl;
	cout << "There are " << logEntries.size() << " log entries in this state." << endl;
	for (LogEntry entry : logEntries) {
		cout << "term: " << entry.getTerm() << " command: " << entry.getCommand() << endl;
	}
	cout << endl << endl;
}


// 注册等待接收AppendEntries
void State::registerAppendEntries() {
	appendEntriesRpcServer.reset(new rpc_server(appendEntriesAddress.second, handleNum));
	registerHandleAppendEntries();
	appendEntriesRpcServer->run();//启动服务端
	if (debug) cout << "State::registerAppendEntries close AppendEntries" << endl;
}

// 注册投票线程RequestVote
void State::registerRequestVote() {
	requestVoteRpcServer.reset(new rpc_server(requestVoteAddress.second, handleNum));
	registerHandleRequestVote();
	requestVoteRpcServer->run();//启动服务端
	if (debug) cout << "State::registerRequestVote close RequestVote" << endl;
}

void State::registerServer() {
	// 开启接收start的线程
	startThread = new thread(&State::registerStart, this);
	// 开启AppendEntries
	appendEntriesThread = new thread(&State::registerAppendEntries, this);
	// 开启RequestVote
	requestVoteThread = new thread(&State::registerRequestVote, this);
}

// 注册start函数
void State::registerStart() {
	startRpcServer.reset(nullptr);
	startRpcServer.reset(new rpc_server(startAddress.second, handleNum));
	registerHandleStart();
	startRpcServer->run();//启动服务端
	if (debug) cout << "State::registerStart close start" << endl;
}
int State::getCurrentTerm() const {
	return currentTerm;
}
bool State::appendEntriesReal(int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries) {
	// 如果prevLogIndex比当前entries列表都大，可以直接返回false了
	if (prevLogIndex >= logEntries.size()) return false;
	// 如果prevLogTerm对不上，也可以直接返回false，若prevLogIndex小于0（即-1），则不必检测
	if (prevLogIndex >= 0 && logEntries[prevLogIndex].getTerm() != prevLogTerm) return false;
	// 在对应位置加入entries
	lastApplied = prevLogIndex + 1;
	// 覆写
	for (; lastApplied < logEntries.size() && lastApplied - prevLogIndex - 1 < entries.size(); ++lastApplied) 
		logEntries[lastApplied] = entries[lastApplied - prevLogIndex - 1];
	// 追加
	while (lastApplied - prevLogIndex - 1 < entries.size()) {
		logEntries.push_back(entries[lastApplied - prevLogIndex - 1]);
		lastApplied++;
	}
	// 更新leaderCommit
	commitIndex = leaderCommit;
	// 返回执行成功的信息
	return true;
}

StartAnswer State::start(rpc_conn conn, string command) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive the command from client, content: " << command << endl;
	//将client给的数据加入当前列表中
	logEntries.push_back(LogEntry(currentTerm, command));
	// 有新增加的entries，更新lastApplied
	lastApplied = logEntries.size() - 1;
	return StartAnswer{ currentTerm, lastApplied };
}
State* State::run() {
	registerServer();
	work();
	
	return nextState;
}
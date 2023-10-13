#include "State.h"

State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	ServerState state, int commitIndex, int lastApplied, vector<LogEntry> logEntries):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress), state(state),
	requestVoteAddress(requestVoteAddress), commitIndex(commitIndex), lastApplied(lastApplied), 
	logEntries(logEntries){
	// ͶƱ�����Ϊ-1����˭��ûͶ
	votedFor = -1;
	// ���뼯Ⱥ������server�ĵ�ַ
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
}

void State::timeoutCounterThread() {
	timeoutCounter.run();
	// �������߳���ִ�е�ָ���ÿ�
}

// ע��ȴ�����AppendEntries
void State::registerAppendEntries() {

}

// ע��ͶƱ�߳�RequestVote
void State::registerRequestVote() {

}
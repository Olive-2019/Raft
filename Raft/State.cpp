#include "State.h"

State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	ServerState state, int commitIndex, int lastApplied): 
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress), state(state),
	requestVoteAddress(requestVoteAddress), commitIndex(commitIndex), lastApplied(lastApplied) {
	votedFor = -1;
}

void State::timeoutCounterThread() {
	timeoutCounter.run();
	// �������߳���ִ�е�ָ���ÿ�
}
// �ȴ�����AppendEntries
string State::appendEntries(string appendEntriesCodedIntoString) {

}
// ע��ȴ�����AppendEntries
void State::registerAppendEntries() {

}
// ͶƱ�߳�RequestVote
string State::requestVote(string requestVoteCodedIntoString) {

}
// ע��ͶƱ�߳�RequestVote
void State::registerRequestVote() {

}
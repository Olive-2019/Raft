#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries), 
	getVote(0){
	for (auto follower = serverAddress.begin(); follower != serverAddress.end(); ++follower) {
		int followerID = follower->first;
		voteResult[followerID] = 0;
	}
}
// ����RequestVote������Ҫ���ü�ʱ����leader�м�ʱ��ֻ����һ��
string Candidate::requestVote(string requestVoteCodedIntoString) {
	receiveInfoLock.lock();
	RequestVote requestVote(requestVoteCodedIntoString);
	// termû�бȵ�ǰCandidate�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (requestVote.getTerm() <= currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = requestVote.getTerm();

	// ��Ӧ�ڷ��ؽ���Ժ�����������̣߳����Ǵ˴��޷���ô����
	// ������nextState��Ϊ�ź�������֤�̼߳�ͬ���ͷ�
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// ������һ״̬��
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}

// ����AppendEntries������Ҫ���ü�ʱ����leader�м�ʱ��ֻ����һ��
// ֻҪ�Է���term�����Լ�С�ͽ��ܶԷ�Ϊleader
string Candidate::appendEntries(string appendEntriesCodedIntoString) {
	receiveInfoLock.lock();
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// termû�бȵ�ǰCandidate�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (appendEntries.getTerm() < currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = appendEntries.getTerm();
	// ��entries��ӵ���ǰ�б��У����ú���������Ҫ�ж����ܷ���ӣ���һ����ʵ�Ѿ�����follower�Ĺ����ˣ�
	bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
		appendEntries.getLeaderCommit(), appendEntries.getEntries());
	// ��Ӧ�ڷ��ؽ���Ժ�����������̣߳����Ǵ˴��޷���ô����
	// ������nextState��Ϊ�ź�������֤�̼߳�ͬ���ͷ�
	if (!nextState || nextState->getCurrentTerm() <= currentTerm) {
		delete nextState;
		// ������һ״̬��
		nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
			commitIndex, lastApplied, logEntries);
	}
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}
// ���иû���������ֵ����һ��״̬
State* Candidate::run() {
	work();
	return nextState;
}

void Candidate::work() {
	while (!nextState) {
		sendRequestVote();
		// ѡ�ٳɹ�
		if (checkVoteResult()) {
			nextState = new Leader(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries);
			timeoutCounter.stopCounter();
			return;
		}
	}
}
// ����ͶƱ��Ϣ
void Candidate::checkRequestVote() {
	// װ��requestVote����
	int lastIndex = -1, lastTerm = -1;
	if (logEntries.size()) lastTerm = logEntries.back().getTerm();
	RequestVote requestVote(currentTerm, ID, lastIndex, lastTerm);
	/*
	* ��Ŀǰ���Ѿ��з���ֵ�ˣ���ûӮ��ѡ�٣����˳���ǰ״̬��������һ��candidate״̬
	* �������л�ûͶƱ��follower�������Լ�
	* 1. û�з���ֵ���ط�
	* 2. �з���ֵ������voteRes��getVoteCounter
	*	
	*/ 

	for (auto follower = voteResult.begin(); follower != voteResult.end(); ++follower) {
		if (follower->second) continue;
		int followerID = follower->first;
		
	}
}

// ���ͶƱ���
bool Candidate::checkVoteResult() {
	if (getVoteCounter > voteResult.size() / 2) return true;
	return false;
}
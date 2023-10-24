#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, commitIndex, lastApplied, logEntries), getVoteCounter(1)
	 {
	// ���Լ�ͶƱ
	votedFor = ID;
	// ��ʼ��voteResult������������ͶƱ��Ϣ
	for (auto follower = serverAddress.begin(); follower != serverAddress.end(); ++follower) {
		int followerID = follower->first;
		if (followerID == ID) continue;
		voteResult[followerID] = 0;
		sendRequestVote(followerID);
	}
	// ������ʱ��
	timeoutThread = new thread(&Candidate::timeoutCounterThread, this);
}
Candidate::~Candidate() {
	// join�߳�
	timeoutThread->join();
	// �ͷŶ���
	delete timeoutThread;
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
		startAddress, commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}

void Candidate::timeoutCounterThread() {
	// ��ʱ���أ�ת����candidate
	if (timeoutCounter.run())
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, commitIndex, lastApplied, logEntries);
	// δ��ʱ���������أ���nextState�ĳ�ʼ������stop�ĵ��ô�
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
			startAddress, commitIndex, lastApplied, logEntries);
	}
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}


// ����ͶƱ��Ϣ
bool Candidate::checkRequestVote() {
	/*
	* ��Ŀǰ���Ѿ��з���ֵ�ˣ���ûӮ��ѡ�٣����˳���ǰ״̬��������һ��candidate״̬
	* �������л�ûͶƱ��follower�������Լ�
	* 1. û�з���ֵ���ط�
	* 2. �з���ֵ������voteResult��getVoteCounter
	*/ 
	bool left = false;
	for (auto follower = voteResult.begin(); follower != voteResult.end(); ++follower) {
		// ����Ѿ����ع�ֵ�ˣ�����Ҫ�ٶ�����в���
		if (follower->second) continue;
		left = true;
		int followerID = follower->first;
		// û�з���ֵ���ط�
		if (!followerReturnVal[followerID]._Is_ready()) sendRequestVote(followerID);
		// �з���ֵ������voteResult��getVoteCounter
		else {
			Answer answer(followerReturnVal[followerID].get());
			if (answer.getSuccess()) follower->second = 1, getVoteCounter++;
			else follower->second = -1;
		}
	}
	return left;
}
void Candidate::sendRequestVote(int followerID) {
	if (voteResult.find(followerID) == voteResult.end()) throw exception("Candidate::sendRequestVote follower doesn't exist.");
	RequestVote requestVoteContent(currentTerm, ID, logEntries.size() - 1, logEntries.size() ? logEntries.back().getTerm() : -1);
	followerReturnVal[followerID] =
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], "requestVote", requestVoteContent.code());
}
// ���ͶƱ���
bool Candidate::checkVoteResult() {
	if (getVoteCounter > voteResult.size() / 2) return true;
	return false;
}

void Candidate::work() {
	while (!nextState) {
		sleep_for(seconds(300));
		if (checkRequestVote()) {
			// û�о���ʤ�����ؿ�
			nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
				startAddress, commitIndex, lastApplied, logEntries);
			return;
		}
		// ѡ�ٳɹ�
		if (checkVoteResult()) {
			nextState = new Leader(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
				startAddress, commitIndex, lastApplied, logEntries);
			return;
		}
	}
}

// ���иû���������ֵ����һ��״̬
State* Candidate::run() {
	State::run();
	// ֹͣ��ǰ������
	timeoutCounter.stopCounter();
	return nextState;
}
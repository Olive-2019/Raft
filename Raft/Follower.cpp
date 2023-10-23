#include "Follower.h"
Follower::Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries) { }
bool Follower::isNewerThanMe(int lastLogIndex, int lastLogTerm) const {
	if (!logEntries.size()) return true;
	if (logEntries.back().getTerm() == currentTerm) return logEntries.size() < lastLogIndex + 1;
	return currentTerm < logEntries.back().getTerm();
}
// ����RequestVote
string Follower::requestVote(string requestVoteCodedIntoString) {
	RequestVote requestVote(requestVoteCodedIntoString);
	//ֱ�ӷ���false��term < currentTerm
	if (requestVote.getTerm() < currentTerm) return Answer(currentTerm, false).code();
	//��� ��votedFor == null || votedFor == candidateId�� && candidate��log�ȵ�ǰ�ڵ��£�ͶƱ���ýڵ㣬����ܾ��ýڵ�
	if ((votedFor < 0 || votedFor == requestVote.getCandidateId())
		&& isNewerThanMe(requestVote.getLastLogIndex(), requestVote.getLastLogTerm())) {
		votedFor = requestVote.getCandidateId();
		return Answer(currentTerm, true).code();
	}
	return Answer(currentTerm, false).code();
}
// ����AppendEntries
string Follower::appendEntries(string appendEntriesCodedIntoString) {
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// ��ʱ��ʱ������
	timeoutCounter.setReceiveInfoFlag();
	//ֱ�ӷ���false��term < currentTerm or prevLogIndex/Term��Ӧ��log������
	if ((appendEntries.getTerm() < currentTerm) 
		|| appendEntries.getPrevLogIndex() >= logEntries.size()
		|| logEntries[appendEntries.getPrevLogIndex()].getTerm() != appendEntries.getTerm())
		return Answer(currentTerm, false).code();
	int index = appendEntries.getPrevLogIndex() + 1;
	for (LogEntry entry : appendEntries.getEntries()) {
		//���ڳ�ͻentry������ͬ��index����ͬ��term������leaderû�е����ݣ���ɾ�����е�entries��д��leader��������
		if (index < logEntries.size()) logEntries[index] = entry;
		//�������ڵ�entries׷�ӵ�ϵͳ��
		else logEntries.push_back(entry);
		index++;
	}
	//��leaderCommit > commitIndexʱ������commitIndex = min(leaderCommit, Ŀǰ����entry��index)
	if (appendEntries.getLeaderCommit() > commitIndex) {
		commitIndex = appendEntries.getLeaderCommit();
		if (commitIndex > logEntries.size() - 1) commitIndex = logEntries.size() - 1;
	}
	return Answer(currentTerm, true).code();
}
// ���㳬ʱ���߳�
void Follower::timeoutCounterThread() {
	State::timeoutCounterThread();
	nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress,
		requestVoteAddress, commitIndex, lastApplied, logEntries);
}
// ��������ת������һ��״̬
State* Follower::run() {
	State::run();
	//waitThread();
	return nextState;
}
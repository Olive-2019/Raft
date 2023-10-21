#include "Candidate.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries), 
	getVote(0){
	voteResult = vector<int>(serverAddress.size(), 0);
}
// ����RequestVote
string Candidate::requestVote(rpc_conn conn, string requestVoteCodedIntoString) {
	RequestVote requestVote(requestVoteCodedIntoString);
	// termû�бȵ�ǰCandidate�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (requestVote.getTerm() <= currentTerm) return Answer(currentTerm, false).code();
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	// ֹͣ��ǰ�ڵ����У���ת��follower��������Ҫ�࿪һ���̣߳��Ҹ��߳���Ҫ�ȴ�һ��ʱ��&&detach
	return Answer(currentTerm, true).code();
}
// ����AppendEntries
string Candidate::appendEntries(rpc_conn conn, string appendEntriesCodedIntoString) {
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// termû�бȵ�ǰCandidate�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (appendEntries.getTerm() <= currentTerm) return Answer(currentTerm, false).code();
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	// ֹͣ��ǰ�ڵ����У���ת��follower��������Ҫ�࿪һ���̣߳��Ҹ��߳���Ҫ�ȴ�һ��ʱ��&&detach
	return Answer(currentTerm, true).code();
}
// ���иû���������ֵ����һ��״̬
State* Candidate::run() {

}
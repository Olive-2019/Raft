#include "Leader.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries) {
	
	// ��������
}
// ����RequestVote
string Leader::requestVote(rpc_conn conn, string requestVoteCodedIntoString) {
	RequestVote requestVote(requestVoteCodedIntoString);
	// termû�бȵ�ǰleader�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (requestVote.getTerm() <= currentTerm) return to_string(currentTerm) + " 0";
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	// ֹͣ��ǰ�ڵ����У���ת��follower��������Ҫ�࿪һ���̣߳��Ҹ��߳���Ҫ�ȴ�һ��ʱ��&&detach
	return to_string(currentTerm) + " 1";
}
// ����AppendEntries
string Leader::appendEntries(rpc_conn conn, string appendEntriesCodedIntoString) {
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// termû�бȵ�ǰleader�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (appendEntries.getTerm() <= currentTerm) return to_string(currentTerm) + " 0";
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	// ֹͣ��ǰ�ڵ����У���ת��follower��������Ҫ�࿪һ���̣߳��Ҹ��߳���Ҫ�ȴ�һ��ʱ��&&detach
	return to_string(currentTerm) + " 1";
}
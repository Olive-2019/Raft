#include "Raft.h"
// ��client���ã���״̬�������������ֵ��index��term���Կո���
string Raft::start(string command) {
	return "";
}
// ��ɺ���client���Ϳ���ִ�и����������
void Raft::applyMsg(string command, int index) {

}
// ����״̬��������״̬���ӿ��л�״̬
void Raft::run(int serverID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress) {
	int currentTerm = -1, commitedIndex = -1, lastApplied = -1;
	vector<LogEntry> entries;
	state = new Follower(currentTerm, serverID, appendEntriesAddress, requestVoteAddress, 
		startAddress, commitedIndex, lastApplied, entries);
	while (state) {
		State* nextState = state->run();
		delete state;
		state = nextState;
	}
}
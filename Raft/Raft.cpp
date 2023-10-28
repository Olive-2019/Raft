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
	// ��ȡ�־û���Ϣ�����޳־û���Ϣ����д��־û���Ϣ
	PersistenceInfoReaderAndWriter persistenceInfoReaderAndWriter(serverID);
	persistenceInfoReaderAndWriter.read();
	int currentTerm = persistenceInfoReaderAndWriter.getCurrentTerm(), commitedIndex = -1;
	int votedFor = persistenceInfoReaderAndWriter.getVotedFor();
	vector<LogEntry> entries = persistenceInfoReaderAndWriter.getEntries();
	int lastApplied = entries.size() - 1;
	// ��Follower��ʼѭ��
	//currentTerm = 1;
	state = new Follower(currentTerm, serverID, appendEntriesAddress, requestVoteAddress, 
		startAddress, commitedIndex, lastApplied, entries, votedFor);
	while (state) {
		State* nextState = state->run();
		delete state;
		state = nextState;
	}
}
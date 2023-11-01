#include "Raft.h"

// ����״̬��������״̬���ӿ��л�״̬
void Raft::run(int serverID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress) {
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
		startAddress, applyMessageAddress, commitedIndex, lastApplied, entries, votedFor);
	while (state) {
		State* nextState = state->run();
		delete state;
		if (!nextState) throw exception("Raft::run the next state is NULL.");
		state = nextState;
	}
}
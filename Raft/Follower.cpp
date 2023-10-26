#include "Follower.h"
Follower::Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress,
		commitIndex, lastApplied, logEntries, votedFor), leaderID(-1) {
	if (debug) cout << endl << ID << " become Follower" << endl;
	// ���뼯Ⱥ������server�ĵ�ַ��follower����StartAddress�ĵ�ַ
	ServerAddressReader serverAddressReader("StartAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
	// ������ʱ��
	timeoutThread = new thread(&Follower::timeoutCounterThread, this);
}
Follower::~Follower() {
	timeoutThread->join();
	delete timeoutThread;
	if (debug) cout << ID << " will not be Follower any more." << endl;
}
void Follower::timeoutCounterThread() {
	// ��ʱ���أ�ת����candidate
	if (timeoutCounter.run()) {
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, commitIndex, lastApplied, logEntries);
		if (debug) cout << "Follower " << ID << " timeout and quit." << endl;
	}
	else if (debug) cout << "Follower " << ID << " quit." << endl;
	
	// δ��ʱ���������أ���nextState�ĳ�ʼ������stop�ĵ��ô�
}
bool Follower::isNewerThanMe(int lastLogIndex, int lastLogTerm) const {
	if (!logEntries.size()) return true;
	if (logEntries.back().getTerm() == currentTerm) return logEntries.size() < lastLogIndex + 1;
	return currentTerm < logEntries.back().getTerm();
}
void Follower::start(AppendEntries newEntries) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	// ������leader��ת����leader��û�о͸��Լ���
	if (serverAddress.find(leaderID) != serverAddress.end())
		rpc.invokeRemoteFunc(serverAddress[leaderID], "start", newEntries.code());
	else State::start(newEntries);
}
// ����RequestVote
string Follower::requestVote(string requestVoteCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive requestVote Msg" << endl;
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
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive appendEntries Msg" << endl;
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// ��ʱ��ʱ������
	timeoutCounter.setReceiveInfoFlag();
	//ֱ�ӷ���false��term < currentTerm or prevLogIndex/Term��Ӧ��log������
	if ((appendEntries.getTerm() < currentTerm)
		|| appendEntries.getPrevLogIndex() >= logEntries.size()
		|| logEntries[appendEntries.getPrevLogIndex()].getTerm() != appendEntries.getTerm())
		return Answer(currentTerm, false).code();
	
		
	int index = appendEntries.getPrevLogIndex() + 1;
	// ����leaderID
	leaderID = appendEntries.getLeaderId();
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
void Follower::work() {
	// ��nextState��Ϊͬ���ź���,��ʱ/�յ����µ���Ϣ��ʱ��Ϳ����˳���
	while (!nextState) {
		// ˯��һ��ʱ��
		sleep_for(seconds(2));
	}
}
// ��������ת������һ��״̬
State* Follower::run() {
	State::run();
	timeoutCounter.stopCounter();
	return nextState;
}
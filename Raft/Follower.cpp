#include "Follower.h"
Follower::Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, applyMessageAddress,
		commitIndex, lastApplied, logEntries, votedFor), leaderID(-1), timeoutThread(NULL) {
	if (debug) cout << "Follower::Follower new a Follower" << endl;

}
Follower::~Follower() {
	timeoutCounter.stopCounter();
	if (timeoutThread) timeoutThread->join();
	delete timeoutThread;
	if (debug) cout << ID << " will not be Follower any more." << endl;
}
void Follower::timeoutCounterThread() {
	// ��ʱ���أ�ת����candidate
	if (timeoutCounter.run()) {
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
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
StartAnswer Follower::start(rpc_conn conn, Command command) {
	// ����������������������̶߳�Ҫ����ת��start�ķ���ֵ
	//lock_guard<mutex> lockGuard(receiveInfoLock);
	// ������leader��ת����leader��û�о͸��Լ���
	if (debug) cout << "Follower::start try to redirect to " << leaderID << endl;
	if (serverAddress.find(leaderID) != serverAddress.end())
		return rpc.invokeRemoteStart(serverAddress[leaderID], command);
	
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//��client�������ݼ��뵱ǰ�б���
	logEntries.push_back(LogEntry(currentTerm, command));
	// �������ӵ�entries������lastApplied
	lastApplied = logEntries.size() - 1;
	return StartAnswer{ currentTerm, lastApplied };
	
}
// ����RequestVote
Answer Follower::requestVote(rpc_conn conn, RequestVote requestVote) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//RequestVote requestVote(requestVoteCodedIntoString);
	//if (debug) cout << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << " content is " << requestVote.code() << endl;
	//ֱ�ӷ���false��term < currentTerm
	if (nextState) return nextState->requestVote(conn, requestVote);
	if (debug) cout << "Follower::requestVote " << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << endl;
	if (requestVote.getTerm() < currentTerm) return Answer(currentTerm, false);
	currentTerm = requestVote.getTerm();
	//��� ��votedFor == null || votedFor == candidateId�� && candidate��log�ȵ�ǰ�ڵ��£�ͶƱ���ýڵ㣬����ܾ��ýڵ�
	bool vote = false;
	if ((votedFor < 0 && isNewerThanMe(requestVote.getLastLogIndex(), requestVote.getLastLogTerm()))
		|| votedFor == requestVote.getCandidateId()) {
		votedFor = requestVote.getCandidateId();
		vote = true;
	}
	//if (debug) cout << "Follower::requestVote: send to " << requestVote.getCandidateId()
		//<< ", content is " << currentTerm << ' ' << vote << endl;
	return Answer(currentTerm, vote);
}
// ����AppendEntries
Answer Follower::appendEntries(rpc_conn conn, AppendEntries appendEntries) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (nextState) return nextState->appendEntries(conn, appendEntries);
	//AppendEntries appendEntries(appendEntriesCodedIntoString);
	//if (debug) cout << "Follower::appendEntries : content " << appendEntriesCodedIntoString << endl;
	if (debug) cout << "Follower::appendEntries from " << appendEntries.getLeaderId()
		<<  " : log entries size: " << appendEntries.getEntries().size()
		<< " prevIndex " << appendEntries.getPrevLogIndex() << " prevTerm " << appendEntries.getPrevLogTerm() << endl;
	if (debug && appendEntries.getPrevLogIndex() >= 0 && appendEntries.getPrevLogIndex() < logEntries.size())
		cout << "real pervTerm " << logEntries[appendEntries.getPrevLogIndex()].getTerm() << endl;
	// ��ʱ��ʱ������
	timeoutCounter.setReceiveInfoFlag();
	//ֱ�ӷ���false��term < currentTerm 
	if (appendEntries.getTerm() < currentTerm) return Answer(currentTerm, false);
	// ��������true
	leaderID = appendEntries.getLeaderId();
	if (!appendEntries.getEntries().size()) return Answer(currentTerm, true);
	//ֱ�ӷ���false��prevLogIndex/Term��Ӧ��log������
	if (appendEntries.getPrevLogIndex() >= 0 && (appendEntries.getPrevLogIndex() >= logEntries.size()
		|| logEntries[appendEntries.getPrevLogIndex()].getTerm() != appendEntries.getTerm()))
		return Answer(currentTerm, false);
	currentTerm = appendEntries.getTerm();
	//if (debug) cout << "Follower::appendEntries here" << endl;
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
	commitIndex = appendEntries.getLeaderCommit();
	//if (debug) cout << "Follower::appendEntries newCommitIndex " << commitIndex << endl;
	// ����д���ձ�־�����޸ĵ�ǰϵͳ״̬��֪ͨ�ϲ�Ӧ��д���գ����ޣ���֪ͨ�ϲ�Ӧ�õ�ǰ�ύ������
	if (appendEntries.isSnapshot()) snapShotModifyState(commitIndex);
	else if (commitIndex >= 0) applyMsg();
	if (debug) cout << "Follower::appendEntries real " << appendEntries.getEntries()[0].getCommand().getKey() << endl;
	return Answer( currentTerm, true );
}
void Follower::work() {


	if (debug) cout << endl << ID << " work as Follower" << endl;
	// ���뼯Ⱥ������server�ĵ�ַ��follower����StartAddress�ĵ�ַ
	if (nextState) return;
	ServerAddressReader serverAddressReader("StartAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
	// ������ʱ��
	timeoutThread = new thread(&Follower::timeoutCounterThread, this);
	// ��nextState��Ϊͬ���ź���,��ʱ/�յ����µ���Ϣ��ʱ��Ϳ����˳���
	while (!nextState) {
		// ˯��һ��ʱ��
		if (debug) printState();
		sleep_for(seconds(3));
		persistence();
		// ģ��ͣ��
		/*if (crush(0.1)) {
			if (debug) cout << "crush" << endl;
			break;
		}*/
	}
}



// ע��ȴ�����AppendEntries���
void Follower::registerHandleAppendEntries() {
	appendEntriesRpcServer->register_handler("appendEntries", &Follower::appendEntries, this);
}
// ע��ͶƱ�߳�RequestVote���
void Follower::registerHandleRequestVote() {
	requestVoteRpcServer->register_handler("requestVote", &Follower::requestVote, this);
}
// ע��start�������
void Follower::registerHandleStart() {
	startRpcServer->register_handler("start", &Follower::start, this);
}
#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor, int maxResendNum) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries, votedFor),
	getVoteCounter(1), maxResendNum(maxResendNum), rejectCounter(0), timeoutThread(NULL){
	if (debug) cout << "Candidate::Candidate new a Candidate" << endl;
}
Candidate::~Candidate() {
	timeoutCounter.stopCounter();
	// join�߳�
	if (timeoutThread) timeoutThread->join();
	// �ͷŶ���
	delete timeoutThread;
	if (debug) cout << ID << " will not be Candidate any more." << endl;
}
// ����RequestVote������Ҫ���ü�ʱ����leader�м�ʱ��ֻ����һ��
Answer Candidate::requestVote(rpc_conn conn, RequestVote requestVote) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//RequestVote requestVote(requestVoteCodedIntoString);
	if (nextState) return nextState->requestVote(conn, requestVote);
	if (debug) cout << "Candidate::requestVote " << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << endl;
	// termû�бȵ�ǰCandidate�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (requestVote.getTerm() <= currentTerm) {
		if (debug) cout << "reject " << requestVote.getCandidateId() << ", cause its term is old." << endl;
		return Answer(currentTerm, false);
	}
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = requestVote.getTerm();

	// ��Ӧ�ڷ��ؽ���Ժ�����������̣߳����Ǵ˴��޷���ô����
	// ������nextState��Ϊ�ź�������֤�̼߳�ͬ���ͷ�
	// ������һ״̬��
	if (debug) cout << "Candidate::requestVote new Follower" << endl;
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);

	return Answer( currentTerm, true );
}

void Candidate::timeoutCounterThread() {
	// ��ʱ���أ�ת����candidate
	if (timeoutCounter.run()) {
		if (debug) cout << "Candidate::timeoutCounterThread new Candidate" << endl;
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
		//if (debug) cout << "Candidate " << ID << " timeout and quit." << endl;
	}
	//else if (debug) cout << "Candidate " << ID << " quit." << endl;
		
	// δ��ʱ���������أ���nextState�ĳ�ʼ������stop�ĵ��ô�
}

// ����AppendEntries������Ҫ���ü�ʱ����leader�м�ʱ��ֻ����һ��
// ֻҪ�Է���term�����Լ�С�ͽ��ܶԷ�Ϊleader
Answer Candidate::appendEntries(rpc_conn conn, AppendEntries appendEntries) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (nextState) return nextState->appendEntries(conn, appendEntries);
	if (debug) cout << "Candidate::appendEntries " << ID << " receive appendEntries Msg from " << appendEntries.getLeaderId() << endl;
	//AppendEntries appendEntries(appendEntriesCodedIntoString);
	// termû�бȵ�ǰCandidate�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (appendEntries.getTerm() < currentTerm) return Answer( currentTerm, false );
	// term���£����˳���ǰ״̬�����ص�Follower��״̬������ִ�е���ʵfollower���߼�
	currentTerm = appendEntries.getTerm();
	// ��entries��ӵ���ǰ�б��У����ú���������Ҫ�ж����ܷ���ӣ���һ����ʵ�Ѿ�����follower�Ĺ����ˣ�
	//bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
	//	appendEntries.getLeaderCommit(), appendEntries.getEntries());
	//if (appendEntries.isSnapshot()) snapShotModifyState(commitIndex);
	//else applyMsg();
	//// ��Ӧ�ڷ��ؽ���Ժ�����������̣߳����Ǵ˴��޷���ô����
	//// ������nextState��Ϊ�ź�������֤�̼߳�ͬ���ͷ�
	//// ������һ״̬��
	if (debug) cout << "Candidate::appendEntries new Follower" << endl;
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
	nextState->appendEntries(conn, appendEntries);
	return Answer( currentTerm, true );
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
		Answer answer( 0, false );
		// û�з���ֵ���ط�
		if (!checkOneFollowerReturnValue(followerID, answer)) continue;
		// �з���ֵ������voteResult��getVoteCounter
		//if (debug) cout << "receive the return value of " << followerID << ", and its result is " << answer.success << endl;
		// �յ�ͶƱ��voteResult��Ϊ1
		if (answer.isSuccess()) follower->second = 1, getVoteCounter++;
		// û���յ��Ϸ�ͶƱ��voteResult��Ϊ0
		else {
			follower->second = -1;
			rejectCounter++;
			if (answer.getTerm() > currentTerm || rejectCounter > serverAddress.size() / 2) {
				currentTerm = answer.getTerm();
				if (debug) cout << "Candidate::checkRequestVote new Follower" << endl;
				nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, applyMessageAddress,
					commitIndex, lastApplied, logEntries);
				return true;
			}
		}
		
	}
	return left;
}
bool Candidate::sendRequestVote(int followerID) {
	if (voteResult.find(followerID) == voteResult.end()) throw exception("Candidate::sendRequestVote follower doesn't exist.");
	// ����Ѿ���������ط����������ط���ֱ�ӷ���
	if (followerReturnVal[followerID].size() >= maxResendNum) return false;
	RequestVote requestVoteContent(currentTerm, ID, logEntries.size() - 1, logEntries.size() ? logEntries.back().getTerm() : -1);
	// �첽���� ��������
	followerReturnVal[followerID].push_back(
		async(&RPC::invokeRequestVote, &rpc, serverAddress[followerID], requestVoteContent)
	);
	//if (debug) cout << "send requestVote to " << followerID << " content is " << requestVoteContent.code() << endl;
}

// ��鵥��follower�����ɹ���true�������ɹ������ط�
bool Candidate::checkOneFollowerReturnValue(int followerID, Answer& ans) {

	for (auto val = followerReturnVal[followerID].begin(); val != followerReturnVal[followerID].end(); ) {
		future_status status = val->wait_for(seconds(0));
		if (status == future_status::ready) {
			timeoutCounter.setReceiveInfoFlag();
			ans = val->get();
			followerReturnVal[followerID].clear();
			return true;
		}
		else if (status == future_status::timeout) val = followerReturnVal[followerID].erase(val);
		else val++;
	}
	// ��û�з��أ������ط�
	sendRequestVote(followerID);
	return false;
}



// ���ͶƱ���
bool Candidate::checkVoteResult() {
	if (getVoteCounter > voteResult.size() / 2) return true;
	return false;
}

void Candidate::work() {
	if (debug) cout << endl << ID << " work as Candidate" << endl;
	// ���뼯Ⱥ������server�ĵ�ַ��candidate����RequestVoteAddress�ĵ�ַ
	if (nextState) return;
	ServerAddressReader serverAddressReader("RequestVoteAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
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

	while (!nextState) {
		sleep_for(seconds(2));
		if (!checkRequestVote()) {
			// û�о���ʤ�����ؿ�
			if (debug) cout << "Candidate::work new Candidate" << endl;
			nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
				startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
			return;
		}
		// ѡ�ٳɹ�
		if (checkVoteResult()) {
			if (debug) cout << "Candidate::work new Leader" << endl;
			nextState = new Leader(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
				startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
			return;
		}
		persistence();
	}
}

// ע��ȴ�����AppendEntries���
void Candidate::registerHandleAppendEntries() {
	appendEntriesRpcServer->register_handler("appendEntries", &Candidate::appendEntries, this);
}
// ע��ͶƱ�߳�RequestVote���
void Candidate::registerHandleRequestVote() {
	requestVoteRpcServer->register_handler("requestVote", &Candidate::requestVote, this);
}
// ע��start�������
void Candidate::registerHandleStart() {
	startRpcServer->register_handler("start", &Candidate::start, this);
}
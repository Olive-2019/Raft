#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor, int maxResendNum) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, commitIndex, lastApplied, logEntries, votedFor), 
	getVoteCounter(1), maxResendNum(maxResendNum), rejectCounter(0), timeoutThread(NULL){
	
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
Answer Candidate::requestVote(string requestVoteCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	RequestVote requestVote(requestVoteCodedIntoString);
	if (debug) cout << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << endl;
	// termû�бȵ�ǰCandidate�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (requestVote.getTerm() <= currentTerm) return Answer{ currentTerm, false };
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = requestVote.getTerm();

	// ��Ӧ�ڷ��ؽ���Ժ�����������̣߳����Ǵ˴��޷���ô����
	// ������nextState��Ϊ�ź�������֤�̼߳�ͬ���ͷ�
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// ������һ״̬��
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		startAddress, commitIndex, lastApplied, logEntries);
	return Answer{ currentTerm, true };
}

void Candidate::timeoutCounterThread() {
	// ��ʱ���أ�ת����candidate
	if (timeoutCounter.run()) {
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, commitIndex, lastApplied, logEntries);
		if (debug) cout << "Candidate " << ID << " timeout and quit." << endl;
	}
	else if (debug) cout << "Candidate " << ID << " quit." << endl;
		
	// δ��ʱ���������أ���nextState�ĳ�ʼ������stop�ĵ��ô�
}

// ����AppendEntries������Ҫ���ü�ʱ����leader�м�ʱ��ֻ����һ��
// ֻҪ�Է���term�����Լ�С�ͽ��ܶԷ�Ϊleader
Answer Candidate::appendEntries(string appendEntriesCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive appendEntries Msg" << endl;
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// termû�бȵ�ǰCandidate�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (appendEntries.getTerm() < currentTerm) return Answer{ currentTerm, false };
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
	return Answer{ currentTerm, true };
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
		Answer answer(0, false);
		// û�з���ֵ���ط�
		if (!checkOneFollowerReturnValue(followerID, answer)) continue;
		// �з���ֵ������voteResult��getVoteCounter
		if (debug) cout << "receive the return value of " << followerID << ", and its result is " << answer.getSuccess() << endl;
		// �յ�ͶƱ��voteResult��Ϊ1
		if (answer.getSuccess()) follower->second = 1, getVoteCounter++;
		// û���յ��Ϸ�ͶƱ��voteResult��Ϊ0
		else {
			follower->second = -1;
			rejectCounter++;
			if (answer.getTerm() > currentTerm || rejectCounter > serverAddress.size() / 2) {
				if (answer.getTerm() > currentTerm) currentTerm = answer.getTerm();
				nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress,
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
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], "requestVote", requestVoteContent.code())
	);
	if (debug) cout << "send requestVote to " << followerID << " content is " << requestVoteContent.code() << endl;
}

// ��鵥��follower�����ɹ���true�������ɹ������ط�
bool Candidate::checkOneFollowerReturnValue(int followerID, Answer& ans) {

	for (auto val = followerReturnVal[followerID].begin(); val != followerReturnVal[followerID].end(); ) {
		future_status status = val->wait_for(seconds(0));
		if (status == future_status::ready) {
			timeoutCounter.setReceiveInfoFlag();
			// ����ط�����ֵΪ���ַ���������follower���������
			string returnValStr = val->get();
			//// ��֪��Ϊɶ����ֵ���ǿգ���Ӳ������
			//if (!returnValStr.size()) {
			//	val = followerReturnVal[followerID].erase(val);
			//	continue;
			//}
			ans = Answer(returnValStr);
			if (debug) cout << "Candidate::getOneFollowerReturnValue get return value from " << followerID << endl;
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
	if (debug) cout << endl << ID << " become Candidate" << endl;
	// ���뼯Ⱥ������server�ĵ�ַ��candidate����RequestVoteAddress�ĵ�ַ
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


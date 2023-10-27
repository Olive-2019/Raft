#include "Leader.h"
#include "Follower.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor, int maxResendNum) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, commitIndex, lastApplied, logEntries, votedFor),
    maxResendNum(maxResendNum){
	
}
Leader::~Leader() {
	if (debug) cout << ID << " will not be Leader any more." << endl;
}

// ����RequestVote
string Leader::requestVote(string requestVoteCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	RequestVote requestVote(requestVoteCodedIntoString);
	if (debug) cout << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << endl;
	// termû�бȵ�ǰleader�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (requestVote.getTerm() <= currentTerm) {
		if (debug) cout << "reject " << requestVote.getCandidateId() << ", cause its term is old." << endl;
		return Answer(currentTerm, false).code();
	}
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = requestVote.getTerm();
	
	// ��Ӧ�ڷ��ؽ���Ժ�����������̣߳����Ǵ˴��޷���ô����
	// ������nextState��Ϊ�ź�������֤�̼߳�ͬ���ͷ�
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// ������һ״̬��
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		startAddress, commitIndex, lastApplied, logEntries, votedFor = requestVote.getCandidateId());
	if (debug) cout << "vote for " << requestVote.getCandidateId() << "." << endl;
	return Answer(currentTerm, true).code();
}
// ����AppendEntries
string Leader::appendEntries(string appendEntriesCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive appendEntries Msg" << endl;
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// timeoutCounter.setReceiveInfoFlag();
	// termû�бȵ�ǰleader�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (appendEntries.getTerm() <= currentTerm) {
		if (debug) cout << "reject " << appendEntries.getLeaderId() << "'s appendEntries, cause its term is old." << endl;
		return Answer(currentTerm, false).code();
	}
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = appendEntries.getTerm();
	// ��entries��ӵ���ǰ�б��У����ú���������Ҫ�ж����ܷ���ӣ���һ����ʵ�Ѿ�����follower�Ĺ����ˣ�
	bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
		appendEntries.getLeaderCommit(), appendEntries.getEntries());
	if (!nextState || nextState->getCurrentTerm() <= currentTerm) {
		delete nextState;
		// ������һ״̬��
		nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, commitIndex, lastApplied, logEntries);
	} 
	return Answer(currentTerm, canAppend).code();
}

void Leader::checkFollowers() {
	// ѭ���������е�follower�������nextIndex�Ƿ����
		// 1. �з���ֵ��
		//  1.0 ����ֵterm���£���Ϊfollower
		//	1.1 ����ֵΪtrue������next��match����next��ͷ�ͷ�����(�����ķ��ػ���Ҫ�ټ��)
		//  1.2 ����ֵΪfalse��next--���ط�һ��
		// 2. �޷���ֵ���ط���һ���� 
	for (auto follower = nextIndex.begin(); follower != nextIndex.end(); ++follower) {
		int followerID = follower->first;
		// �޷���ֵ���ط���һ����
		if (!checkOneFollowerReturnValue(followerID)) continue;
		// �з���ֵ
		Answer answer = getOneFollowerReturnValue(followerID);
		if (debug) cout << "receive the return value of " << followerID << ", and its result is " << answer.getSuccess() << endl;
		// ����ֵterm���£���Ϊfollower
		if (answer.getTerm() > currentTerm) {
			nextState = new Follower(answer.getTerm(), ID, appendEntriesAddress, requestVoteAddress,
				startAddress, commitIndex, lastApplied, logEntries);
			return;
		}
		// ����ֵΪtrue������next��match����next��ͷ�ͷ�����
		if (answer.getSuccess()) {
			// ��һ��������������Ҫ����next��match
			if (lastAppendEntries[followerID].getEntries().size()) {
				nextIndex[followerID] = lastAppendEntries[followerID].getPrevLogIndex()
					+ lastAppendEntries[followerID].getEntries().size() + 1;
				matchIndex[followerID] = lastAppendEntries[followerID].getPrevLogIndex()
					+ lastAppendEntries[followerID].getEntries().size();
			}
			// next��ͷ�ˣ���Ҫ����������Ϣ
			if (nextIndex[followerID] >= logEntries.size()) sendAppendEntries(followerID, -1, -1);
			// nextû��ͷ���������Ķ�����ȥ
			else sendAppendEntries(followerID, nextIndex[followerID], logEntries.size() - 1);
		}
		// ����ֵΪfalse��next--���ط�һ��
		else {
			// needn't check the heartbreakt, cause if the heartbreak fail, it will be stop at the first one
			nextIndex[followerID]--;
			sendAppendEntries(followerID, nextIndex[followerID], nextIndex[followerID]);
		}
	}
}
// ��鵥��follower�����ɹ���true�������ɹ������ط�
bool Leader::checkOneFollowerReturnValue(int followerID) {
	// ����follower�ķ���ֵ
	for (int i = 0; i < followerReturnVal[followerID].size(); ++i) 
		if (followerReturnVal[followerID][i]._Is_ready()) return true;
	// ��û�з��أ������ط�
	sendAppendEntries(followerID);
	return false;
}
// ��ȡ����follower�ķ���ֵ
Answer Leader::getOneFollowerReturnValue(int followerID) {
	for (int i = 0; i < followerReturnVal[followerID].size(); ++i)
		if (followerReturnVal[followerID][i]._Is_ready()) {
			Answer ans(followerReturnVal[followerID][i].get());
			followerReturnVal[followerID].clear();
			return ans;
		}
	throw exception("Leader::getOneFollowerReturnValue Logical Error: didn't hava return value.");
	return Answer(0, false);
}

void Leader::updateCommit() {
	// commit��������follower����match��log entries
	while (commitIndex < logEntries.size()) {
		int counter = 0;
		for (auto it = matchIndex.begin(); it != matchIndex.end(); ++it)
			if (commitIndex + 1 >= it->second) counter++;
		if (counter > matchIndex.size() / 2) commitIndex++;
		else break;
	}
}
void Leader::sendAppendEntries(int followerID, int start, int end) {
	// �±�Ϸ����ж�
	if (end >= logEntries.size() || start > end) throw exception("Leader::sendAppendEntries: index is illegal");
	// follower id �Ϸ����ж�
	if (followerID < 0 || serverAddress.find(followerID) == serverAddress.end()) throw exception("Leader::sendAppendEntries: followerID is illegal");
	// ��ʼ��appendEntries������
	int prevTerm = -1, prevIndex = -1;
	// ��־��Ϣ
	vector<LogEntry> entries;
	// ��ʼ����־��Ϣ
	if (start >= 0) {
		prevIndex = start - 1;
		// ��ǰһ�����ڣ������ڷ��Ĳ��ǵ�һ��
		if (prevIndex >= 0) prevTerm = logEntries[prevIndex].getTerm();
		for (int index = start; index <= end; ++index) entries.push_back(logEntries[index]);
	}
		
	// ������
	lastAppendEntries[followerID] = AppendEntries(currentTerm, ID, prevIndex, prevTerm, commitIndex, entries);
	// �첽���� ��������
	sendAppendEntries(followerID);
	
}
bool Leader::sendAppendEntries(int followerID) {
	if (serverAddress.find(followerID) == serverAddress.end()) throw exception("Leader::resendAppendEntries follower doesn't exist.");
	// ����Ѿ���������ط����������ط���ֱ�ӷ���
	if (followerReturnVal[followerID].size() >= maxResendNum) return false;
	// �첽���� ��������
	followerReturnVal[followerID].push_back(
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], 
			"appendEntries", lastAppendEntries[followerID].code()));
	if (debug) cout << "send appendEntries to " << followerID << endl;
	return true;
}

//���������н���ͬ��log entries
void Leader::work() {


	if (debug) cout << endl << ID << " become Leader" << endl;

	// ���뼯Ⱥ������server�ĵ�ַ��leader����AppendEntriesAddress�ĵ�ַ
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");

	serverAddress = serverAddressReader.getNetWorkAddresses();
	// ���εĲ�����������������ʼ��nextIndex��matchIndex
	for (auto follower = serverAddress.begin(); follower != serverAddress.end(); ++follower) {
		if (follower->first == ID) continue;
		// ��ʼ��nextΪ��ǰlog�����һ��
		nextIndex[follower->first] = logEntries.size() - 1;
		// ��ʼ��matchAddressΪ-1
		matchIndex[follower->first] = -1;
		// ����������Ϣ(������)
		sendAppendEntries(follower->first, nextIndex[follower->first], nextIndex[follower->first]);
	}

	// ��nextState��Ϊͬ���ź���,��ʱ/�յ����µ���Ϣ��ʱ��Ϳ����˳���
	while (!nextState) {
		// ˯��һ��ʱ��
		sleep_for(seconds(15));
		checkFollowers();
		updateCommit();
	}
}

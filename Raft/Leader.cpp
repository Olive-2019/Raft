#include "Leader.h"
#include "Follower.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, 
	vector<LogEntry> logEntries, int votedFor, int maxResendNum, int snapshotThreshold) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, applyMessageAddress,
		commitIndex, lastApplied, logEntries, votedFor), maxResendNum(maxResendNum), snapshotThreshold(snapshotThreshold) {
	if (debug) cout << "Leader::Leader new a leader" << endl;
}
Leader::~Leader() {
	if (debug) cout << ID << " will not be Leader any more." << endl;
}

// ����RequestVote
Answer Leader::requestVote(rpc_conn conn, RequestVote requestVote) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//RequestVote requestVote(requestVoteCodedIntoString);
	if (debug) cout << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << endl;
	// termû�бȵ�ǰleader�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (requestVote.getTerm() <= currentTerm) {
		if (debug) cout << "reject " << requestVote.getCandidateId() << ", cause its term is old." << endl;
		return Answer( currentTerm, false );
	}
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = requestVote.getTerm();
	
	// ��Ӧ�ڷ��ؽ���Ժ�����������̣߳����Ǵ˴��޷���ô����
	// ������nextState��Ϊ�ź�������֤�̼߳�ͬ���ͷ�
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// ������һ״̬��
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries, votedFor = requestVote.getCandidateId());
	if (debug) cout << "vote for " << requestVote.getCandidateId() << "." << endl;
	return Answer( currentTerm, true );
}
// ����AppendEntries
Answer Leader::appendEntries(rpc_conn conn, AppendEntries appendEntries) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//AppendEntries appendEntries(appendEntriesCodedIntoString);
	if (debug) cout << ID << " receive appendEntries Msg from " << appendEntries.getLeaderId() << endl;
	// timeoutCounter.setReceiveInfoFlag();
	// termû�бȵ�ǰleader�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (appendEntries.getTerm() <= currentTerm) {
		if (debug) cout << "reject " << appendEntries.getLeaderId() << "'s appendEntries, cause its term is old." << endl;
		return Answer( currentTerm, false );
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
			startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
	}
	return Answer(currentTerm, canAppend);
}

void Leader::checkFollowers() {
	// ѭ���������е�follower�������nextIndex�Ƿ����
		// 1. �з���ֵ��
		//  1.0 ����ֵterm���£���Ϊfollower
		//	1.1 ����ֵΪtrue������next��match����next��ͷ�ͷ�����(�����ķ��ػ���Ҫ�ټ��)
		//  1.2 ����ֵΪfalse��next--���ط�һ��
		// 2. �޷���ֵ���ط���һ���� 
	for (auto follower = nextIndex.begin(); follower != nextIndex.end(); ++follower) {
		lock_guard<mutex> lockGuard(receiveInfoLock);
		int followerID = follower->first;
		// �޷���ֵ���ط���һ����
		if (!checkOneFollowerReturnValue(followerID)) continue;
		// �з���ֵ
		const Answer answer = getOneFollowerReturnValue(followerID);
		//if (debug) cout << "appendEntries get value from " << followerID << " content: term " << answer.term << " success " << answer.success << endl;
		// ����ֵterm���£���Ϊfollower
		if (answer.getTerm() > currentTerm) {
			nextState = new Follower(answer.getTerm(), ID, appendEntriesAddress, requestVoteAddress,
				startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
			return;
		}
		// ����ֵΪtrue������next��match����next��ͷ�ͷ�����
		if (answer.isSuccess()) {
			// ��һ��������������Ҫ����next��match
			if (lastAppendEntries[followerID].getEntries().size()) {
				nextIndex[followerID] = lastAppendEntries[followerID].getPrevLogIndex()
					+ lastAppendEntries[followerID].getEntries().size() + 1;
				matchIndex[followerID] = lastAppendEntries[followerID].getPrevLogIndex()
					+ lastAppendEntries[followerID].getEntries().size();
			}
			int logEntriesNum = logEntries.size();
			// next��ͷ�ˣ���Ҫ����������Ϣ(����ط������⣬ִ���߼�ò�������⣬�����²���û�м������µ�)
			if (nextIndex[followerID] >= logEntriesNum) {
				if (debug) cout << "Leader::checkFollowers nextIndex " << nextIndex[followerID] << " logEntries.size " << logEntriesNum << endl;
				sendAppendEntries(followerID, -1, -1);
			}
			else {
				// nextû��ͷ���������Ķ�����ȥ
				if (logEntries.size()) sendAppendEntries(followerID, nextIndex[followerID], logEntries.size() - 1);
				else sendAppendEntries(followerID, -1, -1);
			}
			
		}
		// ����ֵΪfalse��next--���ط�һ��
		else {
			// needn't check the heartbreakt, cause if the heartbreak fail, it will be stop at the first one
			nextIndex[followerID]--;
			sendAppendEntries(followerID, nextIndex[followerID], logEntries.size() - 1);
		}
	}
}
// ��鵥��follower�����ɹ���true�������ɹ������ط�
bool Leader::checkOneFollowerReturnValue(int followerID, bool snapshot) {
	// ����follower�ķ���ֵ����Ҫ�����Ƿ�snapshot��
	auto val = followerReturnVal[followerID].begin(), end = followerReturnVal[followerID].end();
	if (snapshot) val = snapshotReturnVal[followerID].begin(), end = snapshotReturnVal[followerID].end();
	while (val != end) {
		future_status status = val->wait_for(seconds(0));
		if (status == future_status::ready) return true;
		else if (status == future_status::timeout) {
			if (snapshot) val = snapshotReturnVal[followerID].erase(val);
			else val = followerReturnVal[followerID].erase(val);
		} 
		else val++;
	}
	// ��û�з��أ������ط�
	sendAppendEntries(followerID, snapshot);
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
	return Answer( 0, false );
}

void Leader::updateCommit() {
	// commit��������follower����match��log entries
	while (commitIndex < logEntries.size()) {
		int counter = 0;
		for (auto it = matchIndex.begin(); it != matchIndex.end(); ++it)
			if (commitIndex + 1 >= it->second) counter++;
			else break;
		if (counter + 1 > matchIndex.size() / 2) commitIndex++;
		else break;
	}
	if (commitIndex >= logEntries.size()) commitIndex = logEntries.size() - 1;
	// ��δ������ֵ��������applyMsg
	if (commitIndex < snapshotThreshold) applyMsg();
	// ��commit������������ֵ����Ҫ��һ���߳�ִ�п��ղ���
	else async(&Leader::snapshot, this);
}

void Leader::applyMsg(bool snapshot, int snapshotIndex) {
	//if (debug) cout << "Leader::applyMsg content logEntries.size() " << logEntries.size() << " commitIndex " << commitIndex << endl;
	/*
	* snapshot��Ϊд���ձ�־����Ϊд���գ���indexΪ�����±�
	* ��Ϊ��ͨapplyMsg����ΪcommitedIndex
	*/
	int applyIndex = commitIndex;
	if (snapshot) applyIndex = snapshotIndex;
	if (applyIndex < 0 || applyIndex >= logEntries.size()) throw exception("Leader::applyMsg logical error: index is negative or greater than the log");
	vector<string> commands;
	// ����index��������������
	for (int i = 0; i <= applyIndex; ++i) commands.push_back(logEntries[i].getCommand());
	rpc.invokeRemoteApplyMsg(applyMessageAddress, ApplyMsg(commands, applyIndex, snapshot));
}

void Leader::sendAppendEntries(int followerID, int start, int end, bool snapshot, int snapshotIndex) {
	// �±�Ϸ����ж�
	if (start >= 0 && (end >= logEntries.size() || start > end)) {
		if (debug) cout << "Leader::sendAppendEntries: start " << start << " end " << end << " logEntries.size() " << logEntries.size() << endl;
		throw exception("Leader::sendAppendEntries: index is illegal");
	}
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
	if (snapshot) snapshotLastAppendEntries[followerID] = AppendEntries(currentTerm, ID, prevIndex, prevTerm, snapshotIndex, entries, snapshot);
	else lastAppendEntries[followerID] = AppendEntries(currentTerm, ID, prevIndex, prevTerm, commitIndex, entries, snapshot);
	// �첽���� ��������
	sendAppendEntries(followerID, snapshot);
	
}
bool Leader::sendAppendEntries(int followerID, bool snapshot) {
	if (serverAddress.find(followerID) == serverAddress.end()) throw exception("Leader::resendAppendEntries follower doesn't exist.");
	int sendNum = followerReturnVal[followerID].size();
	AppendEntries sendEntries = lastAppendEntries[followerID];
	if (snapshot) sendNum = snapshotReturnVal[followerID].size(), sendEntries = snapshotLastAppendEntries[followerID];
	// ����Ѿ���������ط����������ط���ֱ�ӷ���
	if (sendNum >= maxResendNum) return false;
	// �첽���� ��������
	shared_future<Answer> ans = async(&RPC::invokeAppendEntries, &rpc, serverAddress[followerID], sendEntries);
	// ���첽������������
	if (snapshot) snapshotReturnVal[followerID].push_back(ans);
	else followerReturnVal[followerID].push_back(ans);
	if (debug) cout << "send appendEntries to " << followerID << " content size is " << sendEntries.getEntries().size() << endl;
	return true;
}

//���������н���ͬ��log entries
void Leader::work() {

	if (debug) cout << endl << ID << " work as Leader" << endl;

	// ���뼯Ⱥ������server�ĵ�ַ��leader����AppendEntriesAddress�ĵ�ַ
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");

	serverAddress = serverAddressReader.getNetWorkAddresses();
	// ���εĲ�����������������ʼ��nextIndex��matchIndex
	for (auto follower = serverAddress.begin(); follower != serverAddress.end(); ++follower) {
		int followerID = follower->first;
		// ����ʼ���Լ�
		if (followerID == ID) continue;
		// ��ʼ��nextΪ��ǰlog�����һ��
		nextIndex[followerID] = logEntries.size() - 1;
		// ��ʼ��matchAddressΪ-1
		matchIndex[followerID] = -1;
		// ����������Ϣ(������)
		sendAppendEntries(followerID, nextIndex[followerID], nextIndex[followerID]);
	}

	// ��nextState��Ϊͬ���ź���,��ʱ/�յ����µ���Ϣ��ʱ��Ϳ����˳���
	while (!nextState) {
		// ˯��һ��ʱ��
		sleep_for(seconds(2));
		checkFollowers();
		updateCommit();
		persistence();
		// ģ��ͣ��
		//if (crush(0.7)) break;
	}
}

// ע��ȴ�����AppendEntries���
void Leader::registerHandleAppendEntries() {
	appendEntriesRpcServer->register_handler("appendEntries", &Leader::appendEntries, this);
}
// ע��ͶƱ�߳�RequestVote���
void Leader::registerHandleRequestVote() {
	requestVoteRpcServer->register_handler("requestVote", &Leader::requestVote, this);
}
// ע��start�������
void Leader::registerHandleStart() {
	startRpcServer->register_handler("start", &Leader::start, this);
}

void Leader::snapshot() {
	int snapshotIndex = commitIndex;
	// ֪ͨÿһ��ϵͳ����գ�����������ͨ������Խ�����һ��
	informSnapshot(snapshotIndex);
	// �޸��Լ���״̬��֪ͨ�ϲ�Ӧ��д����
	snapShotModifyState(snapshotIndex);
}
void Leader::informSnapshot(int snapshotIndex) {
	map<int, bool> hasReturn;
	int counter = 0;
	for (auto follower = followerReturnVal.begin(); follower != followerReturnVal.end(); ++follower) {
		int followerID = follower->first;
		// ����snapshot��AppendEntries
		sendAppendEntries(followerID, 0, snapshotIndex, true);
		// ��ʼ������ֵ
		hasReturn[followerID] = false;
	}
	// ����Ƿ���룬û����Ͳ�ͣ�ط�
	while (counter < serverAddress.size() / 2) {
		for (auto follower = followerReturnVal.begin(); follower != followerReturnVal.end(); ++follower) {
			int followerID = follower->first;
			if (hasReturn[followerID]) continue;
			if (checkOneFollowerReturnValue(followerID, true)) {
				hasReturn[followerID] = true;
				counter++;
			}
		}
	}
}


void Leader::snapShotModifyState(int snapshotIndex) {
	// ��Ҫ�޸�״̬�����Բ��������������̸߳�״̬
	lock_guard<mutex> lockGuard(receiveInfoLock);
	// ֪ͨ�ϲ�Ӧ�ÿ���д����
	applyMsg(true, snapshotIndex);
	logEntries.erase(logEntries.begin(), logEntries.begin() + snapshotIndex);
	for (auto follower = nextIndex.begin(); follower != nextIndex.end(); ++follower) {
		int followerID = follower->first;
		// ����next
		nextIndex[followerID] -= snapshotIndex;
		// ����matchAddress
		matchIndex[followerID] -= snapshotIndex;
		// TODO:֪ͨÿһ��followerҪ�������
	}
}

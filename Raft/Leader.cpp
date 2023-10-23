#include "Leader.h"
#include "Follower.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries) {
	// ��������start���߳�
	startThread = new thread(&Leader::registerStart, this);
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
}
Leader::~Leader() {
	// ��start�߳�joinһ��
	startThread->join();
	// �ͷ��̶߳���
	delete startThread;
}

// ����RequestVote
string Leader::requestVote(string requestVoteCodedIntoString) {
	receiveInfoLock.lock();
	
	RequestVote requestVote(requestVoteCodedIntoString);
	// termû�бȵ�ǰleader�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (requestVote.getTerm() <= currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = requestVote.getTerm();
	
	// ��Ӧ�ڷ��ؽ���Ժ�����������̣߳����Ǵ˴��޷���ô����
	// ������nextState��Ϊ�ź�������֤�̼߳�ͬ���ͷ�
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// ������һ״̬��
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}
// ����AppendEntries
string Leader::appendEntries(string appendEntriesCodedIntoString) {
	receiveInfoLock.lock();
	// ����������Ϣ
	timeoutCounter.setReceiveInfoFlag();
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// termû�бȵ�ǰleader�󣬿���ֱ�Ӿܾ��������ص�ǰ��term
	if (appendEntries.getTerm() <= currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term���£����˳���ǰ״̬�����ص�Follower��״̬
	currentTerm = appendEntries.getTerm();
	// ��entries��ӵ���ǰ�б��У����ú���������Ҫ�ж����ܷ���ӣ���һ����ʵ�Ѿ�����follower�Ĺ����ˣ�
	bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
		appendEntries.getLeaderCommit(), appendEntries.getEntries());
	// ������һ״̬��
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, canAppend).code();
}
State* Leader::run() {
	// ��һ���̸߳�start
	thread startThread(&Leader::registerStart, this);
	// ���̴߳�����appendEntries�����߳��м�������߳��Ƿ����˳���Ϊ
	work();
	return nextState;
}

void Leader::start(AppendEntries newEntries) {
	receiveInfoLock.lock();
	//��client�������ݼ��뵱ǰ�б���
	merge(logEntries.begin(), logEntries.end(), newEntries.getEntries().begin(),
		newEntries.getEntries().end(), std::back_inserter(logEntries));
	// �������ӵ�entries������lastApplied
	lastApplied += newEntries.getEntries().size();
	receiveInfoLock.unlock();
}
// ע��start����
void Leader::registerStart() {
	startRpcServer.reset(nullptr);
	startRpcServer.reset(new rpc_server(startAddress.second, 6));
	startRpcServer->register_handler("start", [this](rpc_conn conn,
		string newEntries) {
			this->start(std::move(newEntries));
		});
	startRpcServer->run();//���������
	cout << "Leader::registerStart close start" << endl;
}

//���������н���ͬ��log entries
void Leader::work() {
	// ��nextState��Ϊͬ���ź���,��ʱ/�յ����µ���Ϣ��ʱ��Ϳ����˳���
	while (!nextState) {
		// ˯��һ��ʱ��
		Sleep(300);
		// ѭ���������е�follower�������nextIndex�Ƿ����
		// 1. �з���ֵ��
		//  1.0 ����ֵterm���£���Ϊfollower
		//	1.1 ����ֵΪtrue������next��match����next��ͷ�ͷ�����(�����ķ��ػ���Ҫ�ټ��)
		//  1.2 ����ֵΪfalse��next--���ط�һ��
		// 2. �޷���ֵ���ط���һ���� 
		for (auto follower = nextIndex.begin(); follower != nextIndex.end(); ++follower) {
			int followerID = follower->first;
			// �޷���ֵ���ط���һ����
			if (!followerReturnVal[followerID]._Is_ready()) {
				resendAppendEntries(followerID);
				continue;
			}
			// �з���ֵ
			Answer answer(followerReturnVal[followerID].get());
			// ����ֵterm���£���Ϊfollower
			if (answer.getTerm() > currentTerm) {
				nextState = new Follower(answer.getTerm(), ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries);
				timeoutCounter.stopCounter();
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
	if (start >= 0) 
		for (int index = start; index <= end; ++index) entries.push_back(logEntries[index]);
	// ������
	lastAppendEntries[followerID] = AppendEntries(currentTerm, ID, prevIndex, prevTerm, commitIndex, entries);
	// �첽���� ��������
	followerReturnVal[followerID] = 
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], "appendEntries", lastAppendEntries[followerID].code());
}
void Leader::resendAppendEntries(int followerID) {
	// �첽���� ��������
	followerReturnVal[followerID] =
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], "appendEntries", lastAppendEntries[followerID].code());
}
void Leader::stopThread() {
	State::stopThread();
	startRpcServer.reset(nullptr);
}
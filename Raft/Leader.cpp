#include "Leader.h"
#include "Follower.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries),
	nextIndex(serverAddress.size(), logEntries.size()), matchIndex(serverAddress.size(), 0)
{
	for ()
	// ��������
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
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// timeoutCounter.setReceiveInfoFlag();
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
	State::run();
	work();
	waitThread();
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
	// ��nextState��Ϊͬ���ź���
	while (!nextState) {
		/*��ʼ��next*/
	}
}
Answer Leader::sendAppendEntries(int followerID, int start, int end) {
	if (end >= logEntries.size()) throw exception("Leader::sendAppendEntries end is illegal");
	vector<LogEntry> entries;
	// ���͵�index��term�ȶ���ʼ��Ϊ-2�����������Ҫ���͵������ٸ�ֵ 
	int prevIndex = -1, prevTerm = -1;
	if (start >= 0) {
		prevIndex = start - 1;
		// ��ǰһ�����ڣ������ڷ��Ĳ��ǵ�һ��
		if (prevIndex >= 0) prevTerm = logEntries[prevIndex].getTerm();
		for (int i = start; i <= end; ++i) entries.push_back(logEntries[i]);
	}
	// ��ȡ���Դ�����ַ���
	string appendEntriesStr = AppendEntries(currentTerm, ID, prevIndex, prevTerm, commitIndex, entries).code();
	rpc_client client(serverAddress[followerID].first, serverAddress[followerID].second);// IP ��ַ���˿ں�
	/*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
	bool has_connected = client.connect(5);
	/*û�н����������˳�����*/
	if (!has_connected) throw exception("Leader::sendAppendEntries connect timeout");
	string appendEntriesRes = client.call<string>("appendEntries", appendEntriesStr);
	return Answer(appendEntriesRes);
}
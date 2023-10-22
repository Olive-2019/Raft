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
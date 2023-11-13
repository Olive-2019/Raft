#include "Raft.h"
using namespace std::chrono;
using namespace std::this_thread;

Raft::Raft(NetWorkAddress raftServerAddress, int applyMsgPort, KVserver* kvServer) : raftServerAddress(raftServerAddress),
commitedIndex(-1), applyMsgPort(applyMsgPort), debug(false), kvServer(kvServer){
    applyMsgThread = new thread(&Raft::registerApplyMsg, this);
    setDebug();
}
Raft::~Raft() {
    applyMsgThread->join();
    delete applyMsgThread;
}
void Raft::applyMsg(rpc_conn conn, ApplyMsg applyMsg) {
    if (debug) cout << "Raft::applyMsg content:command " << applyMsg.getCommands()[0].getKey() << " index " << applyMsg.getIndex();
    // ��������(��дand����)
    updateCommands(applyMsg.getCommands());
    // ִ��
    execute(applyMsg.getIndex());
    // ����Ҫд���գ������д���̵ĺ���
    if (applyMsg.isSnapshot()) snapshot(applyMsg.getIndex());
}
void Raft::registerApplyMsg() {
    rpc_server server(applyMsgPort, 6);
    server.register_handler("applyMsg", &Raft::applyMsg, this);
    server.run();
}

StartAnswer Raft::start(Command command) {
    commands.push_back(command);
    if (debug) cout << "Raft::start command " << command.getID() << " " << command.getKey() << endl;
    rpc_client client(raftServerAddress.first, raftServerAddress.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("Raft::start can't connect");

    /*����Զ�̷��񣬷��ػ�ӭ��Ϣ*/
    StartAnswer ans;
    try {
        StartAnswer ans = client.call<StartAnswer>("start", command);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
        //if (debug) cout << "Raft::start commnad " << command << " term " << ans.term << " index " << ans.index << endl;
        return ans;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}

void Raft::setDebug() {
    debug = true;
}
void Raft::run() {
    /*vector<Command> commands({ Command(CommandType::Append, "Lam", "55190906"),Command(CommandType::Append, "somebody", "55190922") });
    for (Command command : commands) {
        sleep_for(seconds(3));
        if (debug) cout << "Raft::run " << command.getKey() << endl;
        StartAnswer ans = start(command);
    }*/
}
void Raft::execute(int newCommitIndex) {
    // ִ���߼�
    for (int i = commitedIndex + 1; i <= newCommitIndex; ++i) {
        // ִ������
        kvServer->execute(commands[i]);
        //if (debug) cout << "Raft::execute " << commands[i].getKey() << endl;
    }
    // ����committedIndex
    commitedIndex = newCommitIndex;
}

void Raft::snapshot(int snapshotIndex) {
    // ����ǰ״̬д����
    if (debug) cout << "Raft::snapshot write disk" << endl;
    kvServer->snapshot();
    // �޸�״̬��Ҫ��һ�Ѵ���
    lock_guard<mutex> lockGuard(stateLock);
    commands.erase(commands.begin(), commands.begin() + snapshotIndex);
    commitedIndex -= (snapshotIndex + 1);
}
void Raft::updateCommands(vector<Command> commands) {
    // �ҵ���һ����ͬ��command
    int firstIndex = this->commands.size();
    for (int i = 0; i < this->commands.size(); ++i) 
        if (commands[0] == this->commands[i]) {
            firstIndex = i;
            break;
        }
    // ������Ǹ�дor׷��
    for (int i = 0; i < commands.size(); ++i) {
        if (i + firstIndex >= this->commands.size()) this->commands.push_back(commands[i]);
        else this->commands[i + firstIndex] = commands[i];
    }
    
    //commitedIndex = commands.size() - 1;
}
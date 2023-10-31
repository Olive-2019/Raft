#include "Raft.h"
using namespace std::chrono;
using namespace std::this_thread;

Raft::Raft(NetWorkAddress raftServerAddress, int applyMsgPort) : raftServerAddress(raftServerAddress),
commitedIndex(-1), applyMsgPort(applyMsgPort), debug(false) {
    applyMsgThread = new thread(&Raft::registerApplyMsg, this);
    setDebug();
}
Raft::~Raft() {
    applyMsgThread->join();
    delete applyMsgThread;
}
void Raft::applyMsg(rpc_conn conn, ApplyMsg applyMsg) {
    if (debug) cout << "Raft::applyMsg content:command " << applyMsg.getCommands()[0] << " index " << applyMsg.getIndex();
    // ��������(��дand����)
    updateCommands(applyMsg.getCommands());
    // ִ��
    execute(applyMsg.getIndex());
    // ����д���̵ĺ���
    if (applyMsg.isSnapshot()) snapshot();
}
void Raft::registerApplyMsg() {
    rpc_server server(applyMsgPort, 6);
    server.register_handler("applyMsg", &Raft::applyMsg, this);
    server.run();
}

StartAnswer Raft::start(string command) {
    commands.push_back(command);
    rpc_client client(raftServerAddress.first, raftServerAddress.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("Raft::start can't connect");

    /*����Զ�̷��񣬷��ػ�ӭ��Ϣ*/
    StartAnswer ans;
    try {
        StartAnswer ans = client.call<StartAnswer>("start", command);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
        if (debug) cout << "Raft::start commnad " << command << " term " << ans.term << " index " << ans.index << endl;
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
    vector<string> commands({ "Lam", "somebody", "55190906", "55190922" });
    for (string command : commands) {
        sleep_for(seconds(3));
        if (debug) cout << "Raft::run " << command << endl;
        StartAnswer ans = start(command);
    }
}
void Raft::execute(int newCommitIndex) {
    // TODO:ִ���߼�
    for (int i = commitedIndex + 1; i <= newCommitIndex; ++i) {
        // TODO:ִ������
    }
    // ����committedIndex
    commitedIndex = newCommitIndex;
}

void Raft::snapshot() {
    // TODO:����ǰ״̬д����
    // �޸�״̬
    commands.erase(commands.begin(), commands.begin() + commitedIndex);
    commitedIndex = -1;
}
void Raft::updateCommands(vector<string> commands) {
    for (int i = 0; i < commands.size(); ++i) {
        if (i >= this->commands.size()) this->commands.push_back(commands[i]);
        else {
            if (commands[i] != this->commands[i]) {
                if (i < commitedIndex) commitedIndex = i;
                this->commands[i] = commands[i];
            }
        }
    }
}
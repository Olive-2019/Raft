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
void Raft::applyMsg(rpc_conn conn, string command, int index) {
    //if (commands[index] == command) {
    if (debug) cout << "Raft::applyMsg content:command " << command << " index " << index;
    commitedIndex = index;
    //}
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
        auto result = client.call<StartAnswer>("start", command);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
        ans = result;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    if (debug) cout << "Raft::start commnad " << command << " term " << ans.term << " index " << ans.index << endl;
    return ans;
}

void Raft::setDebug() {
    debug = true;
}
void Raft::run() {
    for (int i = 1; i < 10; ++i) {
        sleep_for(seconds(3));
        commands.push_back(to_string(i));
        StartAnswer ans = start(commands[i - 1]);
    }
}
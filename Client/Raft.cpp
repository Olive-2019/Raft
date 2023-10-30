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
    rpc_client client(raftServerAddress.first, raftServerAddress.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("Raft::start can't connect");

    /*调用远程服务，返回欢迎信息*/
    StartAnswer ans;
    try {
        auto result = client.call<StartAnswer>("start", command);// funcName 为事先注册好的服务名，需要一个 arg 参数
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
        commands.push_back("Lam " + to_string(i));
        StartAnswer ans = start(commands[i - 1]);
    }
}
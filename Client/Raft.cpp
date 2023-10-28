#include "Raft.h"
#include <include/rest_rpc/rpc_server.h>
using namespace rest_rpc::rpc_service;

Raft::Raft(NetWorkAddress raftServerAddress) : raftServerAddress(raftServerAddress), 
commitedIndex(commitedIndex){}
bool Raft::applyMsg(string command, int index) {
    if (commands[index] == command) {
        commitedIndex = index;
        return true;
    }
    return false;
}
void Raft::registerApplyMsg() {
    rpc_server server(9000, 6);
    server.register_handler("applyMsg", applyMsg, this);
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
    /*if (funcName == "requestVote") {
        cout << "RPC::invokeRemoteFunc" << endl;
    }*/
    StartAnswer ans;
    try {
        auto result = client.call<StartAnswer>("start", command);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
        ans = result;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    cout << "Raft::start " << ans.term << ' ' << ans.index << endl;
    return ans;
}

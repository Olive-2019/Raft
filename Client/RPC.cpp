#include "RPC.h"
void RPC::invokeGetData(NetWorkAddress address, int commandID, string value) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeGetData can't connect");

    try {
        client.call<void>("getData", commandID, value);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
int RPC::invokeRemoteAcceptCommand(NetWorkAddress address, Command command) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeRemoteAcceptCommand can't connect");

    try {
        return client.call<int>("acceptCommand", command);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
// ��KVCtrler����join
int RPC::invokeJoin(NetWorkAddress address, NetWorkAddress selfAddress) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeJoin can't connect");

    try {
        return client.call<int>("join", selfAddress);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}

// ��KVCtrler��Ҫ��ǰgroup��Ӧ��shardid
vector<int> RPC::invokeQueryShardID(NetWorkAddress address, int group) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeQueryShardID can't connect");

    try {
        return client.call<vector<int>>("queryShardIDByGroupID", group);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}


// ��KVCtrler��Ҫ��ǰshardid��Ӧ���µ�ַ
NetWorkAddress RPC::invokeQueryNewGroup(NetWorkAddress address, int shardID) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeQueryNewGroup can't connect");

    try {
        return client.call<NetWorkAddress>("queryGroupAddressByShardID", shardID);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
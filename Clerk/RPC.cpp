#include "RPC.h"
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

// ����shardCtrler��ȡ�����ַ
NetWorkAddress RPC::invokeQueryGroupAddressByShardID(NetWorkAddress address, int shardID) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeQueryGroupAddressByShardID can't connect");

    try {
        return client.call<NetWorkAddress>("queryGroupAddressByShardID", shardID);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
// ����shardCtrler��ȡ�Լ��ķ��䵽��shard
vector<int> RPC::invokeQueryShardIDByGroupID(NetWorkAddress address, int groupID) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeQueryShardIDByGroupID can't connect");

    try {
        return client.call<vector<int>>("queryShardIDByGroupID", groupID);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
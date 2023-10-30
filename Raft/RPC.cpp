#pragma once
#include "RPC.h"
#include "Answer.h"
Answer RPC::invokeRemoteFunc(NetWorkAddress address, string funcName, string arg) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeRemoteFunc can't connect");

    /*����Զ�̷��񣬷��ػ�ӭ��Ϣ*/
    if (funcName == "appendEntries") {
        cout << "RPC::invokeRemoteFunc appendEntries content:" << arg << endl;
    }
    Answer ans;
    try {
        auto result = client.call<Answer>(funcName, arg);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
        ans = result;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    //cout << "RPC::invokeRemoteFunc " << ans.term << ' ' << ans.success << endl;
    return ans;
}
StartAnswer RPC::invokeRemoteStart(NetWorkAddress address, string arg) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeRemoteFunc can't connect");

    /*����Զ�̷��񣬷��ػ�ӭ��Ϣ*/
    /*if (funcName == "requestVote") {
        cout << "RPC::invokeRemoteFunc" << endl;
    }*/
    StartAnswer ans;
    try {
        auto result = client.call<StartAnswer>("start", arg);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
        ans = result;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    //cout << "RPC::invokeRemoteFunc " << ans.term << ' ' << ans.index<< endl;
    return ans;
}
void RPC::invokeRemoteApplyMsg(NetWorkAddress address, string command, int index) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeRemoteFunc can't connect");

    /*����Զ�̷��񣬷��ػ�ӭ��Ϣ*/
    /*if (funcName == "requestVote") {
        cout << "RPC::invokeRemoteFunc" << endl;
    }*/
    try {
        client.call<bool>("applyMsg", command, index);
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}

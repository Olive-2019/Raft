#include "RPC.h"

Answer RPC::invokeRemoteFunc(NetWorkAddress address, string funcName, string arg) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeRemoteFunc can't connect");

    /*����Զ�̷��񣬷��ػ�ӭ��Ϣ*/
    /*if (funcName == "requestVote") {
        cout << "RPC::invokeRemoteFunc" << endl;
    }*/
    string ans;
    try {
        auto result = client.call<std::string>(funcName, arg);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
        ans = result;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    cout << "RPC::invokeRemoteFunc " << ans << endl;
    return ans;
}

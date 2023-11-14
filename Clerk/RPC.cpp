#include "RPC.h"
void RPC::invokeRemoteAcceptCommand(NetWorkAddress address, Command command) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeRequestVote can't connect");

    try {
        client.call<void>("acceptCommand", command);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}

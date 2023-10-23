#include "RPC.h"

string RPC::invokeRemoteFunc(NetWorkAddress address, string funcName, string arg) {
    rpc_client client(address.first, address.second);// IP ��ַ���˿ں�
    /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
    bool has_connected = client.connect(5);
    /*û�н����������˳�����*/
    if (!has_connected) throw exception("RPC::invokeRemoteFunc can't connect");

    /*����Զ�̷��񣬷��ػ�ӭ��Ϣ*/
    string result = client.call<std::string>(funcName, arg);// funcName Ϊ����ע��õķ���������Ҫһ�� arg ����
    // cout << result << endl;
    return result;
}

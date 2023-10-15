#include "source.h"
//#include <memory>
#include <thread>
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using std::unique_ptr;
using namespace rest_rpc;
using std::thread;
std::string hello(rpc_conn conn, std::string name) {
	/*����Ϊ void �������ͣ�������ú󲻸�Զ�̿ͻ��˷�����Ϣ*/
	return ("Hello " + name); /*���ظ�Զ�̿ͻ��˵�����*/
}
unique_ptr<rpc_server> p;
void run() {
	p.reset(new rpc_server(9000, 6));
	p->register_handler("func_greet", hello);
	p->run();//���������
	cout << "close" << endl;
}
int main() {
	thread t(run);
	Sleep(500);
	p.reset(nullptr);
	t.join();
	
	return 0;
}
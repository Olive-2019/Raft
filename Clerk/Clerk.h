#pragma once
#include "source.h"
#include "RPC.h"
#include <atomic>
using std::atomic;
class Clerk
{
	NetWorkAddress kvServerAddress;
	RPC rpc;
	int invokeCommand(Command command);
	// �������ݵĵ�ַ
	NetWorkAddress getDataAddress;
	thread* getDataThread;
	// ע��getData����
	void registerGetData();
	//��������
	map<int, string> dataCache;
	//mutex dataCacheLock;
public:
	Clerk(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress);
	~Clerk();
	// �������
	void put(string key, string value);
	// ��ĳ��key׷������
	void append(string key, string value);
	// ɾ��������
	void deleteData(string key);
	// ͬ����get
	string get(string key);
	// �������ݵ��ýӿ�
	void getData(rpc_conn conn, int commandID, string value);
	// �첽��get������commandID
	int aget(string key);
	
};

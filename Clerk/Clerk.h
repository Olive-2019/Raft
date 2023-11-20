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
	// ��ɾ�Ĳ�����ַ
	vector<int> crudPorts;
	// ��ɾ�Ĳ��߳�ָ��
	vector<thread*> crudThreads;
	// ע����ɾ�Ĳ��ĸ�����
	void registerPut();
	void registerDelete();
	void registerAppend();
	void registerGet();


	//��������
	map<int, string> dataCache;
	//mutex dataCacheLock;
public:
	// ������kv���ݿ��ַ��get�������ݵĵ�ַ����������put delete append get�Ķ˿ں�
	Clerk(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, vector<int> crudPort);
	~Clerk();
	// �������
	void put(rpc_conn conn, string key, string value);
	// ��ĳ��key׷������
	void append(rpc_conn conn, string key, string value);
	// ɾ��������
	void deleteData(rpc_conn conn, string key);
	// ͬ����get
	string get(rpc_conn conn, string key);
	// �������ݵ��ýӿ�
	void getData(rpc_conn conn, int commandID, string value);
	// �첽��get������commandID
	int aget(rpc_conn conn, string key);
	
};

#pragma once
#include "source.h"
#include "RPC.h"
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
public:
	Clerk(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress);
	~Clerk();
	void put(string key, string value);
	void append(string key, string value);
	// �������ݵ��ýӿ�
	void getData(rpc_conn conn, int commandID, string value);
	// �첽��get������commandID
	int aget(string key);
};

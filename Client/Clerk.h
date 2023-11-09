#pragma once
#include "KVserver.h"
class Clerk
{
	KVserver kvServer;
public:
	Clerk(NetWorkAddress raftServerAddress, int applyMsgPort = 8001, string snapshotFilePath = "snapshot.data");
	void put(string key, string value);
	void append(string key, string value);
	// ͬ��������get
	string get(string key);
	// �첽��get������commandID
	int aget(string key);
};


#pragma once
#include "source.h"
#include "RPC.h"
class Clerk
{
	NetWorkAddress kvServerAddress;
	RPC rpc;
	int invokeCommand(Command command);
public:
	Clerk(NetWorkAddress kvServerAddress);
	void put(string key, string value);
	void append(string key, string value);
	// ͬ��������get
	string get(string key);
	// �첽��get������commandID
	int aget(string key);
};

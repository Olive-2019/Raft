#pragma once
#include "source.h"
enum CommandType
{
	Append, Put, Get, Delete
};
class Command
{
	/*static int IDCounter;
	static mutex IDCounterLock;*/
	// ���������clerk��ַ
	NetWorkAddress clerkAddress;
	// ��ǰ���������
	int commandType;
	// ����Ĳ���
	string key, value;
	// �����ID����Ψһ��ʶ����ֻ�������ƴ���
	int ID;
public:
	Command() {}
	MSGPACK_DEFINE(commandType, key, value, ID, clerkAddress);
	Command(CommandType commandType, NetWorkAddress clerkAddress, string key, string value = "");
	bool operator==(Command other)const;
	int getID() const { return ID; }
	CommandType getType() const { return CommandType(commandType); }
	string getKey() const { return key; }
	string getValue() const { return value; }
	void setID(int ID);
	NetWorkAddress getClerkAddress() const { return clerkAddress; }
};


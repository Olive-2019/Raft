#pragma once
#include "source.h"
#include <mutex>
using std::mutex;
enum CommandType
{
	Append, Put, Get
};
class Command
{
	// ���������clerk��ַ
	NetWorkAddress clerkAddress;
	// ��ǰ���������
	int commandType;
	// ����Ĳ���
	string key, value;
	// �����ID����Ψһ��ʶ��
	int ID;
public:
	MSGPACK_DEFINE(commandType, key, value, ID, clerkAddress);
	Command(){}
	Command(CommandType commandType, int ID, string key, string value = "");
	bool operator==(Command other)const;
	int getID() const { return ID; }
	CommandType getType() const { return CommandType(commandType); }
	string getKey() const { return key; }
	string getValue() const { return value; }

};


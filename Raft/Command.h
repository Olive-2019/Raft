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
	static int IDCounter;
	static mutex IDCounterLock;
	// ��ǰ���������
	CommandType commandType;
	// ����Ĳ���
	string key, value;
	// �����ID����Ψһ��ʶ����
	int ID;
public:
	MSGPACK_DEFINE(commandType, key, value);
	Command(){}
	Command(CommandType commandType, string key, string value = "");
	bool operator==(Command other)const;
	int getID() const { return ID; }
	CommandType getType() const { return commandType; }
	string getKey() const { return key; }
	string getValue() const { return value; }

};


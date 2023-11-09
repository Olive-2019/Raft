#pragma once
#include "source.h"
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
	Command() {}
	MSGPACK_DEFINE(commandType, key, value, ID);
	Command(CommandType commandType, string key, string value = "");
	bool operator==(Command other)const;
	int getID() const { return ID; }
	CommandType getType() const { return commandType; }
	string getKey() const { return key; }
	string getValue() const { return value; }

};


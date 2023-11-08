#pragma once
#include "source.h"
enum CommandType
{
	Append, Put, Get
};
class Command
{
	// ��ǰ���������
	CommandType commandType;
	// ����Ĳ���
	string key, value;
public:
	Command(CommandType commandType, string key = "", string value = "");
	CommandType getType() const { return commandType; }
	string getKey() const { return key; }
	string getValue() const { return value; }

};


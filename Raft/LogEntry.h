#pragma once
#include "POJO.h"

class LogEntry : public POJO
{
	int term;
	string command;
public:
	MSGPACK_DEFINE(term, command);
	LogEntry() {}
	LogEntry(int term, string command);
	// ���л�
	LogEntry(string codedString);
	// �����л�
	//MSGPACK_DEFINE(term, command);
	string code() const;

	string getCommand() const;
	int getTerm() const;
};


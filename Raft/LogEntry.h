#pragma once
#include "POJO.h"
#include "Command.h"
class LogEntry : public POJO
{
	int term;
	Command command;
public:
	MSGPACK_DEFINE(term, command);
	LogEntry() {}
	LogEntry(int term, Command command);
	//// ���л�
	//LogEntry(string codedString);
	//// �����л�
	////MSGPACK_DEFINE(term, command);
	//string code() const;

	Command getCommand() const;
	int getTerm() const;
};


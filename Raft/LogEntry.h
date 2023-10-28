#pragma once
#include "POJO.h"
#include <include/rest_rpc/rpc_client.hpp>

class LogEntry : public POJO
{
	int term;
	string command;
public:
	LogEntry() {}
	LogEntry(int term, string command);
	// ���л�
	LogEntry(string codedString);
	// �����л�
	MSGPACK_DEFINE(term, command);
	string code() const;

	string getCommand() const;
	int getTerm() const;
};


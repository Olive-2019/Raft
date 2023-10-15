#pragma once
#include "POJO.h"

class LogEntry : public POJO
{
	int term;
	string command;
public:
	LogEntry(int term, string command);
	// ���л�
	LogEntry(string codedString);
	// �����л�
	string code() const;

	string getCommand() const;
	int getTerm() const;
};


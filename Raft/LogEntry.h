#pragma once
#include <string>
using std::string;
#include "POJO.h"
using std::to_string;

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


#pragma once
#include "POJO.h"
class Answer : public POJO
{
	int term;
	bool success;
public:
	Answer(int term, bool success);
	// �����л�
	Answer(string codedString);
	// ���л�
	string code() const;

	int getTerm() const;
	bool getSuccess() const;
};


#pragma once
#include "source.h"
class FileOperator
{
	string filePath;
public:
	FileOperator(string filePath);
	// ��map���л�����filePathд��
	void write(const map<string, string>& data);
	// ��file�ж������ݲ������л�Ϊmap<string, string>
	map<string, string> read()const;
};


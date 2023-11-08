#pragma once
#include "source.h"
#include <boost/serialization/map.hpp> 
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <sstream>
#include <fstream>
using std::fstream;
using std::ostringstream;
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


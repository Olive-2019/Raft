#pragma once
#include "source.h"
#include <fstream>
using std::fstream;
class FileOperator
{
	string filePath;
	fstream fileHandle;
public:
	FileOperator(string filePath);
	// ��ȡÿһ�У�����ĩβ���Զ��ر��ļ�
	string getOneRaw();
	// �Ƿ��ȡ���ļ�ĩβ
	bool isEnd() const;
	// ���ļ���ѡ��ģʽ
	void openFile(std::ios_base::openmode openMode);

};


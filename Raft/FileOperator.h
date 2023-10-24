#pragma once
#include "source.h"
#include <fstream>
#include <sstream>
using std::ifstream;
using std::fstream;
using std::stringstream;
class FileOperator
{
protected:

	string filePath;
	fstream fileHandle;
	// ��ȡÿһ�У�����ĩβ���Զ��ر��ļ�
	string getOneRaw();
	// �Ƿ��ȡ���ļ�ĩβ
	bool isEnd() const;
	// ���ļ���ѡ��ģʽ
	void openFile(std::ios_base::openmode openMode);

	// ����Ƿ�����ļ�
	bool isExistFile();
	// appendдһ��
	void appendOneRow(string content);
	// ��дһ��
	void writeOneRow(string content);
public:
	FileOperator(string filePath);
};


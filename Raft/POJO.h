#pragma once
#include "string"
#include <exception>
using std::string;
using std::to_string;
using std::exception;
class POJO
{
protected:
	/*���빤��*/
	//���ص�һ�����������֣�index������,����������ո�
	int getFirstNumber(const string& buff, int& index) const;
	//����ָ�������ַ�����index������
	string getSettedLenString(const string& buff, int& index, int size)const;
public:
	virtual string code() const  = 0;
};


#pragma once
#include "source.h"
class POJO
{
protected:
	/*���빤��*/
	//���ص�һ�����������֣�index������,����������ո�
	int getFirstNumber(const string& buff, int& index) const;
	//����ָ�������ַ�����index������
	string getSettedLenString(const string& buff, int& index, int size)const;
public:
	//virtual string code() const  = 0;
};


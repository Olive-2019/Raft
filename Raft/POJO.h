#pragma once
#include "source.h"
#include <include/rest_rpc/rpc_client.hpp>
class POJO
{
protected:
	/*���빤��*/
	//���ص�һ�����������֣�index������,����������ո�
	int getFirstNumber(const string& buff, int& index) const;
	//����ָ�������ַ�����index������
	string getSettedLenString(const string& buff, int& index, int size)const;
	// ���ش�ָ��λ�ÿ�ʼ����һ���ո���ַ���
	string getNextString(const string& buff, int& index) const;
public:
};


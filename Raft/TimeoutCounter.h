#pragma once
#include <random>
#include <mutex>
using std::mutex;
class TimeoutCounter
{
	int electionTimeouts;
	//flag���ڱ�־���������Ƿ��յ�����������յ�����true
	bool receiveInfoFlag;
	//���ڷ�ֹflag�ö�
	mutex flagLock;
public:
	// ��ʼ����ʱ��ʱ�������һ����ʱ��ֵ
	TimeoutCounter();
	// �յ���Ϣʱ���ñ�־λ
	void setReceiveInfoFlag();
	// �������У�����ܷ��أ�˵��������ڳ�ʱ��
	void run();
};


#pragma once
#include <random>
#include <mutex>
using std::mutex;
class TimeoutCounter
{
	int electionTimeouts;
	// flag���ڱ�־���������Ƿ��յ�����������յ�����true
	bool receiveInfoFlag;
	// ���ڷ�ֹflag�ö�
	mutex flagLock;
	// �Ƿ�ֹͣ
	bool stop;
public:
	// ��ʼ����ʱ��ʱ�������һ����ʱ��ֵ
	TimeoutCounter();
	// �յ���Ϣʱ���ñ�־λ
	void setReceiveInfoFlag();
	// ��������
	// ����ֵΪtrueʱ˵����ʱ���أ�Ϊfalse˵���Ǳ�ϵͳ������ͣ�ģ���stop�ˣ�
	bool run();
	// ��ʱ��ֹͣ���˳�
	void stopCounter();
};


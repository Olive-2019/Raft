#pragma once
#include <random>
#include <mutex>
#include <thread>
#include <chrono>
using std::this_thread::sleep_for;
using std::chrono::seconds;

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
	// ����һ����������ֲ�����ƽ���ֲ�
	int getRandom(int start, int end);
	// �յ���Ϣʱ���ñ�־λ
	void setReceiveInfoFlag();
	// ��������
	// ����ֵΪtrueʱ˵����ʱ���أ�Ϊfalse˵���Ǳ�ϵͳ������ͣ�ģ���stop�ˣ�
	bool run();
	// ��ʱ��ֹͣ���˳�
	void stopCounter();
};


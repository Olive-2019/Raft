#include "TimeoutCounter.h"
TimeoutCounter::TimeoutCounter() {
	receiveInfoFlag = false;
	stop = false;
	electionTimeouts = getRandom(4, 5);
	//electionTimeouts = getRandom(10, 50);
}
int TimeoutCounter::getRandom(int start, int end) {
	std::default_random_engine randomEngine;
	randomEngine.seed(time(0));
	std::uniform_int_distribution<int> u(start, end); // ����ұ�����
	return u(randomEngine);
}
void TimeoutCounter::setReceiveInfoFlag() {
	flagLock.lock();
	receiveInfoFlag = true;
	flagLock.unlock();
}
bool TimeoutCounter::run() {
	while (!stop) {
		flagLock.lock();
		receiveInfoFlag = false;
		flagLock.unlock();
		// ˯��ʱ��Ƭ
		sleep_for(seconds(electionTimeouts));
		flagLock.lock();
		if (!receiveInfoFlag) {
			flagLock.unlock();
			return true;
		}
		flagLock.unlock();
	}
	return false;
}
void TimeoutCounter::stopCounter() {
	stop = true;
}
#pragma once
#include "State.h"
class Leader : public State
{
	// ��Ҫ����ÿһ��follower����һ��log entry����ʼ��ֵ��leader�����һ��log entry����һ��ֵ��
	vector<int> nextIndex;
	// ÿ��follower��ǰƥ�䵽��һ��log entry����ʼ��ֵΪ0��
	vector<int> matchIndex;
};


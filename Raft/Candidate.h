#pragma once
#include "State.h"
class Candidate : public State
{
	// ͶƱ��� 0:û���յ� 1��Ӯ�ø�ѡƱ -1������
	vector<int> voteResult;

	// ��Ⱥ�и��������ĵ�ַ(����RequestVote)
	map<int, NetWorkAddress> serverAddress;
public:

};


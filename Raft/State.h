#pragma once
#include "ServerState.h"
#include "LogEntry.h"
class State
{
	/*��Ҫ�־û���state*/
	//server������term��Ҳ�Ǹ�server����֪������term
	int currentTerm;
	//�����-1�����term����δͶƱ�������candidateID�����term�е�ѡ���Ѿ�ͶƱ���˸�candidate
	int votedFor;
	// ��ǰserver������log entries��ÿһ��log entry���������term���
	vector<LogEntry> logEntries;
	ServerState state;
};


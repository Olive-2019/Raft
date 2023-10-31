#pragma once
#include "LogEntry.h"
#include <sstream>
#include "POJO.h"
#include "LogEntry.h"
#include <include/rest_rpc/rpc_client.hpp>
#include <vector>
using std::vector;
using std::stringstream;
class AppendEntries : public POJO
{
	//leader�ĵ�ǰterm���
	int term;
	// leader��id������follower���յ�����Ϣת����leader
	int leaderId;
	// ǰһ��log��index������followerȷ��һ����
	int prevLogIndex;
	// ǰһ��log��term������ͬ��
	int prevLogTerm;
	// leader�ύ����־ entry index
	int leaderCommit;
	// ��Ҫ�����Ķ���log entry��������Ϣ���ǿ�
	vector<LogEntry> entries;

	// ���ձ�־λ
	bool snapshot;

public:
	AppendEntries() {  }
	AppendEntries(int term, int leaderId, int prevLogIndex, int prevLogTerm, int leaderCommit, 
		vector<LogEntry> entries, bool snapshot = false) :
		term(term), leaderId(leaderId), prevLogIndex(prevLogIndex), prevLogTerm(prevLogTerm), 
		entries(entries), leaderCommit(leaderCommit), snapshot(snapshot) {}
	MSGPACK_DEFINE(term, leaderId, prevLogIndex, prevLogTerm, leaderCommit, entries, snapshot);
	int getTerm()const { return term; }
	int getLeaderId()const { return leaderId; }
	int getPrevLogIndex()const { return prevLogIndex; }
	int getPrevLogTerm()const { return prevLogTerm; }
	int getLeaderCommit()const { return leaderCommit; }
	vector<LogEntry> getEntries()const { return entries; }
	bool isSnapshot() const { return snapshot; }
};


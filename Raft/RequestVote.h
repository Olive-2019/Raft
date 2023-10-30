#pragma once
//#include "source.h"
#include "POJO.h"
#include <include/rest_rpc/rpc_client.hpp>
class RequestVote : public POJO
{
	// ��ǰ��ѡleader��candidate��term
	int term;
	// ��ǰ��ѡleader��candidate��ID
	int candidateId;
	// ���һ��log entry��index
	int lastLogIndex;
	// ���һ��log entry��term
	int lastLogTerm;
public:
	MSGPACK_DEFINE(term, candidateId, lastLogIndex, lastLogTerm);
	RequestVote(){}
	RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm);
	// �����л�
	//RequestVote(string codedString);
	// ���л�
	string code() const;

	int getTerm() const { return term; }
	int getLastLogIndex() const { return lastLogIndex; }
	int getLastLogTerm() const { return lastLogTerm; }
	int getCandidateId() const { return candidateId; }
};


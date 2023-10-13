#pragma once
#include "POJO.h"
class RequestVote : public POJO
{
	// 当前竞选leader的candidate的term
	int term;
	// 当前竞选leader的candidate的ID
	int candidateId;
	// 最后一条log entry的index
	int lastLogIndex;
	// 最后一条log entry的term
	int lastLogTerm;
public:
	RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm);
	// 反序列化
	RequestVote(string codedString);
	// 序列化
	string code() const;

	int getTerm() const { return term; }
	int getLastLogIndex() const { return lastLogIndex; }
	int getLastLogTerm() const { return lastLogTerm; }
	int getCandidateId() const { return candidateId; }
};


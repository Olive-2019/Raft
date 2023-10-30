#pragma once
#include "source.h"
#include "Answer.h"
#include "StartAnswer.h"
#include "AppendEntries.h"
#include "RequestVote.h"
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using namespace rest_rpc;
//��������RPC����/
class RPC
{

public:
	// ���ú�����������
	Answer invokeRequestVote(NetWorkAddress address, RequestVote requestVote);
	Answer invokeAppendEntries(NetWorkAddress address, AppendEntries appendEntries);
	
	//Answer invokeRemoteFunc(NetWorkAddress address, string funcName, string arg);


	StartAnswer invokeRemoteStart(NetWorkAddress address, string arg);

	void invokeRemoteApplyMsg(NetWorkAddress address, string command, int index);
};


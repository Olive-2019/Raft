#pragma once
#include "source.h"
#include "Answer.h"
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using namespace rest_rpc;
//��������RPC����/
class RPC
{

public:
	// ���ú�����������
	Answer invokeRemoteFunc(NetWorkAddress address, string funcName, string arg);
	
};


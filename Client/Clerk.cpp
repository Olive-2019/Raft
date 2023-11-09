#include "Clerk.h"
Clerk::Clerk(NetWorkAddress raftServerAddress, int applyMsgPort, string snapshotFilePath):
 kvServer(raftServerAddress, applyMsgPort, snapshotFilePath){

}
void Clerk::put(string key, string value) {
	Command command(CommandType::Put, key, value);
	kvServer.acceptCommand(command);
}
void Clerk::append(string key, string value) {
	Command command(CommandType::Append, key, value);
	kvServer.acceptCommand(command);
}
// ͬ��������get
string Clerk::get(string key) {
	int commandID = aget(key);
	string value;
	while (true) {
		if (kvServer.getData(commandID, value)) break;
	}
	return value;
}
// �첽��get
int Clerk::aget(string key) {
	Command command(CommandType::Get, key);
	kvServer.acceptCommand(command);
	return command.getID();
}
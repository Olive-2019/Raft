#include "AppendEntries.h"
AppendEntries::AppendEntries(int term, int leaderId, int prevLogIndex, 
	int prevLogTerm, int leaderCommit, vector<LogEntry> entries) : 
term(term), leaderId(leaderId), prevLogIndex(prevLogIndex), 
prevLogTerm(prevLogTerm), entries(entries), leaderCommit(leaderCommit){}
AppendEntries::AppendEntries(string codedString) {
	// �������ǰ������ǰ�˳������ԣ�����vector�ԡ�n (len content) * n������ʽ����
	int index = 0, n, len;
	term = getFirstNumber(codedString, index);
	prevLogIndex = getFirstNumber(codedString, index);
	prevLogTerm = getFirstNumber(codedString, index);
	leaderCommit = getFirstNumber(codedString, index);
	leaderId = getFirstNumber(codedString, index);
	n = getFirstNumber(codedString, index);
	while (n--) {
		len = getFirstNumber(codedString, index);
		string logEntryString = getSettedLenString(codedString, index, len);
		entries.push_back(LogEntry(logEntryString));
	}
}
string AppendEntries::code() const {
	int n = entries.size();
	string buff = to_string(term) + " " + to_string(prevLogIndex) + " " + to_string(prevLogTerm) + " "
		+ to_string(leaderCommit) + " " + to_string(leaderId) + " " + to_string(n) + " ";
	for (int i = 0; i < n; ++i) {
		string entryBuff = entries[i].code();
		buff += to_string(entryBuff.size());
		buff += " ";
		buff += entryBuff;
	}
	return buff;
}
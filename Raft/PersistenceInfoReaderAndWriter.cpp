#include "PersistenceInfoReaderAndWriter.h"
PersistenceInfoReaderAndWriter::PersistenceInfoReaderAndWriter(int serverID) :
	filePath("RaftPersistance"+to_string(serverID)), FileOperator(filePath) {
	// �����ǰid���ļ����������
	
	// �����ǰid��Ӧ�ĳ־û��ļ������ڣ���д��
	
}
void PersistenceInfoReaderAndWriter::setCurrentTerm(int currentTerm) {
	this->currentTerm = currentTerm;
}
void PersistenceInfoReaderAndWriter::setVotedFor(int votedFor) {
	this->votedFor = votedFor;
}
void PersistenceInfoReaderAndWriter::setEntries(vector<LogEntry> entries) {
	this->entries = entries;
}

int PersistenceInfoReaderAndWriter::getCurrentTerm() const {
	return currentTerm;
}
int PersistenceInfoReaderAndWriter::getVotedFor() const {
	return votedFor;
}
vector<LogEntry> PersistenceInfoReaderAndWriter::getEntries() const {
	return entries;
}
bool PersistenceInfoReaderAndWriter::read() {
	if (isExistFile()) {
		openFile(std::ios::in);
		string currentTermAndVotedFor = getOneRaw();
		stringstream ss(currentTermAndVotedFor);
		ss >> currentTerm;
		ss >> votedFor;
		while (!isEnd()) {
			string entryCodedStr = getOneRaw();
			if (!entryCodedStr.size()) break;
			entries.push_back(LogEntry(entryCodedStr));
		}
		return true;
	}
	// ���³�ʼ��
	currentTerm = 0;
	votedFor = -1;
	write();
	return false;
}
void PersistenceInfoReaderAndWriter::write() {
	writeOneRow(to_string(currentTerm) + " " + to_string(votedFor));
	for (LogEntry entry : entries) appendOneRow(entry.code());
}
#pragma once
#include "FileOperator.h"
#include "LogEntry.h"
class PersistenceInfoReaderAndWriter :
    public FileOperator {
    int currentTerm;
    int votedFor;
    vector<LogEntry> entries;
public:
    PersistenceInfoReaderAndWriter(int serverID);

    // ��������
    void setCurrentTerm(int currentTerm);
    void setVotedFor(int votedFor);
    void setEntries(vector<LogEntry> entries);

    // ��ȡ����
    int getCurrentTerm() const;
    int getVotedFor() const;
    vector<LogEntry> getEntries() const;

    // �Ӵ����ж�������
    bool read();
    // �����д����
    void write();

};


#include "FileOperator.h"
FileOperator::FileOperator(string filePath) : filePath(filePath) {
}
string FileOperator::getOneRaw() {
	if (!fileHandle.is_open()) throw exception("FileOperator::getOneRaw don't open file");
	string oneRawBuff;
	getline(fileHandle, oneRawBuff);
	if (isEnd()) fileHandle.close();
	return oneRawBuff;
}
bool FileOperator::isEnd() const {
	return !fileHandle.is_open() || fileHandle.eof();
}
void FileOperator::openFile(std::ios_base::openmode openMode) {
	if (fileHandle.is_open()) fileHandle.close();
	fileHandle.open(filePath.c_str(), openMode);
}

#include "Answer.h"
Answer::Answer(int term, bool success) : term(term), success(success) {}
// �����л�
Answer::Answer(string codedString) {
	int index = 0;
	term = getFirstNumber(codedString, index);
	success = getFirstNumber(codedString, index);
}
// ���л�
string Answer::code() const {
	return to_string(term) + " " + to_string(int(success));
}
int Answer::getTerm() const {
	return term;
}
bool Answer::getSuccess() const {
	return success;
}
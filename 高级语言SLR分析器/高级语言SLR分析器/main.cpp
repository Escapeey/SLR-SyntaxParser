# include "SLR.h"
# include "tokenizer.h"
using namespace std;



int main() {
	ifstream input("test.txt");
	if (!input) {
		cout << " ��ʧ��" << endl;
		return 0;
	}
	//��ȡtoken
	char c;
	string program = "";
	int line = 1;
	cout << "Դ�����token����Ϊ" << endl;
	cout << line << "  ";
	while ((c = input.get()) != EOF) {
		cout << c;
		if (c == '\n') {
			cout << ++line << "  ";
			program += " ";
		}
		else {
			program += c;
		}
	}
	cout << endl;

	cout << program << endl;
	cout << endl;
	program = removeWhitespace(program);
	cout << program << endl;
	cout << endl;
	program = wordAnalysisToID(program);
	cout << program << endl;
	cout << endl;

	string filename = "wenfa.txt";
	SLR grammar(filename);
	grammar.printP();
	grammar.analysis(program);
	return 0;
}
# include "SLR.h"
# include "tokenizer.h"
using namespace std;



int main() {
	ifstream input("test.txt");
	if (!input) {
		cout << " 打开失败" << endl;
		return 0;
	}
	//读取token
	char c;
	string program = "";
	int line = 1;
	cout << "源程序的token序列为" << endl;
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
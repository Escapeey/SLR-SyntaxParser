#include "tokenizer.h"
#include "Item.h"
using namespace std;

vector<string> keys = { "(", ")", "{", "}", "=", "==", "!=", "<", ">", "<=", ">=", "return", "int",
                        "bool", "double", "float", "void", "+", "-", "*", "/", "if", "else", "for",
                        "while", "break", "continue", ";" , " ", "$", ","};

bool AllisNum(string str) {
    for (int i = 0; i < str.size(); i++) {
        if (!isdigit(str[i]))
            return false;
    }
    return true;
}

string wordAnalysisToID(string code) {
    vector<string> words = split(code, " ");
    string ret = "";
    for (string word : words) {
        if (find(keys.begin(), keys.end(), word) == keys.end()) {
            if (!(isdigit(word[0]) && word.size() > 1 && !AllisNum(word))) {
                ret += "id ";
            }
            else {
                cout << "关键字输入错误 " << endl;
            }
        }
        else {
            ret += word + " ";
        }
    }
    ret.erase(ret.end() - 1);
    return ret;
}


//去除类高级语言代码中的空格，制表符和换行符
string removeWhitespace(const string& code) {
    string result;

    for (char ch : code) {
        if (ch != '\n' && ch != '\t') {
            result.push_back(ch);
        }
        else if(ch == '\n')
        {
            result.push_back(' ');
        }
    }

    return result;
}
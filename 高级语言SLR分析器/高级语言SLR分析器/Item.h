#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <cstring>
#include <algorithm>
#include <set>
#include <map>
#include <stack>
#include <vector>
#include <queue>
#include <utility>
#include <iomanip>
using namespace std;

//�з��ַ���
vector<string> split(string s, string separator);

//����s�ĵ�һ����
string firstWord(string s);

class Item
{
private:
	string item;
	string left;
	string right;
	static int count;
public:
	int id;
	// ���캯�� �������ʽ
	Item(string product);
	Item(string l, string r);
	string getLeft();
	string getRight();
	string getItem();

	// ���ص��λ��
	int getDotPos(string item);

	// ������ʽ �ӵ�
	void addDot(int pos);

	// �жϵ�ǰ��Ŀ�Ƿ��� �����Ŀ�����ж�����Ƿ���ĩβ
	int hasNextItem();

	// ���� "."��ĵ�һ���ķ�����
	string getPath();

	// ���ص�ǰ��Ŀ�� �����Ŀ���Ҳ�
	string getNextItemRight();
	
	bool operator == (Item& other);
};

bool operator <(const Item& x, const Item& y);

bool operator ==(const set<Item>& x, const set<Item>& y);
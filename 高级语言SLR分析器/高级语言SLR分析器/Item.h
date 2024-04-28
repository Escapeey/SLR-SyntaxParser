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

//切分字符串
vector<string> split(string s, string separator);

//返回s的第一个词
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
	// 构造函数 读入产生式
	Item(string product);
	Item(string l, string r);
	string getLeft();
	string getRight();
	string getItem();

	// 返回点的位置
	int getDotPos(string item);

	// 给产生式 加点
	void addDot(int pos);

	// 判断当前项目是否还有 后继项目，即判断其点是否在末尾
	int hasNextItem();

	// 返回 "."后的第一个文法符号
	string getPath();

	// 返回当前项目的 后继项目的右部
	string getNextItemRight();
	
	bool operator == (Item& other);
};

bool operator <(const Item& x, const Item& y);

bool operator ==(const set<Item>& x, const set<Item>& y);
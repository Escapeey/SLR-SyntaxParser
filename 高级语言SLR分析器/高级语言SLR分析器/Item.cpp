#include "Item.h"

using namespace std;

vector<string> split(string s, string separator){
	vector<string>v;

	string::size_type pos1, pos2;
	pos2 = s.find(separator);
	pos1 = 0;

	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + separator.size();
		pos2 = s.find(separator, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
	return v;
}

//返回s的第一个词
string firstWord(string s){
	s += " ";
	string first = s.substr(0, s.find(" "));
	return first;
}

// 构造函数 读入产生式
Item::Item(string product) {
	id++;
	left = product.substr(0, product.find("->"));
	left.erase(left.end()-1);
	right = product.substr(product.find("->") + 3);
	if (right.find(".") == string::npos) {
		addDot(0);
	}
	item = left + " -> " + right;
}
Item::Item(string l, string r) {
	id = count++;
	left = l;
	right = r;
	if (right.find(".") == string::npos) {
		addDot(0);
	}
	item = left + " -> " + right;
}
string Item::getLeft() {
	return left;
}
string Item::getRight() {
	return right;
}
string Item::getItem() {
	return left + " -> " + right;
}
// 返回点的位置
int Item::getDotPos(string item) {
	return item.find(".");
}
// 给产生式 加点
void Item::addDot(int pos) {
	if (right[pos] == '@')
		right = ".";
	else if (pos == 0)
		right.insert(pos, ". ");
	else if (pos == right.size())
		right.insert(pos, " .");
	else
		right.insert(pos, " . ");
}

// 判断当前项目是否还有 后继项目，即判断其点是否在末尾
int Item::hasNextItem() {
	vector<string>buffer = split(right, ".");
	if (buffer.size() > 1)
		return 1;
	else
		return 0;
}

// 返回 "."后的第一个文法符号
string Item::getPath() {
	vector<string>buffer = split(item, ".");
	buffer[1].erase(0, 1); //删除"."后第一个空格
	string first = firstWord(buffer[1]);
	return first;
}

// 返回当前项目的 后继项目的右部
string Item::getNextItemRight() {
	int dotPos = right.find(".");
	string first = getPath();
	int nextPos = dotPos + first.size();
	right.erase(right.find("."), 2);
	right.insert(nextPos, " .");
	return right;
}

bool Item::operator == (Item& other) {
	return getItem() == other.getItem();
}
int Item::count = 0;

//====================================================
bool operator <(const Item& x, const Item& y){
	return x.id < y.id;
}

bool operator ==(const set<Item>& x, const set<Item>& y){
	auto it1 = x.begin();
	auto it2 = y.begin();

	for (; it1 != x.end(), it2 != y.end(); it1++, it2++){
		Item a = *it1;
		Item b = *it2;
		if (a == b)
			continue;
		//有一个项目不相等，两项目集一定不相等
		else
			return false;
	}
	return true;
}
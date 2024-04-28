#pragma once
#include "Item.h"
using namespace std;
struct State {
	// 状态编号
	int id; 
	// 项目集
	set<Item>items;
};
struct Edge{
	// 原状态编号
	int source; 
	// 目标状态编号
	int target;
	// 边上符号
	string path;
	
	Edge(int s, string p, int t);
};
class SLR
{
private:
	int number = 0;
	vector<string> T;	//终结符号集合
	vector<string> V;	//非终结符号集合
	string S;			//开始符号
	map<string, vector<string>>production;  //产生式
	map<string, int> numPro;				//编号的产生式集合，用于规约
	map<string, set<string>>FIRST;			//FIRST集
	map<string, set<string>>FOLLOW;			//FOLLOW集
	set<State> States;  //状态集合
	vector<Edge> Edges; //转换函数
	map<pair<int, string>, string>actionTable;//action表
	map<pair<int, string>, int>gotoTable;//goto表


	//读取文法规则
	void readGrammer(string gramFilename);
	//添加产生式
	void addProduct(string left, string right);
	//带标号的产生式集
	void addNumProduct();
	//完成终结符集
	void addTerminal();
	//判断是否是非终结符
	bool isV(string token);
	//判断temp在不在集合Clos中
	bool isInC(Item temp, set<Item> Clos);
	//判断是否应该规约 id为当前状态编号
	string tableReduce(int id);
	//找到item规约到的产生式，返回其编号
	int findReduce(int id);
	//状态集是否已经包含该状态
	int hasState(set<Item> Items);
	//找到产生式序号为pro的产生式右部数量
	int rightCount(string& left, int pro);

public:
	// 构造函数，读入文法
	SLR(string gramFilename);
	// 语法分析
	void analysis(string tokensFilename);
	//语法分析过程
	bool analysing(string tokens);
	// 构造SLR(1)分析表
	void makeSLRtable();
	// 构造状态转换图
	set <Item> GO(set <Item> Items, string op);
	void make_dfa();
	void makeFA();
	// closure函数
	set<Item> closure(Item item);
	// 拓展文法
	void extension();
	// 构建FIRST集
	void makeFirst();
	// 构造符号序列的FIRST集
	set<string> makeFirstSeries(vector<string> input);
	// 构建FOLLOW集
	void makeFollow();
	/******************************************/
	//打印V和T
	void printVT();
	//打印FOLLOW集
	void printFOLLOW();
	//打印分析表
	void printTable();
	//打印产生式
	void printP();
	//打印状态表
	void printS();
	//打印状态转移函数
	void printEdges();
	//打印项目集
	void printItems(set<Item>I);
};

bool operator <(const State& x, const State& y);

bool operator <(const Edge& x, const Edge& y);

bool operator ==(const Edge& x, const Edge& y);

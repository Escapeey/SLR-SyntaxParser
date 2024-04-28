#pragma once
#include "Item.h"
using namespace std;
struct State {
	// ״̬���
	int id; 
	// ��Ŀ��
	set<Item>items;
};
struct Edge{
	// ԭ״̬���
	int source; 
	// Ŀ��״̬���
	int target;
	// ���Ϸ���
	string path;
	
	Edge(int s, string p, int t);
};
class SLR
{
private:
	int number = 0;
	vector<string> T;	//�ս���ż���
	vector<string> V;	//���ս���ż���
	string S;			//��ʼ����
	map<string, vector<string>>production;  //����ʽ
	map<string, int> numPro;				//��ŵĲ���ʽ���ϣ����ڹ�Լ
	map<string, set<string>>FIRST;			//FIRST��
	map<string, set<string>>FOLLOW;			//FOLLOW��
	set<State> States;  //״̬����
	vector<Edge> Edges; //ת������
	map<pair<int, string>, string>actionTable;//action��
	map<pair<int, string>, int>gotoTable;//goto��


	//��ȡ�ķ�����
	void readGrammer(string gramFilename);
	//��Ӳ���ʽ
	void addProduct(string left, string right);
	//����ŵĲ���ʽ��
	void addNumProduct();
	//����ս����
	void addTerminal();
	//�ж��Ƿ��Ƿ��ս��
	bool isV(string token);
	//�ж�temp�ڲ��ڼ���Clos��
	bool isInC(Item temp, set<Item> Clos);
	//�ж��Ƿ�Ӧ�ù�Լ idΪ��ǰ״̬���
	string tableReduce(int id);
	//�ҵ�item��Լ���Ĳ���ʽ����������
	int findReduce(int id);
	//״̬���Ƿ��Ѿ�������״̬
	int hasState(set<Item> Items);
	//�ҵ�����ʽ���Ϊpro�Ĳ���ʽ�Ҳ�����
	int rightCount(string& left, int pro);

public:
	// ���캯���������ķ�
	SLR(string gramFilename);
	// �﷨����
	void analysis(string tokensFilename);
	//�﷨��������
	bool analysing(string tokens);
	// ����SLR(1)������
	void makeSLRtable();
	// ����״̬ת��ͼ
	set <Item> GO(set <Item> Items, string op);
	void make_dfa();
	void makeFA();
	// closure����
	set<Item> closure(Item item);
	// ��չ�ķ�
	void extension();
	// ����FIRST��
	void makeFirst();
	// ����������е�FIRST��
	set<string> makeFirstSeries(vector<string> input);
	// ����FOLLOW��
	void makeFollow();
	/******************************************/
	//��ӡV��T
	void printVT();
	//��ӡFOLLOW��
	void printFOLLOW();
	//��ӡ������
	void printTable();
	//��ӡ����ʽ
	void printP();
	//��ӡ״̬��
	void printS();
	//��ӡ״̬ת�ƺ���
	void printEdges();
	//��ӡ��Ŀ��
	void printItems(set<Item>I);
};

bool operator <(const State& x, const State& y);

bool operator <(const Edge& x, const Edge& y);

bool operator ==(const Edge& x, const Edge& y);

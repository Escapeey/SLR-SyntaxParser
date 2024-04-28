#include "SLR.h"
using namespace std;

Edge::Edge(int s, string p, int t) {
	source = s;
	path = p;
	target = t;
}

// һЩ������������
bool operator <(const State& x, const State& y){

	return x.id < y.id;
}

bool operator <(const Edge& x, const Edge& y){

	return x.source < y.source;
}

bool operator ==(const Edge& x, const Edge& y){
	return x.source == y.source && x.path == y.path && x.target == y.target;
}

void SLR::readGrammer(string gramFilename) {
	ifstream input(gramFilename);
	if (!input) {
		cout << gramFilename << " ��ȡʧ��!!! " << endl;
	}
	//���ж�ȡ�ķ� �� �ֽ�Ϊ����ʽ
	string line;
	while (getline(input, line)) {
		// ��ȡ��
		string left = line.substr(0, line.find_first_of('-'));
		left.erase(left.end()-1);
		// �󲿼�����ս���ż�
		V.push_back(left);

		// ��ȡ�Ҳ�
		string right = line.substr(line.find_first_of('-') + 3);
		addProduct(left, right);
	}
	// ����ս����
	addTerminal();
	S = *V.begin();
	input.close();
}

// ����ս����
void SLR::addTerminal() {
	string temp = "";
	for (string left : V) {
		for (string right : production[left]) {
			right += "#";
			for (int i = 0; i < right.size(); i++) {
				if (right[i] == ' ' || right[i] == '#') {
					if (find(V.begin(), V.end(), temp) == V.end() && temp != "@") {
						T.push_back(temp);
					}
					temp = "";
				}
				else {
					temp += right[i];
				}
			}
		}
	}
	//�ս��ȥ��
	sort(T.begin(), T.end());
	T.erase(unique(T.begin(), T.end()), T.end());
}

//��Ӳ���ʽ
void SLR::addProduct(string left, string right) {
	right += " #"; // ��Ϊÿ���ķ���β��־
	string pR = "";
	for (int i = 0; i < right.size(); i++) {
		if (right[i] == '|' || right[i] == '#') {
			pR.erase(pR.end()-1);
			production[left].push_back(pR);
			i++;
			pR = "";
		}
		else {
			pR += right[i];
		}
	}
}

//����ŵĲ���ʽ��
void SLR::addNumProduct(){
	int i = 0;
	for (string left : V){
		for (string right : production[left]){
			numPro[left + " -> " + right] = i;
			i++;
		}
	};
}

bool SLR::isV(string token)
{
	if (find(V.begin(), V.end(), token) != V.end())
		return true;
	return false;
}

//�ж�temp�ڲ��ڼ���Clos��
bool SLR::isInC(Item temp, set<Item> Clos) {
	for (Item i : Clos){
		if (i.getItem() == temp.getItem())
			return true;
	}
	return false;
}

//�ж��Ƿ�Ӧ�ù�Լ idΪ��ǰ״̬���
string SLR::tableReduce(int id)
{
	for (State s : States){
		//Ŀ��״̬
		if (s.id == id){
			//������Ŀ��
			for (Item i : s.items){
				//������һ���㣬�϶����ǹ�Լ��Ŀ
				if (i.hasNextItem())
					return "";
				//�ǹ�Լ��Ŀ
				else
					return i.getLeft();//������NT
			}
		}
	}
	return "";
}

//�ҵ�item��Լ���Ĳ���ʽ����������
int SLR::findReduce(int id)
{
	for (State s : States){
		if (s.id == id){
			for (Item i : s.items){
				string temp = i.getItem();
				temp.erase(temp.find("."));
				temp.pop_back();
				return numPro.find(temp)->second;
			}
		}
	}
	return -1;
}

//״̬���Ƿ��Ѿ�������״̬
int SLR::hasState(set<Item> Items)
{
	for (State s : States){
		if (s.items.size() != Items.size())
			continue;
		if (s.items == Items)
			return s.id;
		else
			continue;
	}
	return -1;
}

//�ҵ�����ʽ���Ϊpro�Ĳ���ʽ�Ҳ�����
int SLR::rightCount(string& left, int pro){
	for (auto it = numPro.begin(); it != numPro.end(); it++){
		if (it->second == pro){
			cout << it->first << endl;
			string target = it->first;
			left = target.substr(0, target.find("->"));
			left.erase(left.end() - 1);
			string right = target.substr(target.find("->") + 3);
			vector<string>temp = split(right, " ");
			return temp.size();
		}
	}
	return 0;
}

// ���캯���������ķ�
SLR::SLR(string gramFilename) {
	readGrammer(gramFilename);
	extension();
}

// �﷨����
void SLR::analysis(string program) {

	// ����FOLLOW��
	makeFollow();
	printVT();
	// ����״̬ת��ͼ
	//makeFA();
	make_dfa();
	// ����SLR(1)������
	makeSLRtable();

	cout << "���������" << endl;

	if (analysing(program))
		cout << endl << "*********************���봮���ڸ��ķ�*********************" << endl;
	else
		cout << endl << "********************���봮�����ڸ��ķ�********************" << endl;;
}

//�﷨��������
bool SLR::analysing(string tokens) {
	stack<string> Analysis;
	tokens += " $";

	//0״̬��״̬ջ
	Analysis.push("$");
	Analysis.push("0");

	vector<string>word = split(tokens, " ");//�����봮�ֳ�һ������
	int pointer = 0;//���봮��ָ��

	while (true) {
		pair<int, string> title(stoi(Analysis.top()), word[pointer]);
		string res = actionTable[title];
		cout << "ջ����״̬" << setw(10) << Analysis.top() << "��ǰ�����ַ���" << setw(8) << word[pointer];
		
		// shift
		if (res[0] == 's') {
			cout << "������shift" << endl;
			int state = stoi(res.substr(1));
			Analysis.push(word[pointer]);
			Analysis.push(to_string(state));
			pointer++;
		}
		// reduce
		else if (res[0] == 'r') {
			cout << "������reduce ";
			int pro = stoi(res.substr(1));
			string left;//����ʽ��
			int b = 2 * rightCount(left, pro);//2���Ĳ���ʽ�Ҳ���������

			while (b > 0){
				Analysis.pop();
				b--;
			}

			int s1 = stoi(Analysis.top());
			Analysis.push(left);

			pair<int, string>t(s1, left);
			Analysis.push(to_string(gotoTable[t]));
		}
		else if (res[0] == 'A') {
			cout << "����������" << endl;
			return 1;
		}
		else {
			cout << "�ķ�����" << endl;
			return 0;
		}
	}
}

// ����SLR(1)������
void SLR::makeSLRtable() {
	addNumProduct();
	string s = S;
	s = s.erase(s.find("'"));
	// int Ϊ״̬��� string Ϊ����
	pair<int, string> title(1, "$");
	actionTable[title] = "ACC";

	for (Edge edge : Edges) {
		// Ŀ�ĵ���V
		if (isV(edge.path)) {
			pair<int, string> title(edge.source, edge.path);
			gotoTable[title] = edge.target;
		}
		//����action��
		else {
			//shift
			pair<int, string> title(edge.source, edge.path);
			actionTable[title] = "s" + to_string(edge.target);
		}
		//reduce
		//�ж��Ƿ�Ӧ�ù�Լ
		string rV = tableReduce(edge.target);
		if (rV != "") {
			if (edge.path != s) {
				vector<string> x = T;
				x.push_back("$");
				set<string>follow = FOLLOW[rV];
				for (string p : x) {
					if (follow.find(p) != follow.end()) {
						pair<int, string> title(edge.target, p);
						actionTable[title] = "r" + to_string(findReduce(edge.target));
					}
				}
			}
		}
	}
	printTable();
}

// ����״̬ת��ͼ
/**/
set <Item> SLR::GO(set <Item> Items, string op)
{
	set <Item> res;
	res.clear();
	set <Item>::iterator it;
	for (it = Items.begin(); it != Items.end(); it++)
	{
		Item cur = *it;
		if (cur.hasNextItem())
		{
			Item newitem = Item(cur.getLeft(), cur.getNextItemRight());
			if (cur.getPath() == op)
			{
				set <Item> C = closure(newitem);
				set <Item>::iterator it1;
				for (it1 = C.begin(); it1 != C.end(); it1++)
				{
					if (res.find(*it1) == res.end())
						res.insert(*it1);
				}
			}
		}
	}
	return res;
}
void SLR::make_dfa()//����DFA
{
	State S0; //��ʼ��Ŀ��
	S0.id = number++;
	//��ʼ��Ŀ��
	string firstSet = *(production[S].begin());
	Item st(S, firstSet);
	S0.items = closure(st);
	States.insert(S0);
	set <State>::iterator it;
	bool change;
	while (true)
	{
		change = false;
		set <State> newStates;
		newStates.clear();
		for (it = States.begin(); it != States.end(); it++)
		{
			State cur = *it;
			int vsize = V.size();
			int tsize = T.size();
			for (int i = 0; i < vsize; i++)
			{
				set <Item> newI = GO(cur.items, V[i]);
				if (newI.size() == 0)
					continue;
				int index = hasState(newI);
				if (index == -1)
				{
					State newS;
					newS.id = number++;
					newS.items = newI;
					newStates.insert(newS);
					Edges.push_back(Edge(cur.id, V[i], newS.id));
					change = true;
				}
				else if (find(Edges.begin(), Edges.end(), Edge(cur.id, V[i], index))== Edges.end())
					Edges.push_back(Edge(cur.id, V[i], index)), change = true;

			}
			for (int i = 0; i < tsize; i++)
			{
				set <Item> newI = GO(cur.items, T[i]);
				if (newI.size() == 0)
					continue;
				int index = hasState(newI);
				if (index == -1)
				{
					State newS;
					newS.id = number++;
					newS.items = newI;
					newStates.insert(newS);
					Edges.push_back(Edge(cur.id, T[i], newS.id));
					change = true;
				}
				else if (find(Edges.begin(), Edges.end(), Edge(cur.id, T[i], index)) == Edges.end())
					Edges.push_back(Edge(cur.id, T[i], index)), change = true;
			}
		}
		States.insert(newStates.begin(), newStates.end());
		if (!change)
			break;
	}
	printS();
	printEdges();
}
void SLR::makeFA() {
	State s0;
	s0.id = number++;

	// ��ʼ��Ŀ��
	string firstRight = *(production[S].begin());
	Item start(S, firstRight);
	s0.items = closure(start);
	States.insert(s0);

	//����FA
	State temp;
	for (State s : States) {
		cout << "״̬:" << s.id << endl;
		printItems(s.items);
		map<string, int> Paths; //·��
		for (Item now : s.items) {
			if (now.hasNextItem()) {
				string path = now.getPath(); // path ��Ϊ ���ϵķ���
				Item nextItem(now.getLeft(), now.getNextItemRight());
				set<Item> nextClos = closure(nextItem);

				//��״̬�Ѿ�������·���ˣ����²����ıհ���ӵ�ԭ��Ŀ��״̬��
				int oldDes;
				if (Paths.find(path) != Paths.end()) {
					oldDes = Paths.find(path)->second;

					for (State dest : States) {
						if(dest.id == oldDes){
							dest.items.insert(nextClos.begin(), nextClos.end());
							nextClos = dest.items;

							//����״̬����״̬
							//set�������ظ����룬���ֻ��ɾ���ٲ�
							States.erase(dest);
							States.insert(dest);
							int tID = hasState(nextClos);
							if (tID != -1) {
								for (int i = 0; i < Edges.size(); i++) {
									if (Edges[i].target == oldDes) {
										Edges[i].target = tID;
									}
								}
							}
							//����״̬�������
						}
					}
				}

				int tID = hasState(nextClos);
				//�����Ŀ��״̬��״̬����û�г��ֹ����ͼ���״̬��
				if (tID == -1) {
					State t;
					t.id = number++;
					t.items = nextClos;
					States.insert(t);
					Paths.insert(pair<string, int>(path, t.id));
					Edges.push_back(Edge(s.id, path, t.id));
				}
				else {
					Paths.insert(pair<string, int>(path, tID));
					Edges.push_back(Edge(s.id, path, tID));
				}
			}
		}
	}
	//ɾ���ظ�Edge
	sort(Edges.begin(), Edges.end());
	Edges.erase(unique(Edges.begin(), Edges.end()), Edges.end());
	
	//�����ظ�״̬
	for (State s1 : States) {
		for (State s2 : States){
			//�����ظ�״̬��
			if (s2.id > s1.id && s1.items == s2.items){
				int erase_id = s2.id;
				States.erase(s2);
				//�ظ�״̬���������״̬���-1
				for (State s : States){
					if (s.id > erase_id){
						//ԭ���޸�set��
						State& newS = const_cast<State&>(*States.find(s));
						newS.id--;
					}
				}
				//״̬ת�ƺ���
				for (int i = 0; i < Edges.size(); i++){
					if (Edges[i].source == erase_id || Edges[i].target == erase_id)
						Edges.erase(find(Edges.begin(), Edges.end(), Edges[i]));
					if (Edges[i].source > erase_id)
						Edges[i].source--;
					if (Edges[i].target > erase_id)
						Edges[i].target--;
				}
			}
		}
	}
	printS();
	printEdges();
}

// closure����
set<Item> SLR::closure(Item item) {
	set<Item> Clos; 
	Clos.insert(item);

	queue<Item> que;
	que.push(item);

	while (!que.empty()) {
		Item now = que.front();
		que.pop();

		vector<string> buffer = split(now.getRight(), ".");
		if (buffer.size() > 1) {
			string first = firstWord(buffer[1].erase(0, 1));
			if (isV(first)) {
				for (auto it = production[first].begin(); it != production[first].end(); it++){
					Item temp(first, *it);

					if (!isInC(temp, Clos))
					{
						Clos.insert(temp);
						que.push(temp);
					}
				}
			}
		}
	}
	return Clos;
}

// ��չ�ķ�
void SLR::extension() {
	string newS = S;
	newS += "'";
	V.insert(V.begin(), newS);
	production[newS].push_back(S);
	S = newS;
}

// ����FIRST�� 
void SLR::makeFirst() {
	FIRST.clear();
	//�ս���Ż�@
	FIRST["@"].insert("@");
	for (string x : T){
		FIRST[x].insert(x);
	}
	//���ս��
	while (true) {
		bool flag = true;
		for (int i = 0; i < V.size(); i++) {
			string lef = V[i];
			//����lef��ÿ������ʽ
			for (int k = 0; k < production[lef].size(); k++) {
				string right = production[lef][k];
				vector<string> temp = split(right, " ");
				for (int j = 0; j < temp.size(); j++) {
					// �õ�����
					string token = temp[j];
					//������Ƿ��ս�� ��Ϊ �ս�� �� @(NULL)
					if (!isV(token)) {
						if (FIRST[lef].find(token) == FIRST[lef].end())
							flag = false;
						FIRST[lef].insert(token);
						break;
					} // �ս��
					else {
						set<string> tempSet = FIRST[token];
						if (tempSet.find("@") != tempSet.end()) {
							tempSet.erase("@");
						}
						for (auto it : tempSet) {
							if (FIRST[lef].find(it) == FIRST[lef].end()) {
								flag = false;
								break;
							}
						}
						FIRST[lef].insert(tempSet.begin(), tempSet.end());
						if (FIRST[token].find("@") == FIRST[token].end()) {
							break;
						}
						else if (j == temp.size() - 1) {
							FIRST[lef].insert("@");
						}
					}
				}
			}
		}
		if (flag == true)
			break;
	}
}

// ����������е�FIRST��
set<string> SLR::makeFirstSeries(vector<string> input) {
	set<string> result = FIRST[input[0]];
	result.erase("@");
	int i;
	for (i = 0; i < input.size()-1; i++) {
		set<string> tempSet = FIRST[input[i]];
		if (tempSet.find("@") != tempSet.end()) {
			set<string> tempNextSet = FIRST[input[i+1]];
			tempNextSet.erase("@");
			result.insert(tempNextSet.begin(), tempNextSet.end());
		}
		else {
			break;
		}
	}
	if (i == input.size() - 1 && FIRST[input[i]].find("@") != FIRST[input[i]].end()) {
		result.insert("@");
	}
	return result;
}

// ����FOLLOW��
void SLR::makeFollow() {
	makeFirst();

	FOLLOW[S].insert("$");

	while (true) {
		bool flag = true;
		for (int i = 0; i < V.size(); i++) {
			string lef = V[i];
			//����lef��ÿ������ʽ
			// A -> aBp �� A -> aB
			for (int k = 0; k < production[lef].size(); k++) {
				string right = production[lef][k];
				vector<string> temp = split(right, " ");
				for (int j = 0; j < temp.size(); j++) {
					// �õ�����
					string token = temp[j];
					if (isV(token)) {
						// �ж� ��ǰ B �Ƿ���ĩβ
						// A -> aBp
						if (j != temp.size() - 1) {
							//�õ���ǰ����B�ĺ����������� 
							vector<string> tokenNextS;
							for (int z = j + 1; z < temp.size(); z++) {
								tokenNextS.push_back(temp[z]);
							}
							//�õ������������е�FIRST��
							set<string> tempSet = makeFirstSeries(tokenNextS);
							bool existNull = false;
							if (tempSet.find("@") != tempSet.end()) {
								tempSet.erase("@");
								existNull = true;
							}
					
							for (auto it : tempSet) {
								if (FOLLOW[token].find(it) == FOLLOW[token].end()) {
									flag = false;
									break;
								}
							}
							FOLLOW[token].insert(tempSet.begin(), tempSet.end());
							
							if (existNull) {
								for (auto it : FOLLOW[lef]) {
									if (FOLLOW[token].find(it) == FOLLOW[token].end()) {
										flag = false;
										break;
									}
								}
								FOLLOW[token].insert(FOLLOW[lef].begin(), FOLLOW[lef].end());
							}
						}
						// A -> aB ��ǰ����B��ĩβ
						else {
							for (auto it : FOLLOW[lef]) {
								if (FOLLOW[token].find(it) == FOLLOW[token].end()) {
									flag = false;
									break;
								}
							}
							FOLLOW[token].insert(FOLLOW[lef].begin(), FOLLOW[lef].end());
						}
					}
				}
			}
		}
		if (flag == true)
			break;
	}
	printFOLLOW();
}

//**********************��ӡ****************************************

//��ӡNT��T
void SLR::printVT()
{
	cout << "���ս���ż��ϣ�" << endl;
	for (int i = 0; i < V.size(); i++){
		cout << V[i] << " ";
	}
	cout << endl;
	cout << "�ս���ż��ϣ�" << endl;
	for (int i = 0; i < T.size(); i++){
		cout << T[i] << " ";
	}
	cout << endl;
}

//��ӡFOLLOW��
void SLR::printFOLLOW()
{
	cout << "FOLLOW����Ϊ" << endl;
	cout.setf(ios::left);
	for (string non_terminal : V){
		cout << setw(20) << non_terminal;
		for (string follow : FOLLOW[non_terminal])
			cout << follow << " ";
		cout << endl;
	}
	cout << endl;
}

//��ӡ������
void SLR::printTable()
{
	cout << "SLR(1)������" << endl;

	vector<string>x = T;//��������ս���ż���
	x.push_back("$");

	//���������
	cout << "****************action****************" << endl;
	cout.setf(ios::left);
	for (auto it1 = x.begin(); it1 != x.end(); it1++){
		if (it1 == x.begin())
			cout << setw(10) << " ";
		cout << setw(8) << *it1;
	}
	cout << endl;

	for (int i = 0; i < States.size(); i++){
		cout << setw(10) << i;

		for (string t : x){
			//cout<<i<<"ttt"<<endl;

			if (!actionTable.empty()){
				pair<int, string>title(i, t);
				cout << setw(8) << actionTable[title];
			}
			else
				cout << setw(8);
		}
		cout << endl;
	}
	cout << endl;

	/*��ӡGOTO��*/
	vector<string>y = V;//����S���ķ��ս���ż���
	y.erase(y.begin());

	cout << "****************goto******************" << endl;
	cout.setf(ios::left);

	for (auto it1 = y.begin(); it1 != y.end(); it1++){
		if (it1 == y.begin())
			cout << setw(10) << "";

		cout << setw(8) << *it1;
	}
	cout << endl;

	for (int i = 0; i < States.size(); i++){
		cout << setw(10) << i;

		for (string t : y){
			pair<int, string>title(i, t);

			if (gotoTable[title] != 0){
				cout << setw(8) << gotoTable[title];
			}
			else
				cout << setw(8) << "";
		}
		cout << endl;
	}
	cout << endl << "LR�����������" << endl << endl;
}

//��ӡ����ʽ
void SLR::printP()
{
	cout << "�﷨�Ĳ���ʽΪ" << endl;
	for (string left : V){
		cout << left << " -> ";
		for (auto it = production[left].begin(); it != production[left].end(); it++){
			if (it != production[left].end() - 1)
				cout << *it << " | ";
			else
				cout << *it << endl;
		}
	}
	cout << endl;
}

//��ӡ״̬��
void SLR::printS()
{
	cout << "**********״̬����Ϊ**********" << endl;
	for (State s : States){
		cout << "״̬��ţ�" << s.id << endl;
		printItems(s.items);
	}
	cout << endl;
}

//��ӡ״̬ת�ƺ���
void SLR::printEdges()
{
	cout << "**********״̬ת�ƺ���Ϊ**********" << endl;
	for (Edge go : Edges){
		cout << go.source << "---" << go.path << "-->" << go.target << endl;
	}
	cout << endl;
}

//��ӡ��Ŀ��
void SLR::printItems(set<Item>I)
{
	cout << "SLR(1)����Ŀ��Ϊ" << endl;
	for (Item i : I){
		cout << i.getItem() << endl;
	}
	cout << endl;
}

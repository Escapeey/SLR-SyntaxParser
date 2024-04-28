#include "SLR.h"
using namespace std;

Edge::Edge(int s, string p, int t) {
	source = s;
	path = p;
	target = t;
}

// 一些操作符的重载
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
		cout << gramFilename << " 读取失败!!! " << endl;
	}
	//按行读取文法 并 分解为产生式
	string line;
	while (getline(input, line)) {
		// 读取左部
		string left = line.substr(0, line.find_first_of('-'));
		left.erase(left.end()-1);
		// 左部加入非终结符号集
		V.push_back(left);

		// 读取右部
		string right = line.substr(line.find_first_of('-') + 3);
		addProduct(left, right);
	}
	// 完成终结符集
	addTerminal();
	S = *V.begin();
	input.close();
}

// 完成终结符集
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
	//终结符去重
	sort(T.begin(), T.end());
	T.erase(unique(T.begin(), T.end()), T.end());
}

//添加产生式
void SLR::addProduct(string left, string right) {
	right += " #"; // 作为每句文法结尾标志
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

//带标号的产生式集
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

//判断temp在不在集合Clos中
bool SLR::isInC(Item temp, set<Item> Clos) {
	for (Item i : Clos){
		if (i.getItem() == temp.getItem())
			return true;
	}
	return false;
}

//判断是否应该规约 id为当前状态编号
string SLR::tableReduce(int id)
{
	for (State s : States){
		//目标状态
		if (s.id == id){
			//遍历项目集
			for (Item i : s.items){
				//还有下一个点，肯定不是规约项目
				if (i.hasNextItem())
					return "";
				//是规约项目
				else
					return i.getLeft();//返回左部NT
			}
		}
	}
	return "";
}

//找到item规约到的产生式，返回其编号
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

//状态集是否已经包含该状态
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

//找到产生式序号为pro的产生式右部数量
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

// 构造函数，读入文法
SLR::SLR(string gramFilename) {
	readGrammer(gramFilename);
	extension();
}

// 语法分析
void SLR::analysis(string program) {

	// 构建FOLLOW集
	makeFollow();
	printVT();
	// 构造状态转换图
	//makeFA();
	make_dfa();
	// 构造SLR(1)分析表
	makeSLRtable();

	cout << "分析结果：" << endl;

	if (analysing(program))
		cout << endl << "*********************输入串属于该文法*********************" << endl;
	else
		cout << endl << "********************输入串不属于该文法********************" << endl;;
}

//语法分析过程
bool SLR::analysing(string tokens) {
	stack<string> Analysis;
	tokens += " $";

	//0状态入状态栈
	Analysis.push("$");
	Analysis.push("0");

	vector<string>word = split(tokens, " ");//将输入串分成一个个词
	int pointer = 0;//输入串的指针

	while (true) {
		pair<int, string> title(stoi(Analysis.top()), word[pointer]);
		string res = actionTable[title];
		cout << "栈顶：状态" << setw(10) << Analysis.top() << "当前输入字符：" << setw(8) << word[pointer];
		
		// shift
		if (res[0] == 's') {
			cout << "动作：shift" << endl;
			int state = stoi(res.substr(1));
			Analysis.push(word[pointer]);
			Analysis.push(to_string(state));
			pointer++;
		}
		// reduce
		else if (res[0] == 'r') {
			cout << "动作：reduce ";
			int pro = stoi(res.substr(1));
			string left;//产生式左部
			int b = 2 * rightCount(left, pro);//2倍的产生式右部符号数量

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
			cout << "动作：接受" << endl;
			return 1;
		}
		else {
			cout << "文法错误" << endl;
			return 0;
		}
	}
}

// 构造SLR(1)分析表
void SLR::makeSLRtable() {
	addNumProduct();
	string s = S;
	s = s.erase(s.find("'"));
	// int 为状态编号 string 为符号
	pair<int, string> title(1, "$");
	actionTable[title] = "ACC";

	for (Edge edge : Edges) {
		// 目的地是V
		if (isV(edge.path)) {
			pair<int, string> title(edge.source, edge.path);
			gotoTable[title] = edge.target;
		}
		//加入action表
		else {
			//shift
			pair<int, string> title(edge.source, edge.path);
			actionTable[title] = "s" + to_string(edge.target);
		}
		//reduce
		//判断是否应该规约
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

// 构造状态转换图
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
void SLR::make_dfa()//构造DFA
{
	State S0; //初始项目集
	S0.id = number++;
	//初始项目集
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

	// 初始项目集
	string firstRight = *(production[S].begin());
	Item start(S, firstRight);
	s0.items = closure(start);
	States.insert(s0);

	//构建FA
	State temp;
	for (State s : States) {
		cout << "状态:" << s.id << endl;
		printItems(s.items);
		map<string, int> Paths; //路径
		for (Item now : s.items) {
			if (now.hasNextItem()) {
				string path = now.getPath(); // path 即为 边上的符号
				Item nextItem(now.getLeft(), now.getNextItemRight());
				set<Item> nextClos = closure(nextItem);

				//该状态已经有这条路径了，则将新产生的闭包添加到原有目的状态中
				int oldDes;
				if (Paths.find(path) != Paths.end()) {
					oldDes = Paths.find(path)->second;

					for (State dest : States) {
						if(dest.id == oldDes){
							dest.items.insert(nextClos.begin(), nextClos.end());
							nextClos = dest.items;

							//更新状态集中状态
							//set不允许重复插入，因此只能删除再插
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
							//更新状态代码结束
						}
					}
				}

				int tID = hasState(nextClos);
				//如果该目的状态在状态集里没有出现过，就加入状态集
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
	//删除重复Edge
	sort(Edges.begin(), Edges.end());
	Edges.erase(unique(Edges.begin(), Edges.end()), Edges.end());
	
	//处理重复状态
	for (State s1 : States) {
		for (State s2 : States){
			//发现重复状态集
			if (s2.id > s1.id && s1.items == s2.items){
				int erase_id = s2.id;
				States.erase(s2);
				//重复状态后面的所有状态序号-1
				for (State s : States){
					if (s.id > erase_id){
						//原地修改set！
						State& newS = const_cast<State&>(*States.find(s));
						newS.id--;
					}
				}
				//状态转移函数
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

// closure函数
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

// 拓展文法
void SLR::extension() {
	string newS = S;
	newS += "'";
	V.insert(V.begin(), newS);
	production[newS].push_back(S);
	S = newS;
}

// 构建FIRST集 
void SLR::makeFirst() {
	FIRST.clear();
	//终结符号或@
	FIRST["@"].insert("@");
	for (string x : T){
		FIRST[x].insert(x);
	}
	//非终结符
	while (true) {
		bool flag = true;
		for (int i = 0; i < V.size(); i++) {
			string lef = V[i];
			//遍历lef的每个产生式
			for (int k = 0; k < production[lef].size(); k++) {
				string right = production[lef][k];
				vector<string> temp = split(right, " ");
				for (int j = 0; j < temp.size(); j++) {
					// 得到符号
					string token = temp[j];
					//如果不是非终结符 即为 终结符 或 @(NULL)
					if (!isV(token)) {
						if (FIRST[lef].find(token) == FIRST[lef].end())
							flag = false;
						FIRST[lef].insert(token);
						break;
					} // 终结符
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

// 构造符号序列的FIRST集
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

// 构建FOLLOW集
void SLR::makeFollow() {
	makeFirst();

	FOLLOW[S].insert("$");

	while (true) {
		bool flag = true;
		for (int i = 0; i < V.size(); i++) {
			string lef = V[i];
			//遍历lef的每个产生式
			// A -> aBp 或 A -> aB
			for (int k = 0; k < production[lef].size(); k++) {
				string right = production[lef][k];
				vector<string> temp = split(right, " ");
				for (int j = 0; j < temp.size(); j++) {
					// 得到符号
					string token = temp[j];
					if (isV(token)) {
						// 判断 当前 B 是否在末尾
						// A -> aBp
						if (j != temp.size() - 1) {
							//得到当前符号B的后续符号序列 
							vector<string> tokenNextS;
							for (int z = j + 1; z < temp.size(); z++) {
								tokenNextS.push_back(temp[z]);
							}
							//得到上述符号序列的FIRST集
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
						// A -> aB 当前符号B在末尾
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

//**********************打印****************************************

//打印NT和T
void SLR::printVT()
{
	cout << "非终结符号集合：" << endl;
	for (int i = 0; i < V.size(); i++){
		cout << V[i] << " ";
	}
	cout << endl;
	cout << "终结符号集合：" << endl;
	for (int i = 0; i < T.size(); i++){
		cout << T[i] << " ";
	}
	cout << endl;
}

//打印FOLLOW集
void SLR::printFOLLOW()
{
	cout << "FOLLOW集合为" << endl;
	cout.setf(ios::left);
	for (string non_terminal : V){
		cout << setw(20) << non_terminal;
		for (string follow : FOLLOW[non_terminal])
			cout << follow << " ";
		cout << endl;
	}
	cout << endl;
}

//打印分析表
void SLR::printTable()
{
	cout << "SLR(1)分析表：" << endl;

	vector<string>x = T;//含界符的终结符号集合
	x.push_back("$");

	//输出表格横轴
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

	/*打印GOTO表*/
	vector<string>y = V;//不含S’的非终结符号集合
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
	cout << endl << "LR分析表构建完成" << endl << endl;
}

//打印产生式
void SLR::printP()
{
	cout << "语法的产生式为" << endl;
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

//打印状态表
void SLR::printS()
{
	cout << "**********状态集合为**********" << endl;
	for (State s : States){
		cout << "状态编号：" << s.id << endl;
		printItems(s.items);
	}
	cout << endl;
}

//打印状态转移函数
void SLR::printEdges()
{
	cout << "**********状态转移函数为**********" << endl;
	for (Edge go : Edges){
		cout << go.source << "---" << go.path << "-->" << go.target << endl;
	}
	cout << endl;
}

//打印项目集
void SLR::printItems(set<Item>I)
{
	cout << "SLR(1)的项目集为" << endl;
	for (Item i : I){
		cout << i.getItem() << endl;
	}
	cout << endl;
}

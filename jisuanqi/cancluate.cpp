#include "moban.h"

using namespace std;

stack<char> opt;     //操作符栈
stack<double> val;   //操作数栈

//多项式求值所用结构
struct PolyNode {
	int coefficient;  // 系数
	int exponent;     // 指数
	PolyNode* next;   // 指向下一个节点
};
// 头指针
PolyNode* head = nullptr;


//求阶乘所用数组
int LargeNumberFactorial[1000] = { 0 };

//定义两个常量表示在数字中的状态
const int IN = 0;
const int OUT = 1;

char opt_set[10] = "+-*/()=.";

int level(char theOpt);									//操作符的优先级
bool in_set(char theChar);								//判断输入的操作符是否合法，仅包括()+-*/
bool del_space(string& theString);						//用于去除空格并检查是否有非法字符
string to_string(int theInt);							//把数字转换成字符串
bool change(string& from, string& to);					//将中缀表达式转换成后缀表达式
bool compute(string& theExp);							//计算后缀表达式
int Judge_rectangle(string& theStr);					//判断是否为三角函数或反三角函数并返回对应的编号
double cal_rect_number(string& theStr, int theInt);		//求三角函数里包含的数字并求出对应的结果
bool Judge_factorial(string& theStr);					//判断是否为求阶乘
int cal_factorial(int theInt);							//利用数组求阶乘
int cal_factorial_number(string& theStr);				// 求阶乘所求的数字
string generate_random_expression(int num_terms);
void quiz_mode();
void Matrix();											//矩阵运算
vector<vector<int>> inputMatrix(int& rows, int& cols, const string& name);//输入矩阵
bool isValidMatrix(const vector<vector<int>>& mat);		//验证矩阵有效性
vector<vector<int>> addMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B);//＋矩阵
vector<vector<int>> subtractMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B);//-矩阵
vector<vector<int>> multiplyMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B);//*矩阵
void insertTerm(int coef, int exp);//多项式求值函数
void printPolynomial();//多项式求值函数
void freePolynomial();// 释放链表内存

int main()
{
	system("color F3");
	cout << "欢迎使用计算器" << endl;
	cout << "选择模式: 1-普通计算 2-答题模式 3-退出\n";
	while (true) {
		int choice;
		cout << "请选择模式: 1-普通计算 2-答题模式 3-矩阵模式 4-计算三角函数 5-计算多项式 6-退出\n";
		cin >> choice;
		if (choice == 2) quiz_mode();
		//判断是否三角函数和反三角函数并求值
		else if (choice == 4) {
				//输入表达式	
				string init_exp;
				cout << "---请输入三角函数表达式:";
				cout << "   格式 cos(30) 30为弧度rad" << endl;
				cin >> init_exp;

				double res = 0;
				res = cal_rect_number(init_exp, Judge_rectangle(init_exp));
				if (res)
					cout << "结果为: " << fixed << setprecision(9) << res << endl;
		}
		//是加减乘除的情况
		else if(choice==1){
				//输入表达式	
				string init_exp;
				cout << "---请输入表达式:";
				cin >> init_exp;

				cin.clear();
				cin.sync();
				while (!opt.empty()) {
					opt.pop();
				}
				while (!val.empty()) {
					val.pop();
				}
				del_space(init_exp);//去除空格
				string cng_exp;
				cng_exp.clear();
				change(init_exp, cng_exp);	//转换为后缀表达式
				compute(cng_exp);//计算后缀表达式
				double stdans = val.top();
				cout << "结果为:" << (double)stdans << endl;
		}
		else if (choice == 3) {
			Matrix();
		}
		else if (choice == 5) {
			// 头指针
			PolyNode* head = nullptr;
			int coef, exp;

			cout << "请输入多项式的系数和指数（输入 0 0 结束）：" << endl;
			while (true) {
				cin >> coef >> exp;
				if (coef == 0 && exp == 0) break; // 结束输入
				insertTerm(coef, exp);
			}

			cout << "化简后的多项式：" << endl;
			printPolynomial();

			// 释放内存
			freePolynomial();
		}
		else {
			cout << "感谢您的使用..." << endl;
			return 0;
		}
	}
}
void Matrix() {
	cout << "矩阵运算选择" << endl;
	cout << "1.矩阵加法\t2.矩阵减法\t3.矩阵乘法\t4.退出" << endl;
	int signal;
	cin >> signal;
	if (!(signal == 1 || signal == 2 || signal == 3)) {
		return;
	}
	// 输入第一个矩阵
	int rowsA, colsA;
	auto matrixA = inputMatrix(rowsA, colsA, "Matrix A");

	cout << "如果为矩阵乘法，应当满足第一个矩阵的列数等于第二个矩阵的行数" << endl;
	// 输入第二个矩阵
	int rowsB, colsB;
	auto matrixB = inputMatrix(rowsB, colsB, "Matrix B");
	vector<vector<int>> mat;
	if (signal == 1) {
	 mat=addMatrix(matrixA, matrixB);
	}
	else if (signal == 2) {
	mat=subtractMatrix(matrixA, matrixB);
	}
	else if (signal == 3) {
	mat=multiplyMatrix(matrixA, matrixB);
	}
	else {
		exit(0);
	}
	cout << "运算完成后的矩阵为" << endl;
	for (int i = 0; i < mat.size(); i++) {
		for (int j = 0; j < mat[0].size(); j++) {
			cout << mat[i][j] << " ";
		}
		cout << endl;
	}

	Matrix();
}
#include <iostream>
#include <vector>
using namespace std;

// 输入单个矩阵
vector<vector<int>> inputMatrix(int& rows, int& cols, const string& name) {
	cout << "输入矩阵" << name << " 的行和列，空格分开: ";
	while (!(cin >> rows >> cols) || rows <= 0 || cols <= 0) {
		cin.clear();
		cin.ignore(1024, '\n');
		cout << "Invalid input! Re-enter dimensions (positive integers): ";
	}

	vector<vector<int>> mat(rows, vector<int>(cols));
	cout << "按照矩阵的 " << rows << "x" << cols << " 格式输入数据:\n";
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
		bool a111=scanf_s("%d", &mat[i][j]);
		if (!a111) {
			exit(0);
		}
		}
	}
	return mat;
}

// 验证矩阵有效性


/*为每一个操作符返回一个数，数代表了优先级*/
int level(char theOpt)
{
	switch (theOpt) {
	case '(':
		return 0;
	case '+':
	case '-':
		return 1;
	case '*':
	case '/':
		return 2;
	case ')':
		return 3;
	}
}

/*判断输入的操作符是否合法*/
bool in_set(char theChar) {
	for (int i = 0; i < 8; i++) {
		if (theChar == opt_set[i])
			return true;
	}
	return false;
}

/*用于去除空格并检查是否有非法字符*/
bool del_space(string& theString)
{
	string res;
	for (int i = 0; i < theString.length(); i++) {
		if (in_set(theString[i]) || isdigit(theString[i]))
			res += theString[i];
		else if (theString[i] == ' ') {}
		else {
			cout << "The enter is Wrong" << endl;
			return false;
		}
	}
	theString = res;
	return true;
}
/*把数字转换成字符串*/
string to_string(int theInt)
{
	if (theInt == 0)
		return string("0");
	bool neg = false;
	if (theInt < 0)
	{
		neg = true;
		theInt = -theInt;
	}
	string res;
	while (theInt != 0)
	{
		char c = (theInt % 10) + '0';
		res = c + res;
		theInt /= 10;
	}
	if (neg)
		res = '-' + res;
	return res;
}

/*将中缀表达式转换成后缀表达式*/
bool change(string& from, string& to)
{
	int theInt = 0;
	int state = OUT;
	char c;

	for (int i = 0; i < from.length(); i++)
	{
		c = from[i];
		if (isdigit(c)) {
			to = to + c;
			theInt *= 10;
			theInt += c - '0';
			state = IN; //状态改为在数字内
		}
		else {
			if (state == IN && c == '.') {
				to = to + '.';
				theInt = 0;
				continue;
			}
			if (state == IN && c != '.') {
				to += ' ';
				theInt = 0;
			}
			if (c == '=')
				break;
			else if (c == '(')
				opt.push(c);
			else if (c == ')') {
				while (!opt.empty() && opt.top() != '(') {
					to += opt.top();
					to += ' ';
					opt.pop();
				}
				opt.pop();
			}
			else {
				while (true) {
					if (opt.empty() || opt.top() == '(')
						opt.push(c);
					else if (level(c) > level(opt.top()))
						opt.push(c);
					else {
						to += opt.top();
						to += ' ';
						opt.pop();
						continue;
					}
					break;
				}
			}
			state = OUT; //状态为在数字外
		}
	}
	while (!opt.empty()) {
		to += opt.top();
		to += ' ';
		opt.pop();
	}
	return true;
}

/*计算后缀表达式*/
bool compute(string& theExp)
{
	int theInt = 0; //暂存数字
	int state = OUT;//初始状态为在数字外
	char c;
	bool dot = false;
	double count = 1.0;
	for (int i = 0; i < theExp.length(); i++)
	{
		c = theExp[i];
		if (isdigit(c) || c == '.') {
			if (isdigit(c)) {
				theInt *= 10;
				theInt += c - '0';
				state = IN; //状态为在数字内
				if (dot == true)
					count *= 10.0;
			}
			if (c == '.') {
				dot = true;
				continue;
			}
		}
		else {
			dot = false;
			double ans = theInt / count;
			count = 1.0;
			if (state == IN) {
				val.push(ans);
				theInt = 0;
			}
			double x, y;
			if (c != ' ') {
				x = val.top(); val.pop();
				y = val.top(); val.pop();

				switch (c) {
				case '+':val.push(x + y); break;
				case '-':val.push(y - x); break;
				case '*':val.push(x * y); break;
				case '/':val.push(y / x); break;
				default:cout << "未知的错误!" << endl;
				}
			}
			state = OUT;
		}
	}
	return true;
}

/*判断是否为三角函数或反三角函数并求出里面的数值*/
int Judge_rectangle(string& theStr)
{
	string str1 = "sin";
	string str2 = "cos";
	string str3 = "tan";
	string str4 = "arcsin";
	string str5 = "arccos";
	string str6 = "arctan";
	const char* show1, * show2, * show3, * show4, * show5, * show6;
	show4 = strstr(theStr.c_str(), str4.c_str());
	if (show4 != NULL)
		return 4;
	show5 = strstr(theStr.c_str(), str5.c_str());
	if (show5 != NULL)
		return 5;
	show6 = strstr(theStr.c_str(), str6.c_str());
	if (show6 != NULL)
		return 6;
	show1 = strstr(theStr.c_str(), str1.c_str());
	if (show1 != NULL)
		return 1;
	show2 = strstr(theStr.c_str(), str2.c_str());
	if (show2 != NULL)
		return 2;
	show3 = strstr(theStr.c_str(), str3.c_str());
	if (show3 != NULL)
		return 3;
	return 0;
}

/*求三角函数里包含的数字并求出对应的结果*/
double cal_rect_number(string& theStr, int theInt)
{
	int number = 0;
	for (int i = 0; theStr[i] != ')'; i++)
	{
		if (isdigit(theStr[i])) {
			number *= 10;
			number += theStr[i] - '0';
		}
	}
	double ans = 0.0;
	switch (theInt)
	{
	case 1:ans = sin(number); break;
	case 2:ans = cos(number); break;
	case 3:ans = tan(number); break;
	case 4:ans = asin(number); break;
	case 5:ans = acos(number); break;
	case 6:ans = atan(number); break;
	default:return 0;
		break;
	}
	return ans;
}

/*判断是否为求阶乘*/
bool Judge_factorial(string& theStr)
{
	int len = theStr.length();
	for (int i = 0; i < len; i++)
		if (theStr[i] == '!')
			return true;
	return false;
}

/*求阶乘所求的数字*/
int cal_factorial_number(string& theStr)
{
	int number = 0;
	for (int i = 0; theStr[i] != '!'; i++)
	{
		if (isdigit(theStr[i])) {
			number *= 10;
			number += theStr[i] - '0';
		}
	}
	return  number;
}

/*利用数组求阶乘*/
int cal_factorial(int theInt)
{
	LargeNumberFactorial[0] = 1;
	LargeNumberFactorial[1] = 1;
	int digit = 1;
	for (int i = 1; i <= theInt; i++)
	{
		for (int j = 1; j <= digit; j++)
		{
			LargeNumberFactorial[j] *= i;
		}
		for (int j = 1; j <= digit; j++)
		{
			if (LargeNumberFactorial[j] > 10)
			{
				for (int k = 1; k <= digit; k++)
				{
					if (LargeNumberFactorial[digit] > 9)
						digit++;
					LargeNumberFactorial[k + 1] += LargeNumberFactorial[k] / 10;
					LargeNumberFactorial[k] %= 10;
				}
			}
		}
	}
	return digit;
}


string generate_random_expression(int num_terms = 5) {
	string operators = "+-*/";
	string expression;
	ostringstream oss;
	int open_brackets = 0;

	for (int i = 0; i < num_terms; i++) {
		// 随机决定是否加左括号 (最多 2 层括号)
		if (rand() % 4 == 0 && open_brackets < 2 && i < num_terms - 1) {
			expression += "(";
			open_brackets++;
		}

		// 生成一个随机数 1~10
		oss.str("");  // 清空之前的内容
		oss << (rand() % 10 + 1);
		expression += oss.str();

		// 随机决定是否加右括号 (不能在第一个数后立即加)
		if (open_brackets > 0 && rand() % 4 == 0 && i > 0) {
			expression += ")";
			open_brackets--;
		}

		// 不是最后一个数时，添加运算符
		if (i < num_terms - 1) {
			expression += operators[rand() % 4];
		}
	}

	// 关闭未匹配的括号
	while (open_brackets > 0) {
		expression += ")";
		open_brackets--;
	}

	return expression;
}
// 进行答题模式
void quiz_mode() {
	vector<pair<string, double>> question_bank;  // 存储 (题目, 答案)

	while (true) {
		string question = generate_random_expression();
		cout << "计算: " << question << " = ? (输入 exit 退出)\n";
		string answer;
		cin >> answer;

		if (answer == "exit") break;  // 退出循环

		string postfix;
		change(question, postfix);
		compute(postfix);
		double correct_answer = val.top();

		stringstream ss(answer);
		double user_answer;
		ss >> user_answer;

		if (abs(user_answer - correct_answer) < 1e-6) {
			cout << "正确！\n";
		}
		else {
			cout << "错误，正确答案是: " << correct_answer << "\n";
		}

		// 记录到题库
		question_bank.push_back({ question, correct_answer });
	}

	// 退出循环后，保存题目和答案到文件
	ofstream file("题库.txt", ios::app);  // 以追加模式打开文件
	if (!file) {
		cerr << "文件打开失败！\n";
		return;
	}

	for (const auto& entry : question_bank) {
		file << entry.first << " = " << fixed << setprecision(6) << entry.second << "\n";
	}

	file.close();
	cout << "本次生成的题目已保存到 题库.txt\n";
}

// 矩阵加法
vector<vector<int>> addMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	int rows = A.size(), cols = A[0].size();
	vector<vector<int>> result(rows, vector<int>(cols));

	if (A.size() != B.size() ) {
		cerr << "Error: Matrix dimensions mismatch for addition!" << endl;
		return {};
	}

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result[i][j] = A[i][j] + B[i][j];
		}
	}
	return result;
}
// 矩阵减法
vector<vector<int>> subtractMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	int rows = A.size(), cols = A[0].size();
	vector<vector<int>> result(rows, vector<int>(cols));

	if (A.size() != B.size()) {
		cerr << "Error: Matrix dimensions mismatch for addition!" << endl;
		return {};
	}

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			result[i][j] = A[i][j] - B[i][j];
		}
	}
	return result;
}
// 矩阵乘法 
//矩阵乘法要求第一个矩阵的列数等于第二个矩阵的行数，结果矩阵的维度为 (A行数 × B列数)。
vector<vector<int>> multiplyMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B) {
	int rowsA = A.size(), colsA = A[0].size();
	int rowsB = B.size(), colsB = B[0].size();
	vector<vector<int>> result(rowsA, vector<int>(colsB,0));

	if (colsA != B.size()) {
		cerr << "Error: Matrix dimensions mismatch for multiplication!" << endl;
		return {};
	}

	for (int i = 0; i < rowsA; ++i) {
		for (int j = 0; j < colsB; ++j) {
			for (int k = 0; k < colsA; ++k) {
				result[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	return result;
}
void insertTerm(int coef, int exp) {
	if (coef == 0) return; // 忽略系数为 0 的项

	// 创建新节点
	PolyNode* newNode = new PolyNode{ coef, exp, nullptr };

	// 处理空链表情况
	if (!head || exp > head->exponent) {
		newNode->next = head;
		head = newNode;
		return;
	}

	// 在合适的位置插入
	PolyNode* prev = nullptr;
	PolyNode* curr = head;
	while (curr && curr->exponent > exp) {
		prev = curr;
		curr = curr->next;
	}

	// 合并同类项
	if (curr && curr->exponent == exp) {
		curr->coefficient += coef;
		if (curr->coefficient == 0) { // 如果系数变为 0，删除该项
			if (prev) prev->next = curr->next;
			else head = curr->next;
			delete curr;
		}
		delete newNode; // 新节点不需要了
	}
	else {
		// 插入新项
		newNode->next = curr;
		if (prev) prev->next = newNode;
	}
}

// 输出多项式
void printPolynomial() {
	if (!head) {
		cout << "0" << endl;
		return;
	}

	PolyNode* curr = head;
	bool first = true;

	while (curr) {
		// 处理正号
		if (!first && curr->coefficient > 0) cout << "+";

		// 输出系数
		if (curr->exponent == 0 || abs(curr->coefficient) != 1)
			cout << curr->coefficient;
		else if (curr->coefficient == -1)
			cout << "-"; // 省略 `-1x`

		// 输出指数部分
		if (curr->exponent > 0) {
			cout << "x";
			if (curr->exponent > 1) cout << "^" << curr->exponent;
		}

		curr = curr->next;
		first = false;
	}
	cout << endl;
}
// 释放链表内存
void freePolynomial() {
	while (head) {
		PolyNode* temp = head;
		head = head->next;
		delete temp;
	}
}
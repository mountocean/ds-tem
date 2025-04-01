#include "moban.h"

using namespace std;

stack<char> opt;     //������ջ
stack<double> val;   //������ջ

//����ʽ��ֵ���ýṹ
struct PolyNode {
	int coefficient;  // ϵ��
	int exponent;     // ָ��
	PolyNode* next;   // ָ����һ���ڵ�
};
// ͷָ��
PolyNode* head = nullptr;


//��׳���������
int LargeNumberFactorial[1000] = { 0 };

//��������������ʾ�������е�״̬
const int IN = 0;
const int OUT = 1;

char opt_set[10] = "+-*/()=.";

int level(char theOpt);									//�����������ȼ�
bool in_set(char theChar);								//�ж�����Ĳ������Ƿ�Ϸ���������()+-*/
bool del_space(string& theString);						//����ȥ���ո񲢼���Ƿ��зǷ��ַ�
string to_string(int theInt);							//������ת�����ַ���
bool change(string& from, string& to);					//����׺���ʽת���ɺ�׺���ʽ
bool compute(string& theExp);							//�����׺���ʽ
int Judge_rectangle(string& theStr);					//�ж��Ƿ�Ϊ���Ǻ��������Ǻ��������ض�Ӧ�ı��
double cal_rect_number(string& theStr, int theInt);		//�����Ǻ�������������ֲ������Ӧ�Ľ��
bool Judge_factorial(string& theStr);					//�ж��Ƿ�Ϊ��׳�
int cal_factorial(int theInt);							//����������׳�
int cal_factorial_number(string& theStr);				// ��׳����������
string generate_random_expression(int num_terms);
void quiz_mode();
void Matrix();											//��������
vector<vector<int>> inputMatrix(int& rows, int& cols, const string& name);//�������
bool isValidMatrix(const vector<vector<int>>& mat);		//��֤������Ч��
vector<vector<int>> addMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B);//������
vector<vector<int>> subtractMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B);//-����
vector<vector<int>> multiplyMatrix(const vector<vector<int>>& A, const vector<vector<int>>& B);//*����
void insertTerm(int coef, int exp);//����ʽ��ֵ����
void printPolynomial();//����ʽ��ֵ����
void freePolynomial();// �ͷ������ڴ�

int main()
{
	system("color F3");
	cout << "��ӭʹ�ü�����" << endl;
	cout << "ѡ��ģʽ: 1-��ͨ���� 2-����ģʽ 3-�˳�\n";
	while (true) {
		int choice;
		cout << "��ѡ��ģʽ: 1-��ͨ���� 2-����ģʽ 3-����ģʽ 4-�������Ǻ��� 5-�������ʽ 6-�˳�\n";
		cin >> choice;
		if (choice == 2) quiz_mode();
		//�ж��Ƿ����Ǻ����ͷ����Ǻ�������ֵ
		else if (choice == 4) {
				//������ʽ	
				string init_exp;
				cout << "---���������Ǻ������ʽ:";
				cout << "   ��ʽ cos(30) 30Ϊ����rad" << endl;
				cin >> init_exp;

				double res = 0;
				res = cal_rect_number(init_exp, Judge_rectangle(init_exp));
				if (res)
					cout << "���Ϊ: " << fixed << setprecision(9) << res << endl;
		}
		//�ǼӼ��˳������
		else if(choice==1){
				//������ʽ	
				string init_exp;
				cout << "---��������ʽ:";
				cin >> init_exp;

				cin.clear();
				cin.sync();
				while (!opt.empty()) {
					opt.pop();
				}
				while (!val.empty()) {
					val.pop();
				}
				del_space(init_exp);//ȥ���ո�
				string cng_exp;
				cng_exp.clear();
				change(init_exp, cng_exp);	//ת��Ϊ��׺���ʽ
				compute(cng_exp);//�����׺���ʽ
				double stdans = val.top();
				cout << "���Ϊ:" << (double)stdans << endl;
		}
		else if (choice == 3) {
			Matrix();
		}
		else if (choice == 5) {
			// ͷָ��
			PolyNode* head = nullptr;
			int coef, exp;

			cout << "���������ʽ��ϵ����ָ�������� 0 0 ��������" << endl;
			while (true) {
				cin >> coef >> exp;
				if (coef == 0 && exp == 0) break; // ��������
				insertTerm(coef, exp);
			}

			cout << "�����Ķ���ʽ��" << endl;
			printPolynomial();

			// �ͷ��ڴ�
			freePolynomial();
		}
		else {
			cout << "��л����ʹ��..." << endl;
			return 0;
		}
	}
}
void Matrix() {
	cout << "��������ѡ��" << endl;
	cout << "1.����ӷ�\t2.�������\t3.����˷�\t4.�˳�" << endl;
	int signal;
	cin >> signal;
	if (!(signal == 1 || signal == 2 || signal == 3)) {
		return;
	}
	// �����һ������
	int rowsA, colsA;
	auto matrixA = inputMatrix(rowsA, colsA, "Matrix A");

	cout << "���Ϊ����˷���Ӧ�������һ��������������ڵڶ������������" << endl;
	// ����ڶ�������
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
	cout << "������ɺ�ľ���Ϊ" << endl;
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

// ���뵥������
vector<vector<int>> inputMatrix(int& rows, int& cols, const string& name) {
	cout << "�������" << name << " ���к��У��ո�ֿ�: ";
	while (!(cin >> rows >> cols) || rows <= 0 || cols <= 0) {
		cin.clear();
		cin.ignore(1024, '\n');
		cout << "Invalid input! Re-enter dimensions (positive integers): ";
	}

	vector<vector<int>> mat(rows, vector<int>(cols));
	cout << "���վ���� " << rows << "x" << cols << " ��ʽ��������:\n";
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

// ��֤������Ч��


/*Ϊÿһ������������һ�����������������ȼ�*/
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

/*�ж�����Ĳ������Ƿ�Ϸ�*/
bool in_set(char theChar) {
	for (int i = 0; i < 8; i++) {
		if (theChar == opt_set[i])
			return true;
	}
	return false;
}

/*����ȥ���ո񲢼���Ƿ��зǷ��ַ�*/
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
/*������ת�����ַ���*/
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

/*����׺���ʽת���ɺ�׺���ʽ*/
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
			state = IN; //״̬��Ϊ��������
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
			state = OUT; //״̬Ϊ��������
		}
	}
	while (!opt.empty()) {
		to += opt.top();
		to += ' ';
		opt.pop();
	}
	return true;
}

/*�����׺���ʽ*/
bool compute(string& theExp)
{
	int theInt = 0; //�ݴ�����
	int state = OUT;//��ʼ״̬Ϊ��������
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
				state = IN; //״̬Ϊ��������
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
				default:cout << "δ֪�Ĵ���!" << endl;
				}
			}
			state = OUT;
		}
	}
	return true;
}

/*�ж��Ƿ�Ϊ���Ǻ��������Ǻ���������������ֵ*/
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

/*�����Ǻ�������������ֲ������Ӧ�Ľ��*/
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

/*�ж��Ƿ�Ϊ��׳�*/
bool Judge_factorial(string& theStr)
{
	int len = theStr.length();
	for (int i = 0; i < len; i++)
		if (theStr[i] == '!')
			return true;
	return false;
}

/*��׳����������*/
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

/*����������׳�*/
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
		// ��������Ƿ�������� (��� 2 ������)
		if (rand() % 4 == 0 && open_brackets < 2 && i < num_terms - 1) {
			expression += "(";
			open_brackets++;
		}

		// ����һ������� 1~10
		oss.str("");  // ���֮ǰ������
		oss << (rand() % 10 + 1);
		expression += oss.str();

		// ��������Ƿ�������� (�����ڵ�һ������������)
		if (open_brackets > 0 && rand() % 4 == 0 && i > 0) {
			expression += ")";
			open_brackets--;
		}

		// �������һ����ʱ����������
		if (i < num_terms - 1) {
			expression += operators[rand() % 4];
		}
	}

	// �ر�δƥ�������
	while (open_brackets > 0) {
		expression += ")";
		open_brackets--;
	}

	return expression;
}
// ���д���ģʽ
void quiz_mode() {
	vector<pair<string, double>> question_bank;  // �洢 (��Ŀ, ��)

	while (true) {
		string question = generate_random_expression();
		cout << "����: " << question << " = ? (���� exit �˳�)\n";
		string answer;
		cin >> answer;

		if (answer == "exit") break;  // �˳�ѭ��

		string postfix;
		change(question, postfix);
		compute(postfix);
		double correct_answer = val.top();

		stringstream ss(answer);
		double user_answer;
		ss >> user_answer;

		if (abs(user_answer - correct_answer) < 1e-6) {
			cout << "��ȷ��\n";
		}
		else {
			cout << "������ȷ����: " << correct_answer << "\n";
		}

		// ��¼�����
		question_bank.push_back({ question, correct_answer });
	}

	// �˳�ѭ���󣬱�����Ŀ�ʹ𰸵��ļ�
	ofstream file("���.txt", ios::app);  // ��׷��ģʽ���ļ�
	if (!file) {
		cerr << "�ļ���ʧ�ܣ�\n";
		return;
	}

	for (const auto& entry : question_bank) {
		file << entry.first << " = " << fixed << setprecision(6) << entry.second << "\n";
	}

	file.close();
	cout << "�������ɵ���Ŀ�ѱ��浽 ���.txt\n";
}

// ����ӷ�
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
// �������
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
// ����˷� 
//����˷�Ҫ���һ��������������ڵڶ����������������������ά��Ϊ (A���� �� B����)��
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
	if (coef == 0) return; // ����ϵ��Ϊ 0 ����

	// �����½ڵ�
	PolyNode* newNode = new PolyNode{ coef, exp, nullptr };

	// ������������
	if (!head || exp > head->exponent) {
		newNode->next = head;
		head = newNode;
		return;
	}

	// �ں��ʵ�λ�ò���
	PolyNode* prev = nullptr;
	PolyNode* curr = head;
	while (curr && curr->exponent > exp) {
		prev = curr;
		curr = curr->next;
	}

	// �ϲ�ͬ����
	if (curr && curr->exponent == exp) {
		curr->coefficient += coef;
		if (curr->coefficient == 0) { // ���ϵ����Ϊ 0��ɾ������
			if (prev) prev->next = curr->next;
			else head = curr->next;
			delete curr;
		}
		delete newNode; // �½ڵ㲻��Ҫ��
	}
	else {
		// ��������
		newNode->next = curr;
		if (prev) prev->next = newNode;
	}
}

// �������ʽ
void printPolynomial() {
	if (!head) {
		cout << "0" << endl;
		return;
	}

	PolyNode* curr = head;
	bool first = true;

	while (curr) {
		// ��������
		if (!first && curr->coefficient > 0) cout << "+";

		// ���ϵ��
		if (curr->exponent == 0 || abs(curr->coefficient) != 1)
			cout << curr->coefficient;
		else if (curr->coefficient == -1)
			cout << "-"; // ʡ�� `-1x`

		// ���ָ������
		if (curr->exponent > 0) {
			cout << "x";
			if (curr->exponent > 1) cout << "^" << curr->exponent;
		}

		curr = curr->next;
		first = false;
	}
	cout << endl;
}
// �ͷ������ڴ�
void freePolynomial() {
	while (head) {
		PolyNode* temp = head;
		head = head->next;
		delete temp;
	}
}
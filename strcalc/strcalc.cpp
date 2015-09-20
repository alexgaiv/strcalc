#include <iostream>
#include <vector>
#include <stack>
using namespace std;

/*
	E -> S{+S} | S{-S}
	S -> F{*F} | F{/F}
	F -> N | (E) | -F | +F
	N -> 0|1|2...|9|0N|1N|...|9N
*/

class StrCalc
{
public:
	bool Parse(const char *s, double &result);
private:
	static char delims[];
	enum token_t { PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, NUM, NEGATE, END };
	enum token_state { INITIAL, DIGIT, DELIM };

	struct Token {
		token_t type;
		int value;

		Token(token_t type = PLUS, int value = 0)
			: type(type), value(value) { }
	};
	
	const char *data;
	int result;
	Token token;
	vector<Token> rpnResult;

	Token GetToken();
	void NextToken() { token = GetToken(); }
	double ComputeResult();

	void E();
	void S();
	void F();
};

char StrCalc::delims[] = { '+', '-', '*', '/', '(', ')' };

bool StrCalc::Parse(const char *s, double &result)
{
	data = s;
	rpnResult.clear();
	try {
		E();
		if (token.type != END) throw data;
		result = ComputeResult();
		return true;
	} catch(const char *) {
		return false;
	}
}

StrCalc::Token StrCalc::GetToken()
{
	token_state state = INITIAL;
	int n = 0;

	while (true)
	{
		char c = *data;
		switch (state)
		{
		case INITIAL:
			if (isspace(c))
				data++;
			else if (isdigit(c)) {
				n = c - '0';
				data++;
				state = DIGIT;
			}
			else if (c == 0) {
				return Token(END);
			}
			else state = DELIM;
			break;
		case DIGIT:
			if (isdigit(c)) {
				n = n*10 + c - '0';
				data++;
			} else return Token(NUM, n);
			break;
		case DELIM:
			for (int i = 0; i < sizeof(delims); i++) {
				if (c == delims[i]) {
					data++;
					return Token((token_t)i);
				}
			}
			throw data;
		}
	}
}

double StrCalc::ComputeResult()
{
	stack<double> nums;
	int s = rpnResult.size();
	for (int i = 0; i < s; i++) {
		Token &t = rpnResult[i];
		if (t.type == NUM) {
			nums.push((double)t.value);
		}
		else {
			double op1, op2;
			op2 = nums.top(); nums.pop();
			if (t.type != NEGATE) {
				op1 = nums.top(); nums.pop();
			}

			switch (t.type)
			{
			case PLUS:
				nums.push(op1 + op2);
				break;
			case MINUS:
				nums.push(op1 - op2);
				break;
			case MUL:
				nums.push(op1 * op2);
				break;
			case DIV:
				nums.push(op1 / op2);
				break;
			case NEGATE:
				nums.push(-op2);
			}
		}
	}
	return nums.top();
}

void StrCalc::E() {
	S();
	while (token.type == PLUS || token.type == MINUS) {
		token_t tt = token.type;
		S();
		rpnResult.push_back(Token(tt));
	}
}

void StrCalc::S() {
	F();
	while (token.type == MUL || token.type == DIV) {
		token_t tt = token.type;
		F();
		rpnResult.push_back(Token(tt));
	}
}

void StrCalc::F() {
	NextToken();
	if (token.type == NUM) {
		rpnResult.push_back(token);
		NextToken();
	}
	else if (token.type == LPAREN) {
		E();
		if (token.type != RPAREN) throw data;
		NextToken();
	} 
	else if (token.type == MINUS) {
		F();
		rpnResult.push_back(NEGATE);
	}
	else if (token.type == PLUS) {
		F();
	}
	else throw data;
}

int main()
{
	StrCalc calc;
	char input[256];
	double result = 0.0;

	cout << "Enter expression:\n";
	cin.getline(input, 256, '\n');

	if (calc.Parse(input, result)) {
		cout << result << endl;
	} else cout << "not valid\n";

	system("pause");
	return 0;
}
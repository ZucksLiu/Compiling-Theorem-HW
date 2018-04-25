// ConsoleApplication4.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>



using namespace std;

void B();
void R();
void H();
void T();
void S();
void Viewerror();

#define NTOKEN 1000

//定义词法单元值
enum TokenValue {
	TOK_LBRACKET,          //逗号
	TOK_RBRACKET,
	TOK_END$,
	TOK_INT,
	TOK_SUM,
	TOK_SUB,
	TOK_SUP,
	TOK_ID,
	TOK_NUM,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_BLANK

};

string TOKEN_VALUE_DESCRIPTION[] =
{
	"TOK_LBRACKET",          //逗号
	"TOK_RBRACKET",
	"TOK_END$",
	"TOK_INT",
	"TOK_SUM",
	"TOK_SUB",
	"TOK_SUP",
	"TOK_ID",
	"TOK_NUM",
	"TOK_LBRACE",
	"TOK_RBRACE",
	"TOK_BLANK"
};


//标签和标签的值
typedef struct _Token {
	int  type;
	string str;
	int index;
} Token;


//定义规约式
enum WenValue {
	S_$B$,
	B_TF,          //逗号
	F_B,
	F_e,
	T_int,
	T_sum,
	T_RH,
	H_sup,
	H_,
	Hsup,
	H_e,
	R_id,
	R_num,
	R_blank,
	R_Bre_B,
	Finish,
	error_match,
	Terror,
	Herror,
	Rerror

};

string WEN_VALUE_DESCRIPTION[] =
{
	"S->$B$",
	"B->TF",          //逗号
	"F->B",
	"F->episilon",
	"T->\\int{B}{B}{B}",
	"T->\\sum{B}{B}{B}",
	"T->RH",
	"H->_^{B}",
	"H->_{B}",
	"H->^{B}",
	"H->epsilon",
	"R->id",
	"R->num",
	"R->\\blank",
	"R->(B)",
	"Finish",
	"error_match",
	"Terror",
	"Herror",
	"Rerror"

};


//definition of stack

typedef struct {
	Token stack[NTOKEN];
	int stacksize;
	int top;

}Nsqstack;
//the reload of operator == is ok;

template<class Type>
void Ninitstack(Type &s) {
	int i = 0;
	for (i = 0; i < NTOKEN; i++) {
		s.stack[i].str = "";
		s.stack[i].type = -1;
	}
	s.stacksize = NTOKEN;
	s.top = -1;
	return;
}//end Ninitstack

template<class Type>
void Ndestroystack(Type &s) {
	s.stacksize = 0;
	s.top = -1;
	return;
}//end Ndestroystack

template<class Type,class Type1>
int  Ngettop(Type s,Type1& toke) {
	if (s.top == -1) return 0;
	toke = s.stack[s.top];
	return 1;
}//Ngettop

template<class Type,class Type1>
void Npush(Type &s, Type1 toke) {

	s.stack[++s.top] = toke;

	return;
}//end Npush

template<class Type, class Type1>
int Npop(Type &s, Type1 &toke) {
	if (s.top == -1) return 0;
	toke = s.stack[s.top];
	s.top--;
	return 1;
}//end Npop
template<class Type>
bool Nstackempty(Type s) {
	if (s.top == -1) return true;
	else return false;
}//end Nstackempty

 //end Initialization of stack


//definition of stack


//definition of queue
typedef struct{
	string error;
	int unit;
	int place;

} Error;


typedef struct {
	Error queue[NTOKEN];
	int queuesize;
	int top;
	int rear;

}Nsqueue;
//the reload of operator == is ok;

template<class Type>
void Ninitqueue(Type &q) {
	int i = 0;
	for (i = 0; i < NTOKEN; i++) {
		q.queue[i].unit = -1;
		q.queue[i].error = "";
		q.queue[i].place = -1;
	}
	q.queuesize = NTOKEN;
	q.top = -1;
	q.rear = 0;
	return;
}//end Ninitqueue

template<class Type>
void Ndestroyqueue(Type &q) {
	q.queuesize = 0;
	q.top = -1; 
	q.rear = 0;
	return;
}//end Ndestroyqueue

template<class Type,class Type1>
int  Nqgettop(Type q, Type1& toke) {
	if (s.top == -1) return 0;
	toke = q.queue[q.top];
	return 1;
}//Nqgettop

template<class Type, class Type1>
void Nqpush(Type &q, Type1 toke) {

	if ((q.rear + 1) % NTOKEN == q.top) {
		cout << "Queue is full！！" << endl;
		return;
	}
	else {
		q.queue[q.rear] = toke;
		q.rear = (q.rear + 1) % NTOKEN;
		if (q.top == -1) {
			q.top++;
		}
		return;
	}
	
	
}//end Nqpush

template<class Type,class Type1>
int Nqpop(Type &q, Type1 &toke) {
	if (q.top == -1) return 0;
	toke = q.queue[q.top];
	q.top = (q.top + 1) % NTOKEN;
	if (q.top == q.rear) {
		q.top = -1;
		q.rear = 0;
	}

	return 1;
}//end Nqpop

template<class Type>
bool Nqueueempty(Type q) {
	if (q.top == -1) return true;
	else return false;
}//end Nqueueempty

 //end Initialization of queue




Token token[NTOKEN];
int ptr = 0;
int lookahead = 0;
int err_flag = 0;
Nsqstack s;
Nsqueue q;

//输入参数为路径
int lexer(char *path) {
	fstream fin;
	char  ch;
	char buf[256], *p;
	int n = 0;
	int flag = 0;
	int index = 0;
	fin.open(path, ios::in);
	if (fin.fail()) {
		fprintf(stderr, "Cant' open the file... :(\n");
	}
	fin.get(ch);
	while (!fin.eof()) {
		if (ch <= ' ') {       // [\n\t ]
			fin.get(ch); continue;
		}                 
		p = buf;
		switch (ch)
		{
		case '{':
			token[n].type = TOK_LBRACE;
			token[n].index = index++;
			token[n++].str = '{';
			break;
		case '}':
			token[n].type = TOK_RBRACE;
			token[n].index = index++;
			token[n++].str = '}';
			break;
		case '(':
			token[n].type = TOK_LBRACKET;
			token[n].index = index++;
			token[n++].str = '(';
			break;
		case ')':
			token[n].type = TOK_RBRACKET;
			token[n].index = index++;
			token[n++].str = ')';
			break;
		case '$':
			token[n].type = TOK_END$;
			token[n].index = index++;
			token[n++].str = '$';
			break;
		case '_':
			token[n].type = TOK_SUB;
			token[n].index = index++;
			token[n++].str = '_';
			break;
		case '^':
			token[n].type = TOK_SUP;
			token[n].index = index++;
			token[n++].str = '^';
		default:
			break;
		}
		if (isalpha(ch)) {                        // [A-Za-z][A-Za-z0-9],以字母开头为真
			token[n].type = TOK_ID;               //是一个标识符
			token[n].index = index++;
			*p++ = ch;
			fin.get(ch);
			while (isalnum(ch)) { 
				*p++ = ch; 
				fin.get(ch);
			}  //是否为字母或数字
			fin.unget();
			ch = 0;
			
			*p++ = '\0';
			token[n++].str = buf;
		}
		if (isdigit(ch)) {                 // [0-9]+
			token[n].type = TOK_NUM;
			token[n].index = index++;
			*p++ = ch;
			fin.get(ch);
			while (isdigit(ch)) {
				*p++ = ch;
				fin.get(ch);
			}
			fin.unget();
			ch = 0;
			*p++ = '\0';
			token[n++].str = buf;
		}

		if (ch == '\\') {
			*p++ = ch;
			fin.get(ch);
			
			if (ch == 'i') {
				*p++ = ch;
				fin.get(ch);
				if (ch == 'n') {
					*p++ = ch;
					fin.get(ch);
					if (ch == 't') {
						*p++ = ch;
						*p++ = 0;

						token[n].type = TOK_INT;
						token[n].index = index++;
						token[n++].str = buf;

					}
				}
			}//end if \int
			if (ch == 's') {
				*p++ = ch;
				fin.get(ch);
				if (ch == 'u') {
					*p++ = ch;
					fin.get(ch);
					if (ch == 'm') {
						*p++ = ch;
						*p++ = 0;
						token[n].type = TOK_SUM;
						token[n].index = index++;
						token[n++].str = buf;
					}
				}
			}//end if  \sum
			if (ch == 'b') {
				*p++ = ch;
				fin.get(ch);
				if (ch == 'l') {
					*p++ = ch;
					fin.get(ch);
					if (ch == 'a') {
						*p++ = ch;
						fin.get(ch);
						if (ch == 'n') {
							*p++ = ch;
							fin.get(ch);
							if (ch == 'k') {
								*p++ = ch;
								*p++ = 0;
								token[n].type = TOK_BLANK;
								token[n].index = index++;
								token[n++].str = buf;

							}
						}
					}
				}
			}//end if \blank
			//DFA 未判断错误！！！！！************如\bkank

		}
		fin.get(ch);
	}
	fin.close();
	return n;
}


void View_wen(int num) {
	cout << "按照" << WEN_VALUE_DESCRIPTION[num] << endl;

}

void error(int number) {
	if (err_flag == 0) {
		err_flag = 1;
	}

	cout << "There is a 规约错" << endl;
	Error temp;
	temp.error = WEN_VALUE_DESCRIPTION[number];
	temp.place = ptr-1;
	temp.unit = number;
	Nqpush(q,temp);

}

int NextToken() {
	return token[ptr++].type;
}



void match(int TOK) {
	if (lookahead == TOK) {
		lookahead = NextToken();
	}
	else error(error_match);
}


bool is_in(int lookahead) {
	if (lookahead >= TOK_LBRACKET&&lookahead <= TOK_BLANK) {
		return true;
	}
	else return false;

}
void S() {
	lookahead = NextToken();
	View_wen(S_$B$);
	match(TOK_END$);
	B();
	match(TOK_END$);
	if (err_flag) {
		Viewerror();
	}
	else {
		cout << "规约完成！文法正确！" << endl;
	}
	
	
}

void B() {
	View_wen(B_TF);
	T();
	if (lookahead == TOK_ID || lookahead == TOK_NUM || lookahead == TOK_LBRACKET || lookahead == TOK_SUM || lookahead == TOK_INT
		|| lookahead == TOK_BLANK) {
		View_wen(F_B);
		B();

	}
	
		
	else if (lookahead == TOK_END$||lookahead==TOK_RBRACE||lookahead==TOK_RBRACKET){
		View_wen(F_e);
	}
	else error(B_TF);
}

void T() {
	if (lookahead == TOK_SUM) {
		View_wen(T_sum);
		match(TOK_SUM);
		match(TOK_LBRACE);
		B();
		match(TOK_RBRACE);
		match(TOK_LBRACE);
		B();
		match(TOK_RBRACE);
		match(TOK_LBRACE);
		B();
		match(TOK_RBRACE);
		}
	else if (lookahead == TOK_INT) {
		View_wen(T_int);
		match(TOK_INT);
		match(TOK_LBRACE);
		B();
		match(TOK_RBRACE);
		match(TOK_LBRACE);
		B();
		match(TOK_RBRACE);
		match(TOK_LBRACE);
		B();
		match(TOK_RBRACE);
		}
	else {
		bool flag = is_in(lookahead);
		if (flag) {
			View_wen(T_RH);
		}
		else error(Terror);
		R();
		H();
	}

}
void H() {
	if (lookahead == TOK_SUB) {

		match(TOK_SUB);
		if (lookahead == TOK_SUP) {
			View_wen(H_sup);
			match(TOK_SUP);
			match(TOK_LBRACE);
			B();
			match(TOK_RBRACE);
			match(TOK_LBRACE);
			B();
			match(TOK_RBRACE);
		}
		else if (lookahead == TOK_LBRACE) {
			View_wen(H_);
			match(TOK_LBRACE);
			B();
			match(TOK_RBRACE);
		}
		else error(Herror);
	}
	else if (lookahead == TOK_SUP) {
		match(TOK_SUP);
		
		View_wen(Hsup);
		
		match(TOK_LBRACE);
		B();
		match(TOK_RBRACE);
	}

	else {
		bool flag = is_in(lookahead);
		if (flag) {
			View_wen(H_e);
		}
		else error(error_match);
	}


}

void R() {
	if (lookahead == TOK_ID) {
		View_wen(R_id);
		match(TOK_ID);
	}
	else if (lookahead == TOK_BLANK) {
		View_wen(R_blank);
		match(TOK_BLANK);
	}
	else if (lookahead == TOK_NUM) {
		View_wen(R_num);
		match(TOK_NUM);
	}
	else if (lookahead == TOK_LBRACKET) {
		View_wen(R_Bre_B);
		match(TOK_LBRACKET);
		B();
		match(TOK_RBRACKET);
	}
	else error(Rerror);
}

void Viewerror() {
	Error temp;
	while (!Nqueueempty(q)) {
		Nqpop(q, temp);
		cout <<"The "<<temp.place<<" errpr is "<< temp.error << endl;
	}
}

int main(int argc, char* argv[]) {
	int n;
	char* input = "error02.txt";
	int nToken = lexer(input);
	for (n = 0; n < nToken; n++) {
		cout << TOKEN_VALUE_DESCRIPTION[token[n].type] << ": " << token[n].str << endl;
	}
	if (n != 0) {
		ptr = 0;
		S();
	}
	else {
		cout << "空文件！" << endl;
	}
	
	system("pause");
	return 1;
}

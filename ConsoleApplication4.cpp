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



#define NTOKEN 1000

typedef struct pos {
	int top;
	int left;
	int width;
	int depth;
}Pos;
//Definition of three attribute.

struct Size{
    int blank;
    int symbol;
	int num;
	int special;
};

int B(ofstream &ofn, Pos ps);
int R(ofstream &ofn, Pos ps);
int H(ofstream &ofn, Pos ps);
int T(ofstream &ofn, Pos ps);
void S(ofstream &ofn, Pos ps);
void Viewerror();


Pos Low(Pos ps) {
	Pos ps2;
	
	ps2.left = ps.left;
	ps2.top = ps.top + (ps.depth<3 ? 30 * ps.width / 50 * pow(0.85, ps.depth) : 3 * pow(0.5,ps.depth-3));
	ps2.width = int(ps.width*0.5);
	ps2.depth = ps.depth + 1;
	return ps2;
}

Pos High(Pos ps) {
	Pos ps2;
	ps2.left = ps.left;
	ps2.top = ps.top - (ps.depth<3?5 * ps.width / 50 * pow(0.55, ps.depth):2 * pow(0.5, ps.depth - 3));
	ps2.width = int(ps.width*0.5);
	ps2.depth=ps.depth + 1;
	return ps2;
}

Pos ST_High(Pos ps) {
	Pos ps2;
	ps2.left = ps.left;
	ps2.top = ps.top - 15 * ps.width / 50 * pow(0.5, ps.depth)-15*ps.width/50*(ps.depth>=2?pow(0.9,ps.depth-2):0);
	ps2.width = int(ps.width*0.5);
	ps2.depth = ps.depth + 1;
	return ps2;
}

Pos ST_Low(Pos ps) {
	Pos ps2;

	ps2.left = ps.left;
	ps2.top = ps.top + 35 * ps.width / 50 * pow(0.9, ps.depth) + 15 * ps.width / 50 * (ps.depth >= 2 ? pow(0.9, ps.depth - 2) : 0);
	ps2.width = int(ps.width*0.5);
	ps2.depth = ps.depth + 1;
	return ps2;
}

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
Size sz{ 25,25,20,70 };

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
void S(ofstream &ofn,Pos ps) {

	lookahead = NextToken();
	View_wen(S_$B$);
	const char *beg = "<html>\n<head>\n<META content=\"text/html; charset=gb2312\">\n</head>\n<body>\n";
	ofn << beg;
	match(TOK_END$);

	B(ofn,ps);
	match(TOK_END$);
	if (err_flag) {
		Viewerror();
	}
	else {
		cout << "规约完成！文法正确！" << endl;

	}
	const char *en = "</body>\n</html>";
	ofn << en;
	
}

int B(ofstream &ofn,Pos ps) {
	View_wen(B_TF);
	ps.left=T(ofn,ps);
	if (lookahead == TOK_ID || lookahead == TOK_NUM || lookahead == TOK_LBRACKET || lookahead == TOK_SUM || lookahead == TOK_INT
		|| lookahead == TOK_BLANK) {
		View_wen(F_B);
		ps.left= B(ofn, ps);
	}
	
		
	else if (lookahead == TOK_END$||lookahead==TOK_RBRACE||lookahead==TOK_RBRACKET){
		View_wen(F_e);
	}
	else error(B_TF);

	return ps.left;
}

int T(ofstream &ofn,Pos ps) {
	if (lookahead == TOK_SUM) {
		View_wen(T_sum);
		string s1 = "<div style=\"position: absolute; top:";
		ofn << s1 << ps.top*0.9 / (ps.depth <= 1 ? pow(0.94, ps.depth) : pow(0.94, 1)*0.96) << "px; left:" << ps.left << "px;\"><span style=\"font-size:" << (ps.width*1.6) * pow(0.8, ps.depth) << "; font-style:normal; lineheight:100%;\">∑</span></div>\n";
		ps.left = int(ps.left + sz.special*(double)ps.width / (50) * pow(0.8, ps.depth));
		match(TOK_SUM);
		match(TOK_LBRACE);
		int Subleft = B(ofn,ST_Low(ps));
		match(TOK_RBRACE);
		
		match(TOK_LBRACE);
		int Supleft = B(ofn,ST_High(ps));
		ps.left = Supleft > Subleft ? Supleft : Subleft;
		match(TOK_RBRACE);
		
		match(TOK_LBRACE);
		ps.left = B(ofn, ps);
		match(TOK_RBRACE);
		}
	else if (lookahead == TOK_INT) {
		View_wen(T_int);
		string s1 = "<div style=\"position: absolute; top:";
		ofn << s1 << ps.top*0.9 / (ps.depth<=1 ? pow(0.94,ps.depth):pow(0.94,1)*0.98)  << "px; left:" << ps.left << "px;\"><span style=\"font-size:" << (ps.width*1.6) * pow(1, ps.depth) << "; font-style:normal; lineheight:100%;\">"; 
		ofn << "∫";
		ofn << "</span></div>\n";
		ps.left = int(ps.left + sz.special*(double)ps.width / (50) * pow(1, ps.depth)) ;
		match(TOK_INT);
		match(TOK_LBRACE);
		int Subleft = B(ofn, ST_Low(ps));
		match(TOK_RBRACE);
		match(TOK_LBRACE);
		int Supleft = B(ofn, ST_High(ps));
		ps.left = Supleft > Subleft ? Supleft : Subleft;
		match(TOK_RBRACE);
		match(TOK_LBRACE);
		ps.left = B(ofn, ps);
		match(TOK_RBRACE);
		}
	else {
		bool flag = is_in(lookahead);
		if (flag) {
			View_wen(T_RH);
		}
		else error(Terror);

		ps.left=R(ofn,ps);
		ps.left=H(ofn,ps);
	}
	return ps.left;

}//end T

int H(ofstream &ofn,Pos ps) {
	if (lookahead == TOK_SUB) {

		match(TOK_SUB);
		if (lookahead == TOK_SUP) {
			View_wen(H_sup);
			match(TOK_SUP);
			match(TOK_LBRACE);
			int Subleft = B(ofn, Low(ps));
			match(TOK_RBRACE);
			match(TOK_LBRACE);
			int Supleft = B(ofn, High(ps));
			ps.left = Supleft > Subleft ? Supleft : Subleft;
			match(TOK_RBRACE);
		}
		else if (lookahead == TOK_LBRACE) {
			View_wen(H_);
			match(TOK_LBRACE);
			ps.left = B(ofn, Low(ps));
			match(TOK_RBRACE);
		}
		else error(Herror);
	}
	else if (lookahead == TOK_SUP) {
		match(TOK_SUP);
		
		View_wen(Hsup);
		
		match(TOK_LBRACE);
		ps.left = B(ofn, High(ps));
		match(TOK_RBRACE);
	}

	else {
		bool flag = is_in(lookahead);
		if (flag) {
			View_wen(H_e);
		}
		else error(error_match);
	}
	return ps.left;

}//end H

int R(ofstream &ofn,Pos ps) {
	if (lookahead == TOK_ID) {
		View_wen(R_id);
		string s1 = "<div style=\"position: absolute; top:";
		ofn << s1 << ps.top << "px; left:" << ps.left << "px;\"><span style=\"font-size:" << ps.width << "; font-style:oblique; lineheight:100%;\">";
		Token toke = token[ptr - 1];
		int length =toke.str.length();

		ofn << toke.str;
		ofn << "</span></div>\n";
		ps.left = int(ps.left + length*sz.symbol*(double)ps.width / (50) + 3 * pow(0.1, ps.depth)) + 1;
		match(TOK_ID);

	}
	else if (lookahead == TOK_BLANK) {
		View_wen(R_blank);
		string s1 = "<div style=\"position: absolute; top:";
		ofn << s1 << ps.top << "px; left:" << ps.left << "px;\"><span style = \"font-size:" << ps.width << "; font-style:normal; lineheight:100%;\">";
		ofn << " </span></div>\n";
		ps.left = int(ps.left + sz.blank*(double)ps.width / (50 ));
		match(TOK_BLANK);
	}
	else if (lookahead == TOK_NUM) {
		View_wen(R_num);

		string s1 = "<div style=\"position: absolute; top:";
		ofn << s1 << ps.top << "px; left:" << ps.left << "px;\"><span style=\"font-size:" << ps.width << "; font-style:normal; lineheight:100%;\">";
		Token toke = token[ptr - 1];
		int length = toke.str.length();
		ofn << toke.str;
		ofn << "</span></div>\n";
		double tempk = pow(0.8, ps.depth);
		ps.left = int(ps.left + length*sz.symbol*(double)ps.width / (50) + 3 * pow(0.5, ps.depth)) + 1;
		match(TOK_NUM);
	}
	else if (lookahead == TOK_LBRACKET) {
		View_wen(R_Bre_B);
		string s1 = "<div style=\"position: absolute; top:";
		ofn << s1 << ps.top << "px; left:" << ps.left << "px;\"><span style=\"font-size:" << ps.width << "; font-style:normal; lineheight:100%;\">(</span></div>\n";
		ps.left = int(ps.left + sz.symbol*(double)ps.width / 50 * pow(0.8, ps.depth));
		match(TOK_LBRACKET);
		ps.left=B(ofn,ps);
		ofn << s1 << ps.top << "px; left:" << 5*pow(0.8,ps.depth)+ps.left << "px;\"><span style=\"font-size:" << ps.width<< "; font-style:normal; lineheight:100%;\">)</span></div>\n";
		ps.left = int(ps.left + 5*pow(0.8,ps.depth)+sz.symbol*(double)ps.width / (50 * pow(0.8, ps.depth)));
		match(TOK_RBRACKET);
	}
	else error(Rerror);

	return ps.left;
}//end R

void Viewerror() {
	Error temp;
	while (!Nqueueempty(q)) {
		Nqpop(q, temp);
		cout <<"The "<<temp.place<<" errpr is "<< temp.error << endl;
	}
}

int main(int argc, char* argv[]) {
	int n;
	Pos ps{ 160,250,50,0 };
	char* input = "sample10.txt";
	int nToken = lexer(input);
	if (nToken == 0) {
		cout << "空文件！" << endl;
		return -1; 
	}
	for (n = 0; n < nToken; n++) {
		cout << TOKEN_VALUE_DESCRIPTION[token[n].type] << ": " << token[n].str << endl;
	}

	ptr = 0;
	ofstream ofn;
	const char *outfile= "out.html";
	ofn.open(outfile,ios::out);
	if (ofn.fail()) {
		cout << "打开html文件失败！" << endl;
		return -1;
	}
	S(ofn,ps);
	ofn.close();
	//rename("out.txt", "out.html");

	
	//system("pause");
	return 1;
}

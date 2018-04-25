#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>   //����exit
#include <io.h>    //���ļ�����չ��

char init[100]; //  ��Ŵ��ļ��ж�ȡ��ԭʼ�ַ���
int location=0; //locationָ��������ĵ�һ���ַ���init�ַ������е�λ��
struct Pos{
    int top;    //�������ľ���
    int left;   //����ߵľ���
    int wd;     //����Ļ�����С
};
struct Size{
    int blank;
    int num;
    int symbol;
    int special;
};
Size sz{25,25,25,60};
using namespace std;

Pos Low(Pos);
Pos High(Pos);
int B(fstream &,Pos);
int T(fstream &,Pos);
int R(fstream &,Pos);

//�����±�
Pos Low(Pos ps) {
    Pos ps2;
    ps2.left=ps.left;
    ps2.top=ps.top+30*ps.wd/50;
    ps2.wd=ps.wd*0.7;
    return ps2;
}

Pos High(Pos ps) {
    Pos ps2;
    ps2.left=ps.left;
    ps2.top=ps.top-15*ps.wd/50;
    ps2.wd=ps.wd*0.7;
    return ps2;
}
//ƥ��'$,(,),{,},_,^'
void match_1(char in) {
    if(in==init[location]) {location=location+1;cout<<in<<"\t";}    //�����ַ�ƥ��ɹ�,location��1
    else {
        cout<<"��λ��"<<location<<"�����ִ���!"<<endl;
        cout<<"�����﷨Ӧ��ƥ��: "<<in<<endl;
        cout<<"��ǰ�ַ�Ϊ: "<<init[location]<<endl;
        exit(0);
    }
}

//ƥ��id,num,\int,\sum,\blank;
int match_2(string in) {
    int length=0;   //�洢num����
    if(in=="num") { //������������֣������ڸ�num�Ǻŷ���
        while(init[location]>=48&&init[location]<=57) {
            cout<<init[location];
            location=location+1;
            length++;
        }
        cout<<"\t";
    }
    else if(in=="id") {//������������������ĸ�������ڸ�id�Ǻŷ���
        while((init[location]>=97&&init[location]<=122)||(init[location]>=65&&init[location]<=90)||(init[location]>=48&&init[location]<=57)) {
            cout<<init[location];
            location=location+1;
            length++;
        }
        cout<<"\t";
    }
    else if(in=="\\blank") {
        location=location+6;
        cout<<"\\blank"<<"\t";
    }
    else if(in=="\\int"||in=="\\sum"){
        location=location+4;
        cout<<"\\int"<<"\t";
    }
    else {
        cout<<"��λ��"<<location<<"�����ִ���!!!"<<endl;
        cout<<"�����﷨Ӧ��ƥ��: "<<in<<endl;
        cout<<"��ǰ�ַ�Ϊ: "<<init[location]<<endl;
        exit(1);
    }
    return length;
}

//�ж�init��������һ��������������
string LookAhead(char &lookahead) {
    lookahead=init[location];
    if(lookahead=='$'||lookahead=='('||lookahead==')'||lookahead=='{'||lookahead=='}'||lookahead=='_'||lookahead=='^') {   //���������ַ�����'$,(,),{,},_,^, ,\n��ֱ�ӷ���
        return "special_id";
    }
    else if((lookahead>=97&&lookahead<=122)||(lookahead>=65&&lookahead<=90))
        return "id";
    else if(lookahead>=48&&lookahead<=57)
        return "num";
    else if(lookahead=='\\') {
        if(init[location+1]=='i'&&init[location+2]=='n'&&init[location+3]=='t')
            return "\\int";
        else if(init[location+1]=='s'&&init[location+2]=='u'&&init[location+3]=='m')
            return "\\sum";
        else if(init[location+1]=='b'&&init[location+2]=='l'&&init[location+3]=='a'&&init[location+4]=='n'&&init[location+5]=='k')
            return "\\blank";
    }
    else {
        cout<<"��λ��"<<location<<"�����ִ���"<<endl;
        cout<<"�﷨�в����ַ�: "<<lookahead<<endl;
        exit(1);
    }
}

//��init�ַ������е��ַ����д�������html��ʽ���뵽ioFile��
void S(fstream &ioFile,Pos ps) {
    match_1('$');   //��$����ƥ��
    B(ioFile,ps);
    match_1('$');
}

//����ʽB->TB|T     ÿ����һ�ζ�Ҫ����Pos�ṹ��
int B(fstream &ioFile,Pos ps) {
    int T_left=T(ioFile,ps);
    ps.left=T_left;
    char lookahead=0;
    string result=LookAhead(lookahead);
    if(result=="id"||result=="num"||result=="\\int"||result=="\\sum"||result=="\\blank"||lookahead=='(') { //ͨ����FIRST(B)�ж�T���Ƿ���B
        int B_left=B(ioFile,ps);
        ps.left=B_left;
       }
    return ps.left;
}

//����ʽT->R_^{B}{B}|R^{B}|R_{B}|R
int T(fstream &ioFile,Pos ps) {
    int R_left=R(ioFile,ps);
    ps.left=R_left;
    char lookahead=0;
    string result=LookAhead(lookahead);
    if(lookahead=='_') {
        match_1('_');
        LookAhead(lookahead);
        if(lookahead=='^') {    //��Ӧ����ʽ T->R_^{B}{B}
            match_1('^');
            match_1('{');
            int B_left=B(ioFile,Low(ps));     //low(ps)���±�Ĵ���
            match_1('}');
            match_1('{');
            int B2_left=B(ioFile,High(ps));    //High(ps)���ϱ�Ĵ���
            match_1('}');
            if(B_left>B2_left) ps.left=B_left;
            else ps.left=B2_left;
        }
        else if(lookahead=='{') {   //��Ӧ����ʽT->R_{B}
            match_1('{');
            int B_left=B(ioFile,Low(ps));
            ps.left=B_left;
            match_1('}');
        }
    }
    else if(lookahead=='^') {   //�Բ���ʽT->R^{B}�Ĵ���
        match_1('^');
        match_1('{');
        int B_left=B(ioFile,High(ps));
        ps.left=B_left;
        match_1('}');
    }
    return ps.left;
}

//����ʽR->\int{B}{B}{B}|\sum{B}{B}{B}|id|num|\blank|(B)
int R(fstream &ioFile,Pos ps) {
    char lookahead=0;
    string result=LookAhead(lookahead);
    if(result=="\\blank") {    //�Բ���ʽR��>\blank�Ĵ������ļ�������ո�
        match_2("\\blank");
        string s1="<div style=\"position: absolute; top:";
        ioFile<<s1<<ps.top<<"px; left:"<<ps.left<<"px;\">&nbsp</div>\n";
        ps.left=ps.left+sz.blank*(double)ps.wd/50;

       }
    else if(lookahead=='(') {   //�Բ���ʽR->(B)�Ĵ������ļ��������������B�������
                match_1('(');
                string s1="<div style=\"position: absolute; top:";
                ioFile<<s1<<ps.top<<"px; left:"<<ps.left<<"px;\"><span style=\"font-size:"<<ps.wd<<"; font-style:normal; lineheight:100%;\">(</span></div>\n";
                ps.left=ps.left+sz.symbol*(double)ps.wd/50;

                int B_left=B(ioFile,ps);
                ps.left=B_left;
                match_1(')');
                ioFile<<s1<<ps.top<<"px; left:"<<ps.left<<"px;\"><span style=\"font-size:"<<ps.wd<<"; font-style:normal; lineheight:100%;\">)</span></div>\n";
                ps.left=ps.left+sz.symbol*(double)ps.wd/50;

            }
     else if(result=="\\int") { //�Բ���ʽ\int{B}{B}{B}���д���
        match_2("\\int");
        string s1="<div style=\"position: absolute; top:";
        ioFile<<s1<<ps.top*0.9<<"px; left:"<<ps.left<<"px;\"><span style=\"font-size:"<<(ps.wd*1.6)<<"; font-style:normal; lineheight:100%;\">��</span></div>\n";
        ps.left=ps.left+sz.special*(double)ps.wd/50;

        match_1('{');
        int B_left=B(ioFile,Low(ps));
        match_1('}');
        match_1('{');
        int B2_left=B(ioFile,High(ps));
        if(B_left>B2_left) ps.left=B_left;
        else ps.left=B2_left;
        match_1('}');
        match_1('{');
        int B3_left=B(ioFile,ps);
        ps.left=B3_left;
        match_1('}');
     }
     else if(result=="\\sum") { //�Բ���ʽ\sum{B}{B}{B}���д���
        match_2("\\sum");
        string s1="<div style=\"position: absolute; top:";
        ioFile<<s1<<ps.top*0.9<<"px; left:"<<ps.left<<"px;\"><span style=\"font-size:"<<(ps.wd*1.6)<<"; font-style:normal; lineheight:100%;\">��</span></div>\n";
        ps.left=ps.left+sz.special*(double)ps.wd/50;

        match_1('{');
        int B_left=B(ioFile,Low(ps));
        match_1('}');
        match_1('{');
        int B2_left=B(ioFile,High(ps));
        if(B_left>B2_left) ps.left=B_left;
        else ps.left=B2_left;
        match_1('}');
        match_1('{');
        int B3_left=B(ioFile,ps);
        ps.left=B3_left;
        match_1('}');
     }
     else if(result=="id") {    //�Բ���ʽR->id���д���
        int length=match_2("id");
        int start=location-length;
        string s1="<div style=\"position: absolute; top:";
        ioFile<<s1<<ps.top<<"px; left:"<<ps.left<<"px;\"><span style=\"font-size:"<<ps.wd<<"; font-style:oblique; lineheight:100%;\">";
        for(;start<location;start++){
            ioFile<<init[start];
        }
        ioFile<<"</span></div>\n";
        ps.left=ps.left+length*sz.symbol*(double)ps.wd/50;
     }
     else if(result=="num") {   //�Բ���ʽR->num���д���
        int length=match_2("num");
        int start=location-length;
        string s1="<div style=\"position: absolute; top:";
        ioFile<<s1<<ps.top<<"px; left:"<<ps.left<<"px;\"><span style=\"font-size:"<<ps.wd<<"; font-style:normal; lineheight:100%;\">";
        for(;start<location;start++){
            ioFile<<init[start];
        }
        ioFile<<"</span></div>\n";
        ps.left=ps.left+length*sz.symbol*(double)ps.wd/50;
     }
     else {
        cout<<"��λ��"<<location<<"�����ִ���"<<endl;
        cout<<"��ǰ�ַ�Ϊ: "<<init[location]<<endl;
        exit(1);
     }
     return ps.left;
}


//��������ַ������еĿո��뻻��
int Remov(int i) {
    char c[100];
    int num=0;
    for(int k=0;k<i;k++) {
        if(init[k]!=' '&&init[k]!='\n') {
            c[num]=init[k];
            num++;
        }
    }
    for(int s=0;s<100;s++) init[s]=0;
    for(int k=0;k<num;k++)
        init[k]=c[k];
    return num;
}

int main() {
    char c;
    const string fName1="test05.txt";   //�����ļ���
    fstream ioFile;     //��д�ļ�����

    //����ṹ�崫��λ�ò���
    Pos ps{160,500,50};

    //���ļ��ж�ȡ�ַ���������init�ַ�������
    ioFile.open(fName1,ios::in|ios::out);
    int i=0;
    while(ioFile.get(c)) {
        init[i]=c;
        i++;
    }
    ioFile.close();

    // ȥ���ַ������еĿո�ͻس�����
    Remov(i);

    //��out.txt�ļ������html��ʽ����Ĺ̶�����
    const string fName2="out.txt";
    ioFile.open(fName2,ios::trunc|ios::out);
    const char *beg="<html>\n<head>\n<META content=\"text/html; charset=gb2312\">\n</head>\n<body>\n";
    ioFile<<beg;

    //�����﷨�༭�����������html�ļ���
    S(ioFile,ps);

    const char *en="</body>\n</html>";
    ioFile<<en;
    ioFile.close();
    rename("out.txt", "out.html");
    return 0;
}

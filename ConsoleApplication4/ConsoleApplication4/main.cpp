#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>   //用于exit
#include <io.h>    //改文件的扩展名

char init[100]; //  存放从文件中读取的原始字符串
int location=0; //location指向待分析的第一个字符在init字符数组中的位置
struct Pos{
    int top;    //到顶部的距离
    int left;   //到左边的距离
    int wd;     //字体的基础大小
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

//处理下标
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
//匹配'$,(,),{,},_,^'
void match_1(char in) {
    if(in==init[location]) {location=location+1;cout<<in<<"\t";}    //单个字符匹配成功,location加1
    else {
        cout<<"在位置"<<location<<"处出现错误!"<<endl;
        cout<<"依据语法应该匹配: "<<in<<endl;
        cout<<"当前字符为: "<<init[location]<<endl;
        exit(0);
    }
}

//匹配id,num,\int,\sum,\blank;
int match_2(string in) {
    int length=0;   //存储num长度
    if(in=="num") { //如果后面是数字，则属于该num记号范畴
        while(init[location]>=48&&init[location]<=57) {
            cout<<init[location];
            location=location+1;
            length++;
        }
        cout<<"\t";
    }
    else if(in=="id") {//如果后面是数组或者字母，则属于该id记号范畴
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
        cout<<"在位置"<<location<<"处出现错误!!!"<<endl;
        cout<<"依据语法应该匹配: "<<in<<endl;
        cout<<"当前字符为: "<<init[location]<<endl;
        exit(1);
    }
    return length;
}

//判断init数组中下一个待分析的类型
string LookAhead(char &lookahead) {
    lookahead=init[location];
    if(lookahead=='$'||lookahead=='('||lookahead==')'||lookahead=='{'||lookahead=='}'||lookahead=='_'||lookahead=='^') {   //若待分析字符属于'$,(,),{,},_,^, ,\n，直接返回
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
        cout<<"在位置"<<location<<"处出现错误"<<endl;
        cout<<"语法中不含字符: "<<lookahead<<endl;
        exit(1);
    }
}

//对init字符数组中的字符进行处理，并以html格式输入到ioFile中
void S(fstream &ioFile,Pos ps) {
    match_1('$');   //对$进行匹配
    B(ioFile,ps);
    match_1('$');
}

//产生式B->TB|T     每处理一次都要修正Pos结构体
int B(fstream &ioFile,Pos ps) {
    int T_left=T(ioFile,ps);
    ps.left=T_left;
    char lookahead=0;
    string result=LookAhead(lookahead);
    if(result=="id"||result=="num"||result=="\\int"||result=="\\sum"||result=="\\blank"||lookahead=='(') { //通过求FIRST(B)判断T后是否有B
        int B_left=B(ioFile,ps);
        ps.left=B_left;
       }
    return ps.left;
}

//产生式T->R_^{B}{B}|R^{B}|R_{B}|R
int T(fstream &ioFile,Pos ps) {
    int R_left=R(ioFile,ps);
    ps.left=R_left;
    char lookahead=0;
    string result=LookAhead(lookahead);
    if(lookahead=='_') {
        match_1('_');
        LookAhead(lookahead);
        if(lookahead=='^') {    //对应产生式 T->R_^{B}{B}
            match_1('^');
            match_1('{');
            int B_left=B(ioFile,Low(ps));     //low(ps)对下标的处理
            match_1('}');
            match_1('{');
            int B2_left=B(ioFile,High(ps));    //High(ps)对上标的处理
            match_1('}');
            if(B_left>B2_left) ps.left=B_left;
            else ps.left=B2_left;
        }
        else if(lookahead=='{') {   //对应产生式T->R_{B}
            match_1('{');
            int B_left=B(ioFile,Low(ps));
            ps.left=B_left;
            match_1('}');
        }
    }
    else if(lookahead=='^') {   //对产生式T->R^{B}的处理
        match_1('^');
        match_1('{');
        int B_left=B(ioFile,High(ps));
        ps.left=B_left;
        match_1('}');
    }
    return ps.left;
}

//产生式R->\int{B}{B}{B}|\sum{B}{B}{B}|id|num|\blank|(B)
int R(fstream &ioFile,Pos ps) {
    char lookahead=0;
    string result=LookAhead(lookahead);
    if(result=="\\blank") {    //对产生式R—>\blank的处理，在文件中输出空格
        match_2("\\blank");
        string s1="<div style=\"position: absolute; top:";
        ioFile<<s1<<ps.top<<"px; left:"<<ps.left<<"px;\">&nbsp</div>\n";
        ps.left=ps.left+sz.blank*(double)ps.wd/50;

       }
    else if(lookahead=='(') {   //对产生式R->(B)的处理，在文件中输出（，处理B，输出）
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
     else if(result=="\\int") { //对产生式\int{B}{B}{B}进行处理
        match_2("\\int");
        string s1="<div style=\"position: absolute; top:";
        ioFile<<s1<<ps.top*0.9<<"px; left:"<<ps.left<<"px;\"><span style=\"font-size:"<<(ps.wd*1.6)<<"; font-style:normal; lineheight:100%;\">∫</span></div>\n";
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
     else if(result=="\\sum") { //对产生式\sum{B}{B}{B}进行处理
        match_2("\\sum");
        string s1="<div style=\"position: absolute; top:";
        ioFile<<s1<<ps.top*0.9<<"px; left:"<<ps.left<<"px;\"><span style=\"font-size:"<<(ps.wd*1.6)<<"; font-style:normal; lineheight:100%;\">∑</span></div>\n";
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
     else if(result=="id") {    //对产生式R->id进行处理
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
     else if(result=="num") {   //对产生式R->num进行处理
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
        cout<<"在位置"<<location<<"处出现错误"<<endl;
        cout<<"当前字符为: "<<init[location]<<endl;
        exit(1);
     }
     return ps.left;
}


//清除读入字符数组中的空格与换行
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
    const string fName1="test05.txt";   //测试文件名
    fstream ioFile;     //读写文件内容

    //定义结构体传递位置参数
    Pos ps{160,500,50};

    //从文件中读取字符串并放入init字符数组中
    ioFile.open(fName1,ios::in|ios::out);
    int i=0;
    while(ioFile.get(c)) {
        init[i]=c;
        i++;
    }
    ioFile.close();

    // 去除字符数组中的空格和回车换行
    Remov(i);

    //向out.txt文件中输出html格式所需的固定文字
    const string fName2="out.txt";
    ioFile.open(fName2,ios::trunc|ios::out);
    const char *beg="<html>\n<head>\n<META content=\"text/html; charset=gb2312\">\n</head>\n<body>\n";
    ioFile<<beg;

    //调用语法编辑器，并输出到html文件中
    S(ioFile,ps);

    const char *en="</body>\n</html>";
    ioFile<<en;
    ioFile.close();
    rename("out.txt", "out.html");
    return 0;
}

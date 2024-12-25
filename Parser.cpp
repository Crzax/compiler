//
// Created by Crzax on 12/12/2024.
//
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <fstream>
#include <vector>
#include "Lex.h"
#include <stack>
#include "Parser.h"
#include <algorithm> // Add this header for reverse and other algorithm utilities

using namespace std;

#define Max_Proc 500
#define Max_Length 500

#define Max_NonTer 60
#define Max_Ter 60
#define Max_Length2 100

int procNum = 0;
//proc的维数都是从1开始的
int proc[Max_Proc][Max_Length]; // 产生式的数组，里边存储了终结符或者非终结符对应的编号
int first[Max_Proc][Max_Length];
int follow[Max_Proc][Max_Length];
int select[Max_Proc][Max_Length];
int LL1[Max_NonTer][Max_Ter][Max_Length2]; // LL(1) 分析表

int connectFirst[Max_Length]; // 将某些First集结合起来的集合

int firstVisit[Max_Proc]; // 记录某非终结符的First集是否已经求过
int followVisit[Max_Proc]; // 记录某非终结符的Follow集是否已经求过

int empty[Max_Proc]; // 可推出空的非终结符的编号
int emptyRecu[Max_Proc]; // 在求可推出空的非终结符的编号集时使用的防止递归的集合
int followRecu[Max_Proc]; // 在求Follow集时使用的防止递归的集合

//extern的部分代表可能出现的终结符和其编号
extern vector<pair<const char *,int> > keyMap;
extern vector<pair<const char *,int> > operMap;
extern vector<pair<const char *,int> > limitMap;

extern NormalNode * normalHead;//首结点

fstream resultfile;

vector<pair<const char *,int> > nonTerMap;//非终结符映射表,不可重复的
vector<pair<const char *,int> > terMap;//终结符映射表,不可重复的
// 将特殊符号映射表定义为静态常量
const std::vector<std::pair<const char *, int>> specialMap = {
        {"->", GRAMMAR_ARROW},
        {"|", GRAMMAR_OR},
        {"$", GRAMMAR_NULL},
        {"#", GRAMMAR_SPECIAL}
};

const char * searchMapping(int num)
{
    //标志符
    if(num == IDENTIFER)
    {
        return "id";
    }
    //处理文法中的特殊符号
    for(int i = 0; i<specialMap.size(); i++)
    {
        if(specialMap[i].second == num)
        {
            return specialMap[i].first;
        }
    }
    //处理非终结符
    for(int i=0; i<nonTerMap.size(); i++)
    {
        if(nonTerMap[i].second == num)
        {
            return nonTerMap[i].first;
        }
    }
    //处理终结符
    for(int i=0; i<terMap.size(); i++)
    {
        if(terMap[i].second == num)
        {
            return terMap[i].first;
        }
    }

}

//动态生成非终结符，在基点的基础上，确保不和终结符冲突
int dynamicNonTer(char *word)
{
    int i = 0;
    int dynamicNum;
    for(i=0; i<nonTerMap.size(); i++)
    {
        if(strcmp(word,nonTerMap[i].first) == 0)
        {
            return nonTerMap[i].second;
        }
    }
    if(i == nonTerMap.size())
    {
        if(i == 0)
        {
            dynamicNum = GRAMMAR_BASE;
            nonTerMap.push_back(make_pair(word,dynamicNum));
        }
        else
        {
            dynamicNum = nonTerMap[nonTerMap.size()-1].second + 1;
            nonTerMap.push_back(make_pair(word,dynamicNum));
        }
    }
    return dynamicNum;
}

//判断某个标号是不是非终结符的标号,1代表是，0代表否
int isNonTer(int n)
{
    for(int i=0; i<nonTerMap.size(); i++)
    {
        if(nonTerMap[i].second == n)
        {
            return 1;
        }
    }
    return 0;
}
//判断某个标号是不是终结符的标号，1代表是，0代表否
int isTer(int n)
{
    for(int i=0; i<terMap.size(); i++)
    {
        if(terMap[i].second == n)
        {
            return 1;
        }
    }
    return 0;
}
//判断某个标号在不在此时的empty集中，1代表是，0代表否
int inEmpty(int n)
{
    //当前Empty集的长度
    int emptyLength = 0;
    for(emptyLength = 0;; emptyLength++)
    {
        if(empty[emptyLength] == -1)
        {
            break;
        }
    }
    for(int i = 0; i<emptyLength; i++)
    {
        if(empty[i] == n)
        {
            return 1;
        }
    }
    return 0;

}
//判断某个标号在不在此时的emptyRecu集中，1代表是，0代表否
int inEmptyRecu(int n)
{
    //当前Empty集的长度
    int emptyLength = 0;
    for(emptyLength = 0;; emptyLength++)
    {
        if(emptyRecu[emptyLength] == -1)
        {
            break;
        }
    }
    for(int i = 0; i<emptyLength; i++)
    {
        if(emptyRecu[i] == n)
        {
            return 1;
        }
    }
    return 0;
}
//判断某个标号在不在此时的followRecu集中，1代表是，0代表否
int inFollowRecu(int n)
{
    int followLength = 0;
    for(followLength = 0;; followLength++)
    {
        if(followRecu[followLength] == -1)
        {
            break;
        }
    }
    for(int i = 0; i<followLength; i++)
    {
        if(followRecu[i] == n)
        {
            return 1;
        }
    }
    return 0;
}

//判断某个标号是不是在产生式的右边
int inProcRight(int n,int * p)
{
    //注意这里默认是从3开始
    for(int i=3;; i++)
    {
        if(p[i] == -1)
        {
            break;
        }
        if(p[i] == n)
        {
            return 1;
        }
    }
    return 0;
}

int seekCodeNum(char * word)
{
    //处理文法中的特殊符号
    for(int i = 0; i<specialMap.size(); i++)
    {
        if(strcmp(word,specialMap[i].first) == 0)
        {
            return specialMap[i].second;
        }
    }
    //先搜索终结符映射表中有没有此终结符
    for(int i=0; i<terMap.size(); i++)
    {
        if(strcmp(word,terMap[i].first) == 0)
        {
            return terMap[i].second;
        }
    }
    for(int i = 0; i<keyMap.size(); i++)
    {
        if(strcmp(word,keyMap[i].first) == 0)
        {
            terMap.push_back(make_pair(word,keyMap[i].second));
            return keyMap[i].second;
        }
    }

    for(int i = 0; i<operMap.size(); i++)
    {
        if(strcmp(word,operMap[i].first) == 0)
        {
            terMap.push_back(make_pair(word,operMap[i].second));
            return operMap[i].second;
        }
    }

    for(int i = 0; i<limitMap.size(); i++)
    {
        if(strcmp(word,limitMap[i].first) == 0)
        {
            terMap.push_back(make_pair(word,limitMap[i].second));
            return limitMap[i].second;
        }
    }

    if(strcmp(word,"id")==0)
    {
        //处理标志符
        terMap.push_back(make_pair(word,IDENTIFER));
        return IDENTIFER;
    }
    else
    {
        //处理关键字、运算符、限界符表，即非终结符
        return dynamicNonTer(word);
    }
}
//分割" | "文法
void splitProc(int p[][Max_Length],int &line,int orNum)
{
    if(p[line][1] == -1 || orNum == 0)
    {
        return;
    }
    int head = p[line][1];
    int push = p[line][2];
    int length = 0;
    int right,left;
    int lineTrue = line + orNum;
    for(length = 3;;length++)
    {
        if(p[line][length] == -1)
        {
            break;
        }
    }
    length--;
    for(left = length,right = length;left>=2;)
    {
        if(p[line][left] == GRAMMAR_OR || left == 2)
        {
            p[line + orNum][1] = head;
            p[line + orNum][2] = push;
            for(int i=left+1;i<=right;i++)
            {
                p[line+orNum][i-left+2] = p[line][i];
            }
            p[line+orNum][right-left+3] = -1;
            right = left = left-1;
            orNum--;
        }
        else
        {
            left--;
        }
    }
    line = lineTrue;
}
void initGrammer()
{
    FILE * infile;
    char ch;
    char array[30];
    char * word;
    int i;
    int codeNum;
    int line = 1;
    int count = 0;
    int orNum = 0;
    infile = fopen("production.txt","r");
    if(!infile)
    {
        printf("文法打开失败！\n");
        return;
    }
    // 打开文件输出流
    std::ofstream outFile("grammar.txt");

    // 检查文件是否成功打开
    if (!outFile) {
        std::cerr << "无法打开文件 grammar.txt" << std::endl;
        return; // 如果文件打开失败，则返回
    }
    nonTerMap.clear();
    terMap.clear();

    memset(proc,-1,sizeof(proc));
    memset(first,-1,sizeof(first));
    memset(follow,-1,sizeof(follow));
    memset(select,-1,sizeof(select));

    memset(connectFirst,-1,sizeof(connectFirst));

    memset(firstVisit,0,sizeof(firstVisit));//非终结符的first集还未求过
    memset(followVisit,0,sizeof(followVisit));//非终结符的follow集还未求过

    memset(empty,-1,sizeof(empty));
    memset(emptyRecu,-1,sizeof(emptyRecu));
    memset(followRecu,-1,sizeof(followRecu));

    memset(LL1,-1,sizeof(LL1));
    // 文法解析
    ch = fgetc(infile); // 从文件中读取一个字符
    i = 0;
    while (ch != EOF) // 读取文件直到结束
    {
        i = 0;

        // 跳过空格和制表符
        while (ch == ' ' || ch == '\t') {
            ch = fgetc(infile);
        }

        // 读取一个文法符号（直到遇到空格、换行或文件结束）
        while (ch != ' ' && ch != '\n' && ch != EOF) {
            array[i++] = ch; // 将字符存储到数组中
            ch = fgetc(infile);
        }

        // 再次跳过空格和制表符
        while (ch == ' ' || ch == '\t') {
            ch = fgetc(infile);
        }

        // 将读取到的字符数组转化为字符串
        word = new char[i + 1];
        memcpy(word, array, i);
        word[i] = '\0';

        // 将字符串映射到整数编码
        codeNum = seekCodeNum(word);
        if (codeNum != 0) { // 如果能找到对应的编码
            count++; // 当前产生式的符号计数
            if (codeNum == GRAMMAR_OR) { // 如果是 " | " 分隔符
                orNum++; // 记录 " | " 的数量
            }
            proc[line][count] = codeNum; // 将符号存入当前产生式数组
        }

        // 如果遇到换行符，表示一条产生式结束
        if (ch == '\n') {
            // 将 " | " 分隔的规则拆分为多个独立的产生式
            splitProc(proc, line, orNum);
            count = 0; // 重置计数器
            orNum = 0; // 重置 " | " 计数器
            line++; // 进入下一条产生式
            ch = fgetc(infile); // 继续读取下一行
        }
    }

    procNum = line - 1;
    outFile<<"----------------------------------文法----------------------------------\n\n";
    for(int i=1; i<line; i++)
    {
        for(int j=1; j<Max_Length; j++)
        {
            if(proc[i][j]!=-1)
            {
                outFile<<searchMapping(proc[i][j])<<" ";
            }
            else
            {
                break;
            }
        }
        outFile<<"\n";
    }
    outFile<<"\n----------------------------------文法终结符----------------------------------\n\n";
    for(int i=0; i<terMap.size(); i++)
    {
        outFile<<terMap[i].first<<" ";
    }
    outFile<<"\n";
    outFile<<"\n----------------------------------文法非终结符----------------------------------\n\n";
    for(int i=0; i<nonTerMap.size(); i++)
    {
        outFile<<nonTerMap[i].first;
    }
    outFile<<"\n";
}

//将s集合合并至d集合中，type = 1代表包括空（$）,type = 2代表不包括空
void merge(int *d,int *s,int type)
{
    int flag = 0;
    for(int i = 0;; i++)
    {
        flag = 0;
        if(s[i] == -1)
        {
            break;
        }
        int j = 0;
        for(j = 0;; j++)
        {
            if(d[j] == -1)
            {
                break;
            }
            if(d[j] == s[i])
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1)
        {
            continue;
        }
        if(type == 1)
        {
            d[j] = s[i];
        }
        else
        {
            if(s[i] != GRAMMAR_NULL)
            {
                d[j] = s[i];
            }
        }
        d[j + 1] = -1;
    }
}

void nullSet(int currentNum)
{
    int temp[2];
    for(int j = 1; j<=procNum; j++)
    {
        //如果右边的第一个是该字符，并且长度只有1
        if(proc[j][3] == currentNum && proc[j][4] == -1)
        {
            temp[0] = proc[j][1];
            temp[1] = -1;
            merge(empty,temp,1);
            nullSet(proc[j][1]);
        }
    }
}

//判断该非终结符是否能推出空，但终结符也可能传入，但没关系
int reduNull(int currentNon)
{
    int temp[2];
    int result = 1;
    int mark = 0;
    temp[0] = currentNon;
    temp[1] = -1;
    merge(emptyRecu,temp,1);//先将此符号并入防递归集合中
    if(inEmpty(currentNon) == 1)
    {
        return 1;
    }

    for(int j = 1; j<=procNum; j++)
    {
        if(proc[j][1] == currentNon)
        {
            int rightLength = 0;
            //先求出右部的长度
            for(rightLength = 3;; rightLength++)
            {
                if(proc[j][rightLength] == -1)
                {
                    break;
                }
            }
            rightLength--;
            //如果长度为1，并且已经求过
            if(rightLength - 2 == 1 && inEmpty(proc[j][rightLength]))
            {
                return 1;
            }
                //如果长度为1，并且是终结符
            else if(rightLength -2 == 1 && isTer(proc[j][rightLength]))
            {
                return 0;
            }
                //如果长度超过了2
            else
            {
                for(int k=3; k<=rightLength; k++)
                {
                    if(inEmptyRecu(proc[j][k]))
                    {
                        mark = 1;
                    }
                }
                if(mark == 1)
                {
                    continue;
                }
                else
                {
                    for(int k=3; k<=rightLength; k++)
                    {
                        result*= reduNull(proc[j][k]);
                        temp[0] = proc[j][k];
                        temp[1] = -1;
                        merge(emptyRecu,temp,1);//先将此符号并入防递归集合中
                    }
                }
            }
            if(result == 0)
            {
                continue;
            }
            else if(result == 1)
            {
                return 1;
            }
        }
    }
    return 0;
}

//求first集，传入的参数是在非终结符集合中的序号
void firstSet(int i)
{
    int k = 0;
    int currentNon = nonTerMap[i].second;//当前的非终结符标号
    //依次遍历全部产生式
    for(int j = 1; j<=procNum; j++) //j代表第几个产生式
    {
        //找到该非终结符的产生式
        if(currentNon == proc[j][1])//注意从1开始
        {
            //当右边的第一个是终结符或者空的时候
            if(isTer(proc[j][3]) == 1 || proc[j][3] == GRAMMAR_NULL)
            {
                //并入当前非终结符的first集中
                int temp[2];
                temp[0] = proc[j][3];
                temp[1] = -1;//其实是模拟字符串操作的手段
                merge(first[i],temp,1);
            }
                //当右边的第一个是非终结符的时候
            else if(isNonTer(proc[j][3]) == 1)
            {
                //如果遇到左递归形式的，直接放过
                if(proc[j][3] == currentNon)
                {
                    continue;
                }
                //记录下右边第一个非终结符的位置
                for(k=0;; k++)
                {
                    if(nonTerMap[k].second == proc[j][3])
                    {
                        break;
                    }

                }
                //当右边第一个非终结符还未访问过的时候
                if(firstVisit[k] == 0)
                {
                    firstSet(k);
                    firstVisit[k] = 1;
                }
                merge(first[i],first[k],2);//如果first[k]此时有空值的话，暂时不把空值并入first[i]中
                int rightLength = 0;
                //先求出右部的长度

                for(rightLength = 3;; rightLength++)
                {
                    if(proc[j][rightLength] == -1)
                    {
                        break;
                    }
                }
                //到目前为止，只求出了右边的第一个(还不包括空的部分)，For循环处理之后的
                for(k = 3; k<rightLength; k++)
                {
                    emptyRecu[0] = -1;//相当于初始化这个防递归集合

                    //如果右部的当前字符能推出空并且还不是最后一个字符，就将之后的一个字符并入First集中
                    if(reduNull(proc[j][k]) == 1 && k<rightLength -1)
                    {
                        int u = 0;
                        for(u=0;; u++)
                        {
                            //注意是记录下一个符号的位置
                            if(nonTerMap[u].second == proc[j][k+1])
                            {
                                break;
                            }
                        }
                        if(firstVisit[u] == 0)
                        {
                            firstSet(u);
                            firstVisit[u] = 1;
                        }
                        merge(first[i],first[u],2);
                    }
                        //到达最后一个字符，并且产生式右部都能推出空,将$并入First集中
                    else if(reduNull(proc[j][k]) == 1 && k == rightLength -1)
                    {
                        int temp[2];
                        temp[0] = GRAMMAR_NULL;
                        temp[1] = -1;//其实是模拟字符串操作的手段
                        merge(first[i],temp,1);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    firstVisit[i] = 1;
}
void First()
{
    // 打开文件输出流
    std::ofstream outFile("parser.txt");

    // 检查文件是否成功打开
    if (!outFile) {
        std::cerr << "无法打开文件 parser.txt" << std::endl;
        return; // 如果文件打开失败，则返回
    }
    //先求出能直接推出空的非终结符集合
    nullSet(GRAMMAR_NULL);
    outFile<<"\n";
    for(int i=0; i<nonTerMap.size(); i++)
    {
        firstSet(i);
    }
    outFile<<"\n----------------------------------First集----------------------------------\n\n";
    for(int i=0; i<nonTerMap.size(); i++)
    {
        outFile<<"First["<<nonTerMap[i].first<<"] = ";
        for(int j=0;; j++)
        {
            if(first[i][j] == -1)
            {
                break;
            }
            outFile<<searchMapping(first[i][j])<<" ";
        }
        outFile<<"\n";
    }
}

//将First结合起来的函数
void connectFirstSet(int *p)
{
    int i = 0;
    int flag = 0;
    int temp[2];
    //如果P的长度为1
    if(p[1] == -1)
    {
        if(p[0] == GRAMMAR_NULL)
        {
            connectFirst[0] = GRAMMAR_NULL;
            connectFirst[1] = -1;
        }
        else
        {
            for(i=0; i<nonTerMap.size(); i++)
            {
                if(nonTerMap[i].second == p[0])
                {
                    flag = 1;
                    merge(connectFirst,first[i],1);
                    break;
                }
            }
            //也可能是终结符
            if(flag == 0)
            {
                for(i=0; i<terMap.size(); i++)
                {
                    if(terMap[i].second == p[0])
                    {
                        temp[0] = terMap[i].second;
                        temp[1] = -1;
                        merge(connectFirst,temp,2);//终结符的First集就是其本身
                        break;
                    }
                }
            }
        }
    }
        //如果p的长度大于1
    else
    {
        for(i=0; i<nonTerMap.size(); i++)
        {
            if(nonTerMap[i].second == p[0])
            {
                flag = 1;
                merge(connectFirst,first[i],2);
                break;
            }
        }
        //也可能是终结符
        if(flag == 0)
        {
            for(i=0; i<terMap.size(); i++)
            {
                if(terMap[i].second == p[0])
                {
                    temp[0] = terMap[i].second;
                    temp[1] = -1;
                    merge(connectFirst,temp,2);//终结符的First集就是其本身
                    break;
                }
            }
        }
        flag = 0;
        int length = 0;
        for(length = 0;; length++)
        {
            if(p[length] == -1)
            {
                break;
            }
        }
        for(int k=0; k<length; k++)
        {
            emptyRecu[0] = -1;//相当于初始化这个防递归集合

            //如果右部的当前字符能推出空并且还不是最后一个字符，就将之后的一个字符并入First集中
            if(reduNull(p[k]) == 1 && k<length -1)
            {
                int u = 0;
                for(u=0; u<nonTerMap.size(); u++)
                {
                    //注意是记录下一个符号的位置
                    if(nonTerMap[u].second == p[k+1])
                    {
                        flag = 1;
                        merge(connectFirst,first[u],2);
                        break;
                    }
                }
                //也可能是终结符
                if(flag == 0)
                {
                    for(u=0; u<terMap.size(); u++)
                    {
                        //注意是记录下一个符号的位置
                        if(terMap[u].second == p[k+1])
                        {
                            temp[0] = terMap[i].second;
                            temp[1] = -1;
                            merge(connectFirst,temp,2);
                            break;
                        }
                    }
                }
                flag = 0;
            }
                //到达最后一个字符，并且产生式右部都能推出空,将$并入First集中
            else if(reduNull(p[k]) == 1 && k == length -1)
            {
                temp[0] = GRAMMAR_NULL;
                temp[1] = -1;//其实是模拟字符串操作的手段
                merge(connectFirst,temp,1);
            }
            else
            {
                break;
            }
        }
    }
}
void followSet(int i)
{
    int currentNon = nonTerMap[i].second;//当前的非终结符标号
    int temp[2];
    int result = 1;
    temp[0] = currentNon;
    temp[1] = -1;
    merge(followRecu,temp,1);//将当前标号加入防递归集合中

    //如果当前符号就是开始符号,把特殊符号加入其Follow集中
    if(proc[1][1] == currentNon)
    {
        temp[0] = GRAMMAR_SPECIAL;//这个也是要处理的
        temp[1] = -1;
        merge(follow[i],temp,1);
    }
    for(int j = 1; j<=procNum; j++) //j代表第几个产生式
    {
        //如果该非终结符在某个产生式的右部存在
        if(inProcRight(currentNon,proc[j]) == 1)
        {
            int rightLength = 1;
            int k = 0;//k为该非终结符在产生式右部的序号  
            int flag = 0;
            int leftNum = proc[j][1];//产生式的左边  
            int h = 0;
            int kArray[Max_Length2];
            memset(kArray,-1,sizeof(kArray));
            for(h = 0; h < nonTerMap.size(); h++)
            {
                if(nonTerMap[h].second == leftNum)
                {
                    break;
                }
            }

            for(rightLength = 1;; rightLength++)
            {
                if(currentNon == proc[j][rightLength+2])
                {
                    kArray[k++] = rightLength;
                }
                if(proc[j][rightLength+2] == -1)
                {
                    break;
                }
            }
            rightLength--;
            for(int y=0;; y++)
            {
                if(kArray[y] == -1)
                {
                    break;
                }
                //如果该非终结符在右部产生式的最后  
                if(kArray[y] == rightLength)
                {

                    if(inFollowRecu(leftNum) == 1)
                    {
                        merge(follow[i],follow[h],1);
                        continue;
                    }
                    if(followVisit[h] == 0)
                    {
                        followSet(h);
                        followVisit[h] = 1;
                    }
                    merge(follow[i],follow[h],1);
                }
                    //如果不在最后
                else
                {
                    int n = 0;
                    result = 1;//这是关键的，曾在这里失误过  
                    for(n=kArray[y]+1; n<=rightLength; n++)
                    {
                        emptyRecu[0] = -1;
                        result *= reduNull(proc[j][n+2]);
                    }
                    if(result == 1)
                    {
                        if(inFollowRecu(leftNum) == 1)
                        {
                            merge(follow[i],follow[h],1);
                            continue;
                        }
                        if(followVisit[h] == 0)
                        {
                            followSet(h);
                            followVisit[h] = 1;
                        }
                        merge(follow[i],follow[h],1);
                    }
                    int temp2[Max_Length];
                    memset(temp2,-1,sizeof(temp2));
                    for(n=kArray[y]+1; n<=rightLength; n++)
                    {
                        temp2[n-kArray[y]-1] = proc[j][n+2];
                    }
                    temp2[rightLength-kArray[y]] = -1;
                    connectFirst[0] = -1;//应该重新初始化一下  
                    connectFirstSet(temp2);
                    merge(follow[i],connectFirst,2);
                }
            }
        }
    }
    followVisit[i] = 1;
}

//求所有非终结符的Follow集  
void Follow()
{
    // 打开文件输出流，并设置为追加模式
    std::ofstream outFile("parser.txt", std::ios::app);

    // 检查文件是否成功打开
    if (!outFile) {
        std::cerr << "无法打开文件 parser.txt" << std::endl;
    }

    for(int i=0; i<nonTerMap.size(); i++)
    {
        followRecu[0] = -1;
        followSet(i);
    }
    outFile<<"\n----------------------------------Follow集----------------------------------\n\n";
    for(int i=0; i<nonTerMap.size(); i++)
    {
        outFile<<"Follow["<<nonTerMap[i].first<<"] = ";
        for(int j=0;; j++)
        {
            if(follow[i][j] == -1)
            {
                break;
            }
            outFile<<searchMapping(follow[i][j])<<" ";
        }
        outFile<<"\n";
    }
}

//求已经分解的产生式对应的Select集,注意Select集中不能含有空($),因而Type=2  
void Select()
{
    // 打开文件输出流，并设置为追加模式
    std::ofstream outFile("parser.txt", std::ios::app);

    // 检查文件是否成功打开
    if (!outFile) {
        std::cerr << "无法打开文件 parser.txt" << std::endl;
    }

    for(int i = 1; i<=procNum; i++) //j代表第几个产生式  
    {
        int leftNum = proc[i][1];//产生式的左边  
        int h = 0;
        int result = 1;
        for(h = 0; h < nonTerMap.size(); h++)
        {
            if(nonTerMap[h].second == leftNum)
            {
                break;
            }
        }

        int rightLength = 1;
        for(rightLength = 1;; rightLength++)
        {
            if(proc[i][rightLength+2] == -1)
            {
                break;
            }
        }
        rightLength--;
        //如果右部推出式的长度为1并且是空,select[i-1] = follow[左边]  
        if(rightLength == 1 && proc[i][rightLength + 2] == GRAMMAR_NULL)
        {
            merge(select[i-1],follow[h],2);
        }
            //如果右部不是空的时候,select[i-1] = first[右部全部]
            //如果右部能够推出空，select[i-1] = first[右部全部] ^ follow[左边]
        else
        {
            int temp2[Max_Length];
            int n = 0;
            memset(temp2,-1,sizeof(temp2));
            for(n=1; n<=rightLength; n++)
            {
                temp2[n-1] = proc[i][n+2];
            }
            temp2[rightLength] = -1;
            connectFirst[0] = -1;//应该重新初始化一下  
            connectFirstSet(temp2);
            merge(select[i-1],connectFirst,2);
            for(n=1; n<=rightLength; n++)
            {
                emptyRecu[0] = -1;
                result *= reduNull(proc[i][n+2]);
            }
            //如果右部能推出空，将follow[左边]并入select[i-1]中  
            if(result == 1)
            {
                merge(select[i-1],follow[h],2);
            }
        }
    }
    outFile<<"\n-----------------------------Select集-----------------------------\n\n";
    for(int i=0; i < procNum; i++)
    {
        outFile<<"Select["<<i+1<<"] = ";
        for(int j=0;; j++)
        {
            if(select[i][j] == -1)
            {
                break;
            }
            outFile<<searchMapping(select[i][j])<<" ";
        }
        outFile<<"\n";
    }
}

//输出预测分析表  
void LL1Table()
{
    fstream outfile;
    outfile.open("LL(1)Table.txt",ios::out);

    for(int i=0; i<procNum; i++)
    {
        int m = 0;//非终结符的序号  
        for(int t=0; t<nonTerMap.size(); t++)
        {
            if(nonTerMap[t].second == proc[i+1][1])
            {
                m = t;
                break;
            }
        }

        for(int j=0;; j++)
        {
            if(select[i][j] == -1)
            {
                break;
            }
            for(int k=0; k<terMap.size(); k++)
            {
                if(terMap[k].second == select[i][j])
                {
                    int n = 0;
                    for(n=1; n<=Max_Length2; n++)
                    {
                        LL1[m][k][n-1] = proc[i+1][n];
                        if(proc[i+1][n] == -1)
                        {
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    outfile<<endl<<"-----------------------------预测分析表-----------------------------"<<endl;
    for(int i=0; i<nonTerMap.size(); i++)
    {
        for(int j=0; j<terMap.size(); j++)
        {
            outfile<<"LL1["<<nonTerMap[i].first<<"]["<<terMap[j].first<<"] = ";
            for(int k=0;; k++)
            {
                if(LL1[i][j][k] == -1)
                {
                    break;
                }
                outfile<<searchMapping(LL1[i][j][k]);
            }
            outfile<<endl;
        }
        outfile<<endl<<endl;
    }
    outfile.close();
}

void ShowStack(std::stack<int> s, std::fstream &resultfile)  // 传递栈的副本
{
    // 将原栈的内容拷贝到另一个栈用于输出
    while (!s.empty()) {
        resultfile << searchMapping(s.top()) << " ";
        s.pop();
    }
}

//分析源程序
void Analysis()
{
    //分析结果输出  
    resultfile.open("ParserResult.txt",ios::out);

    stack<int> s1,s2;
    int c1,c2;
    int i = 0;
    int reserve[Stack_Size];//符号栈反向入栈  
    NormalNode * p = normalHead;
    int s1Length = 0;
    memset(reserve,-1,sizeof(reserve));

    s1.push(GRAMMAR_SPECIAL);
    s1.push(proc[1][1]);
    s2.push(GRAMMAR_SPECIAL);

    p = p->next;
    while(p!=NULL)
    {

        if(p->type == AUTO || p->type == CONST || p->type == UNSIGNED || p->type == SIGNED
           || p->type ==STATIC || p->type == VOLATILE )
        {
            reserve[i++] =  DESCRIBE;
        }
        else if(p->type == INT_VAL)
        {
            reserve[i++] =  DIGIT;
        }
        else if(p->type == CHAR || p->type == DOUBLE || p->type == FLOAT || p->type == INT ||
                p->type == LONG || p->type == SHORT || p->type == VOID)
        {
            reserve[i++] =  TYPE;
        }
        else if(p->type == STRING_VAL)
        {
            reserve[i++] =  STRING;
        }
        else if(p->type == DOU_QUE || p->type == SIN_QUE)
        {

        }
        else
        {
            reserve[i++] =  p->type;
        }
        p = p->next;
    }
    //求左边栈的长度  
    for(s1Length = 0;; s1Length++)
    {
        if(reserve[s1Length] == -1)
        {
            break;
        }
    }
    //反向入栈  
    for(i = s1Length; i>0; i--)
    {
        s2.push(reserve[i-1]);
    }

    for(i=0;; i++)   /*分析*/
    {
        int flag = 0;
        int h1;
        int h2;
        resultfile<<"第"<<i + 1<<"步"<<endl;
        resultfile<<"分析栈:";
        ShowStack(s1, resultfile);
        resultfile<<endl;
        resultfile<<"遗留串:";
        ShowStack(s2, resultfile);
        resultfile<<endl;

        c1 = s1.top();
        c2 = s2.top();
        if(c1 == GRAMMAR_SPECIAL && c2 == GRAMMAR_SPECIAL)  /*当符号栈和输入栈都剩余#时，分析成功*/
        {
            resultfile<<"成功!"<<endl;
            break;
        }
        if(c1 == GRAMMAR_SPECIAL && c2!= GRAMMAR_SPECIAL)  /*当符号栈剩余#，而输入串未结束时，分析失败 */
        {
            resultfile<<"失败!"<<endl;
            break;
        }
        if(c1 == c2)/*符号栈的栈顶元素和输入串的栈顶元素相同时，同时弹出*/
        {
            s1.pop();
            s2.pop();
            flag = 1;
        }

        else /*查预测分析表*/
        {
            //记录下非终结符的位置  
            for(h1=0; h1<nonTerMap.size(); h1++)
            {
                if(nonTerMap[h1].second == c1)
                {
                    break;
                }
            }
            //记录下终结符的位置  
            for(h2=0; h2<terMap.size(); h2++)
            {
                if(terMap[h2].second == c2)
                {
                    break;
                }
            }
            if(LL1[h1][h2][0] == -1)
            {
                resultfile<<"Error"<<endl;
                break;//如果错误的话，直接终止分析  
            }
            else
            {
                int length = 0;
                //记录下推导式的长度  
                for(length = 0;; length++)
                {
                    if(LL1[h1][h2][length] == -1)
                    {
                        break;
                    }
                }
                s1.pop();
                //如果不是空的话，反向入栈  
                if(LL1[h1][h2][2] != GRAMMAR_NULL)
                {
                    for(int k = length-1; k>=2; k--)
                    {
                        s1.push(LL1[h1][h2][k]);
                    }
                }
            }
        }
        if(flag == 1)
        {
            resultfile<<"行为: "<<searchMapping(c1)<<"匹配"<<endl<<endl;
        }
        else
        {
            resultfile<<"行为: ";
            int w = 0;
            for(w = 0;; w++)
            {
                if(LL1[h1][h2][w] == -1)
                {
                    break;
                }
                resultfile<<searchMapping(LL1[h1][h2][w]);
            }
            resultfile<<endl<<endl;
        }
    }
    resultfile.close();
}
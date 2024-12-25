#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include "Lex.h"
#include "Parser.h"

using namespace std;

int main()
{
    //词法分析部分
    initNode();
    scanner();
    BraMappingError();
    printInfo();

    //语法分析部分
    initGrammer();
    First();
    Follow();
    Select();
    MTable();
    Analysis();
    close();
    return 0;
}

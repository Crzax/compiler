//
// Created by Crzax on 12/12/2024.
//
#include "Lex.h"
#include "Parser.h"

int main()
{
    //词法分析
    initNode();
    scanner();
    BraMappingError();
    printInfo();

    //语法分析
    initGrammer();
    First();
    Follow();
    Select();
    LL1Table();
    Analysis();
    return 0;
}
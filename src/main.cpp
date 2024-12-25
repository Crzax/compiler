//
// Created by Crzax on 12/12/2024.
//
#include "Lex.h"
#include "Parser.h"
#include <iostream>

int main(int argc, char* argv[])
{
    // 检查命令行参数数量
    if (argc < 2) {
        std::cerr << "使用方法: " << argv[0] << " <文件名>" << std::endl;
        return 1; // 返回错误代码
    }

    // 获取文件名称
    const char* filename = argv[1];

    // 词法分析
    initNode();
    scanner(filename); // 假设scanner函数可以接收文件名作为参数
    BraMappingError();
    printInfo();

    // 语法分析
    initGrammer();
    computeFirst();
    computeFollow();
    computeSelect();
    LL1Table();
    Analysis();
    
    return 0;
}


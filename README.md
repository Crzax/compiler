# C/C++词法解析与语法分析

## 项目简介

这个项目实现了一个C/C++词法解析与语法分析器。词法解析生成中间结果为符号表、错误表和标识符表。语法分析使用LL(1)分析法，生成结果为文法、LL(1)分析表、First集Follow集Select集、LL(1)分析过程以及语法分析树文件。该项目同时是武汉大学编译原理课程两次实习作业。

## 项目文件夹介绍

- `build`: 生成的程序位置
- `include`:词法解析与语法分析的代码头文件
- `REF`: 参考项目
- `src`: 词法解析与语法分析的代码源文件
- `test`: 用来测试词法解析与语法解析的测试文件夹
- `production.txt`: 产生式文件，用于语法解析，可以自定义产生式，程序会自动解析生成LL(1)分析表
- `CMakeLists.txt`: CMake配置文件

## 使用方式-以Windows的MinGW环境为例

### 生成词法解析与语法分析程序

进入`build`文件夹，生成ninja配置文件:

```bash
cmake ..
```

使用ninja生成程序:

```bash
ninja
```

这样在`build`文件夹下就生成了词法解析与语法分析程序`LexAndParser.exe`

### 放入测试文件

进入`test`文件夹，把需要解析的C/C++文件放入`case`文件夹内

### 使用Python进行测试

为了方便测试，这里写了`Python`脚本，通过`Python`调用程序并生成结果。

进入`test`文件夹，如果想对所有文件都进行解析，那就使用

```bash
python3 test.py 
```

如果只想针对单个文件进行解析，比如`test.cpp`，那就使用

```bash
python3 test.py test.cpp
```

生成的结果会放入`result`文件夹，对应文件的解析结果会放入到对应名字的文件夹内，比如`test.cpp`的文件的结果就会放入`result`文件夹的`test`文件夹内

### 生成的结果文件介绍

- grammar.txt: 根据产生式、teminal和non-terminal生成的文法文件
- lex.txt: 词法解析的中间结果，包括符号表、错误表和标识符表
- LL(1)Table.txt: LL(1)分析表
- parser.txt: 得到的First集、Follow集和Select集
- ParserResult.txt: LL(1)的分析过程和语法树结果，其中语法树同样缩进表示在语法树的同一层

这里给出了两个简单的测试文件，分别是normal.cpp和erro.cpp，位于`test/case`文件夹内，生成的结果位于`test/result`文件夹内，它们分别表示正常情况和异常情况。


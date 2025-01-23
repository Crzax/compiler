# C/C++ Lexical and Syntax Analysis

[中文版](README.md)

## Project Overview

This project implements a C/C++ lexical and syntax analyzer. The lexical analysis generates intermediate results including a symbol table, an error table, and an identifier table. The syntax analysis utilizes the LL(1) parsing method to generate outputs such as grammar, an LL(1) parsing table, First set, Follow set, Select set, the LL(1) parsing process, and a syntax analysis tree file. This project serves as two assignments for the Compiler Principles course at Wuhan University.

## Project Directory Structure

- `include`: Header files for lexical and syntax analysis code
- `REF`: Reference projects
- `src`: Source files for lexical and syntax analysis
- `test`: Folder containing test files for lexical and syntax analysis
- `production.txt`: Production file for syntax analysis, customizable to generate an LL(1) parsing table automatically
- `CMakeLists.txt`: CMake configuration file

## Usage - Example with Windows MinGW Environment

### Building the Lexical and Syntax Analysis Program

Create a `build` folder in the project directory:

```bash
mkdir build
```

Navigate to the `build` folder and generate ninja configuration files:

```bash
cd build
cmake ..
```

Use ninja to build the program:

```bash
ninja
```

This will generate the lexical and syntax analysis program `LexAndParser.exe` in the `build` folder.

### Adding Test Files

Navigate to the `test` folder and place the C/C++ files you want to analyze into the `case` folder.

### Testing with Python

For convenient testing, a Python script is provided to call the program and generate results.

Navigate to the `test` folder. To analyze all files, use:

```bash
python test.py 
```

To analyze a specific file, such as `test.cpp`, use:

```bash
python test.py test.cpp
```

The generated results will be placed in the `result` folder. The analysis results for each file will be placed in a corresponding folder with the same name, for example, results for `test.cpp` will be in the `test` folder within `result`.

### Explanation of Result Files

- `grammar.txt`: Grammar file generated based on productions, terminals, and non-terminals
- `lex.txt`: Intermediate results of lexical analysis, including the symbol table, error table, and identifier table
- `LL(1)Table.txt`: LL(1) parsing table
- `parser.txt`: First set, Follow set, and Select set
- `ParserResult.txt`: LL(1) analysis process and syntax tree results, where indentation indicates the same level in the syntax tree

Two simple test files are provided: `normal.cpp` and `erro.cpp`, located in the `test/case` folder. The results are located in the `test/result` folder, representing normal and abnormal cases, respectively.
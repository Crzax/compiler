import os
import subprocess
import shutil
import sys

# 设置当前终端为UTF-8编码
os.environ['PYTHONIOENCODING'] = 'utf-8'

# 定义可执行文件路径
exe_path = r'..\build\LexAndParser.exe'

# 获取当前目录下case目录的所有CPP文件
case_dir = os.path.join(os.getcwd(), 'case')

# 检查命令行参数
if len(sys.argv) > 1:
    cpp_file = sys.argv[1]  # 处理指定的文件
    if not cpp_file.endswith('.cpp'):
        print("请提供一个有效的CPP文件名。")
        sys.exit(1)
    
    cpp_file_path = os.path.join(case_dir, cpp_file)
    if not os.path.exists(cpp_file_path):
        print(f"指定的文件 '{cpp_file}' 不存在。")
        sys.exit(1)
    
    cpp_files = [cpp_file]  # 只处理指定的文件
else:
    cpp_files = [f for f in os.listdir(case_dir) if f.endswith('.cpp')]  # 默认处理所有CPP文件

# 确保result目录存在
result_dir = os.path.join(os.getcwd(), 'result')
os.makedirs(result_dir, exist_ok=True)

# 遍历每个CPP文件并调用可执行文件
for cpp_file in cpp_files:
    cpp_file_path = os.path.join(case_dir, cpp_file)
    
    # 调用可执行文件
    subprocess.run([exe_path, cpp_file_path])
    
    # 创建对应的文件夹
    file_base_name = os.path.splitext(cpp_file)[0]
    output_dir = os.path.join(result_dir, file_base_name)
    os.makedirs(output_dir, exist_ok=True)
    
    # 定义要移动的文件
    output_files = [
        'LL(1)Table.txt',
        'ParserResult.txt',
        'grammar.txt',
        'lex.txt',
        'parser.txt'
    ]
    
    # 移动文件到对应的文件夹
    for output_file in output_files:
        source_file = os.path.join(os.getcwd(), output_file)
        if os.path.exists(source_file):
            destination_file = os.path.join(output_dir, output_file)
            shutil.move(source_file, destination_file)

print("所有文件处理完成。")

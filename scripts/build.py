import subprocess
import argparse
import sys
import os

def run_command(command):
    """运行命令并检查错误"""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"错误: 命令 '{e.cmd}' 返回非零退出码 {e.returncode}", file=sys.stderr)
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="构建和测试 Nota 项目")
    parser.add_argument("--test", action="store_true", help="构建后运行测试")
    args = parser.parse_args()

    # 创建构建目录（如果不存在）
    if not os.path.exists("build"):
        os.makedirs("build")

    # 进入构建目录
    os.chdir("build")

    # 运行 CMake 配置
    print("--- 配置 CMake... ---")
    run_command("cmake ..")

    # 运行构建
    print("--- 构建项目... ---")
    run_command("cmake --build .")

    # 如果指定了 --test，则运行测试
    if args.test:
        print("--- 运行测试... ---")
        # 使用 ctest 运行测试，它会自动找到测试可执行文件
        run_command("ctest")

    # 返回到项目根目录
    os.chdir("..")

if __name__ == "__main__":
    main()

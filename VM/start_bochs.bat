@echo off
REM 检查当前目录下是否存在Configuration.bxrc文件
if exist Configuration.bxrc (
    
    REM 使用绝对路径启动Bochs
    bochs -f %~dp0Configuration.bxrc
    
) else (
    REM 如果不存在Configuration.bxrc文件，输出提示信息并退出
    echo Configuration.bxrc文件不存在，程序将退出。
    exit /b
)

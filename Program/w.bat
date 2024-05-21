@echo off

rem 先执行make操作
make all

setlocal

rem 确定mbr.bin文件的路径
set MBR_BIN=%~dp0boot\mbr.bin
set LOADER_BIN=%~dp0boot\loader.bin

rem 确定目标硬盘映像文件的相对路径
set HD_IMG_REL=..\VM\HOS.raw

rem 计算目标硬盘映像文件的绝对路径
for %%I in (%HD_IMG_REL%) do set HD_IMG=%%~fI

rem 输出涉及各文件的路径信息
echo MBR_BIN=%MBR_BIN%
echo LOADER_BIN=%LOADER_BIN%
echo HD_IMG=%HD_IMG%

rem 切换到脚本所在的目录
cd /d %~dp0

rem 首先清空硬盘
dd if=/dev/zero of=%HD_IMG% bs=1M count=64 conv=notrunc
rem 使用dd将mbr.bin写入硬盘映像文件
dd if=%MBR_BIN% of=%HD_IMG% bs=512 count=1 conv=notrunc
dd if=%LOADER_BIN% of=%HD_IMG% bs=512 count=1 seek=2 conv=notrunc

rem 执行clean操作
make clean

endlocal
pause

#!/bin/bash

echo "请选择要复制的库类型:"
echo "1) 多线程 (/MT)"
echo "2) 多线程调试 (/MTd)"
echo "3) 多线程 DLL (/MD)"
echo "4) 多线程调试 DLL (/MDd)"

read -p "请输入选项 (1-4): " option

case $option in
    1)
        cp AEXModule.lib AEXModule_MT.lib
        cp AEXModule_64.lib AEXModule_MT_64.lib
        echo "已复制 MT 版本库"
        ;;
    2)
        cp AEXModule.lib AEXModule_MTd.lib
        cp AEXModule_64.lib AEXModule_MTd_64.lib
        echo "已复制 MTd 版本库"
        ;;
    3)
        cp AEXModule.lib AEXModule_MD.lib
        cp AEXModule_64.lib AEXModule_MD_64.lib
        echo "已复制 MD 版本库"
        ;;
    4)
        cp AEXModule.lib AEXModule_MDd.lib
        cp AEXModule_64.lib AEXModule_MDd_64.lib
        echo "已复制 MDd 版本库"
        ;;
    *)
        echo "无效选项。请手动运行脚本并选择 1-4 之间的数字。"
        ;;
esac

# 添加输入拦截，等待用户按下任意键后退出
read -n 1 -s -r -p "按任意键退出..."
echo

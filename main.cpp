//// AEXModule.cpp : 锟斤拷锟侥硷拷锟斤拷锟斤拷 "main" 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷执锟叫斤拷锟节此达拷锟斤拷始锟斤拷锟斤拷锟斤拷锟斤拷
////

#include "AEXModule.h" 
#include <iostream>
#include <thread>
#include <windows.h>
int main() {
    try {
        std::cout << std::to_string(123456789012345678.123456789012345678) << std::endl;
        long long time = System::GetRunTime();
        Doubles a("1.1", 50);
        Doubles b("-67.89");
        std::string c("666");
        Doubles e("5"), f("-5"), g("3");
        //std::cout << "Sum: " << (a + "67.89") << "正确值:" << (1.1 + 67.89) << std::endl;
        //std::cout << "Sum: " << (a + b) << "正确值:" << (1.1 + -67.89) << std::endl;
        //std::cout << "Sum: " << (a + c) << "正确值:" << (1.1 + 666) << std::endl;
        //std::cout << "-----------------------------------------" << std::endl;
        //std::cout << "Sum: " << (a - "67.89") << "正确值:" << (1.1 - 67.89) << std::endl;
        //std::cout << "Sum: " << (a - b) << "正确值:" << (1.1 - -67.89) << std::endl;
        //std::cout << "Sum: " << (a - c) << "正确值:" << (1.1 - 666) << std::endl;
        //std::cout << "-----------------------------------------" << std::endl;
        //std::cout << "Sum: " << (a * "67.89") << "正确值:" << (1.1 * 67.89) << std::endl;
        //std::cout << "Sum: " << (a * b) << "正确值:" << (1.1 * -67.89) << std::endl;
        //std::cout << "Sum: " << (a * c) << "正确值:" << (1.1 * 666) << std::endl;
        //std::cout << "-----------------------------------------" << std::endl;
        //std::cout << "Sum: " << (a / "67.89") << "正确值:" << (1.1 / 67.89) << std::endl;
        //std::cout << "Sum: " << (a / b) << "正确值:" << (1.1 / -67.89) << std::endl;
        //std::cout << "Sum: " << (a / c) << "正确值:" << (1.1 / 666) << std::endl;
        //std::cout << "-----------------------------------------" << std::endl;
        //std::cout << "位运算测试" << std::endl;
        //std::cout << "Bit AND: " << (e & g) << " 正确值:" << (5 & 3) << std::endl;
        //std::cout << "Bit AND: " << (f & g) << " 正确值:" << (-5 & 3) << std::endl;
        //std::cout << "Bit OR:  " << (e | g) << " 正确值:" << (5 | 3) << std::endl;
        //std::cout << "Bit OR:  " << (f | g) << " 正确值:" << (-5 | 3) << std::endl;
        //std::cout << "Bit XOR: " << (e ^ g) << " 正确值:" << (5 ^ 3) << std::endl;
        //std::cout << "Bit XOR: " << (f ^ g) << " 正确值:" << (-5 ^ 3) << std::endl;
        //e &= g;
        //std::cout << "Compound &: " << e << " 正确值:" << (5 & 3) << std::endl;
        //f |= g;
        //std::cout << "Compound |: " << f << " 正确值:" << (-5 | 3) << std::endl;
        //e ^= g;
        //std::cout << "Compound ^: " << e << " 正确值:" << (1 ^ 3) << std::endl;
        //Doubles DDD("114514.6666");
        //std::cout << "取整: " << DDD.sqrt() << " 正确值: "<< sqrt(114514.6666) << std::endl;
        Doubles fgh;
        for (int i = 0; i < 10000; i++) {
            fgh = a + b;
            fgh = e & g;
            fgh = a - b;
            fgh = a * b;
            fgh = a / b;
            fgh = e% g;
            fgh = e | g;
            fgh = e ^ g;
            fgh = a == b;
            fgh = a != b;
            fgh = a <  b;
            fgh = a <= b;
            fgh = a >  b;
            fgh = a >= b;
        }
        std::cout << "运行时间:" << System::GetRunTime() - time << "毫秒" << std::endl;
        time = System::GetRunTime();
        double a1 = 1.1, b1 = -67.89, c1 = 666;
        long e1 = 5.0f, f1 = -5.0f, g1 = 3.0f;
        for (int i = 0; i < 10000000; i++) {
            double fgh1 = a1 + b1;
            fgh1 = e1 & g1;
            fgh1 = a1 - b1;
            fgh1 = a1 * b1;
            fgh1 = a1 / b1;
            fgh1 = e1% g1;
            fgh1 = e1 | g1;
            fgh1 = e1 ^ g1;
            fgh1 = a1 == b1;
            fgh1 = a1 != b1;
            fgh1 = a1 <  b1;
            fgh1 = a1 <= b1;
            fgh1 = a1 >  b1;
            fgh1 = a1 >= b1;
        }
        std::cout << "运行时间:" << System::GetRunTime() - time << "毫秒" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
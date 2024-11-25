/**
 * @file FloatToDex.cpp
 * @brief 浮点数转换为十进制表示
 * 
 * 本文件实现了 `FloatToDex` 模块中的两个函数：
 * - `getFloatIEEE`：获取浮点数的 IEEE 754 表示
 * - `binaryToDecimal`：将二进制字符串转换为十进制整数
 */
//? 辅助工具，用于将浮点数转换为十进制表示
#include "../include/backend/FloatToDex.hpp"

void getFloatIEEE(float value) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&value);
    
}
int binaryToDecimal(const std::string& binStr) {
    int decimal = 0;
    for (char bit : binStr) {
        decimal *= 2;
        if (bit == '1') {
            decimal += 1;
        }
    }
    return decimal;
}
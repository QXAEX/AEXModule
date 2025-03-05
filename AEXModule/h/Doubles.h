#pragma once
#include <string>
#include <vector>
#include <iostream>

class Doubles {
public:
    Doubles() = default;
    /*
    * 构造函数，默认精度为50位
    * @param number 数字字符串，可以带符号
    * @param precision 精度，即小数点后保留几位
    */
    Doubles(const std::string& number, size_t precision = 50);
    /*
    * 设置精度
    * @param precision 精度，即小数点后保留几位
    */
    void setPrecision(size_t precision);
    /*
    * 获取精度
    * @return 精度
    */
    size_t getPrecision() const;
    /*
    * 设置数字
    * @param number 数字字符串，可以带符号
    */
    void setNumber(const std::string& number);
    /*
    * 获取数字
    * @return 数字字符串，可以带符号
    */
    std::string getNumber() const;
    /*
    * 是否为负数
    * @return true为负数，false为正数
    */
    bool isNegative() const;
    /*
    * 求平方根
    * @return 平方根
    */
    Doubles& sqrt();
    /*
    * 求绝对值
    * @return 绝对值
    */
    Doubles abs() const;
    // 运算符重载
    Doubles& operator=(const Doubles& other);
    Doubles& operator=(const std::string& other);
    Doubles& operator=(const char* other);
    Doubles& operator=(float other);
    Doubles& operator=(double other);
    Doubles& operator=(int other);
    Doubles& operator=(unsigned int other);
    Doubles& operator=(long long other);
    Doubles& operator=(unsigned long long other);
    Doubles operator+(const Doubles& other) const;
    Doubles operator+(const std::string& other) const;
    Doubles operator+(const char* other) const;
    Doubles& operator+=(const Doubles& other);
    Doubles& operator+=(const std::string& other);
    Doubles& operator+=(const char* other);
    Doubles operator-(const Doubles& other) const;
    Doubles operator-(const std::string& other) const;
    Doubles operator-(const char* other) const;
    Doubles& operator-=(const Doubles& other);
    Doubles& operator-=(const std::string& other);
    Doubles& operator-=(const char* other);
    Doubles operator*(const Doubles& other) const;
    Doubles operator*(const std::string& other) const;
    Doubles operator*(const char* other) const;
    Doubles& operator*=(const Doubles& other);
    Doubles& operator*=(const std::string& other);
    Doubles& operator*=(const char* other);
    Doubles operator/(const Doubles& other) const;
    Doubles operator/(const std::string& other) const;
    Doubles operator/(const char* other) const;
    Doubles& operator/=(const Doubles& other);
    Doubles& operator/=(const std::string& other);
    Doubles& operator/=(const char* other);
    Doubles operator%(const Doubles& other) const;
    Doubles operator%(const std::string& other) const;
    Doubles operator%(const char* other) const;
    Doubles& operator%=(const Doubles& other);
    Doubles& operator%=(const std::string& other);
    Doubles& operator%=(const char* other);
    Doubles operator&(const Doubles& other) const;
    Doubles operator&(const std::string& other) const;
    Doubles operator&(const char* other) const;
    Doubles& operator&=(const Doubles& other);
    Doubles& operator&=(const std::string& other);
    Doubles& operator&=(const char* other);
    Doubles operator|(const Doubles& other) const;
    Doubles operator|(const std::string& other) const;
    Doubles operator|(const char* other) const;
    Doubles& operator|=(const Doubles& other);
    Doubles& operator|=(const std::string& other);
    Doubles& operator|=(const char* other);
    Doubles operator^(const Doubles& other) const;
    Doubles operator^(const std::string& other) const;
    Doubles operator^(const char* other) const;
    Doubles& operator^=(const Doubles& other);
    Doubles& operator^=(const std::string& other);
    Doubles& operator^=(const char* other);

    // 比较运算符
    bool operator !=(const Doubles& other) const;
    bool operator==(const Doubles& other) const;
    bool operator<=(const Doubles& other) const;
    bool operator<(const Doubles& other) const;
    bool operator>=(const Doubles& other) const;
    bool operator>(const Doubles& other) const;
    // 输出运算符
    friend std::ostream& operator<<(std::ostream& os, const Doubles& num);
private:
    bool is_negative = false;// 是否为负数
    size_t precision = 50;      // 精度，即小数点后保留几位
    std::vector<uint8_t> integer;    // 大端存储，例如123存为[1,2,3]
    std::vector<uint8_t> fractional; // 小端存储，例如0.45存为[4,5]
private:
    static void parseNumber(const std::string& num, std::vector<uint8_t>& integer, std::vector<uint8_t>& fractional, bool& is_negative);
    static void normalize(std::vector<uint8_t>& integer, std::vector<uint8_t>& fractional, size_t precision);
    void normalize();
    static void alignDecimals(std::vector<uint8_t>& aInt, std::vector<uint8_t>& aFrac, std::vector<uint8_t>& bInt, std::vector<uint8_t>& bFrac);
    static void addCore(const std::vector<uint8_t>& aInt, const std::vector<uint8_t>& aFrac, const std::vector<uint8_t>& bInt, const std::vector<uint8_t>& bFrac, std::vector<uint8_t>& resInt, std::vector<uint8_t>& resFrac);
    static void subtractCore(const std::vector<uint8_t>& largerInt, const std::vector<uint8_t>& largerFrac, const std::vector<uint8_t>& smallerInt, const std::vector<uint8_t>& smallerFrac, std::vector<uint8_t>& resInt, std::vector<uint8_t>& resFrac);
    static int compareAbsolute(const std::vector<uint8_t>& aInt, const std::vector<uint8_t>& aFrac, const std::vector<uint8_t>& bInt, const std::vector<uint8_t>& bFrac);
    static bool isLessThan(const std::vector<uint8_t>& aInt, const std::vector<uint8_t>& aFrac, const std::vector<uint8_t>& bInt, const std::vector<uint8_t>& bFrac);
    Doubles floor() const;
    std::vector<uint8_t> addInteger(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const;
    void normalizeDivisionResult();
    static bool isLessThan(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);
    std::vector<uint8_t> convertToExtended(size_t target_digits) const;
    void subtractAbsolute(std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const;
    bool isZero() const;
    bool isInteger() const;
    int64_t toInt64() const;
};
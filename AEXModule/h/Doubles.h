#pragma once
#include <string>
#include <vector>
#include <iostream>

class Doubles {
public:
    Doubles() = default;
    /*
    * ���캯����Ĭ�Ͼ���Ϊ50λ
    * @param number �����ַ��������Դ�����
    * @param precision ���ȣ���С���������λ
    */
    Doubles(const std::string& number, size_t precision = 50);
    /*
    * ���þ���
    * @param precision ���ȣ���С���������λ
    */
    void setPrecision(size_t precision);
    /*
    * ��ȡ����
    * @return ����
    */
    size_t getPrecision() const;
    /*
    * ��������
    * @param number �����ַ��������Դ�����
    */
    void setNumber(const std::string& number);
    /*
    * ��ȡ����
    * @return �����ַ��������Դ�����
    */
    std::string getNumber() const;
    /*
    * �Ƿ�Ϊ����
    * @return trueΪ������falseΪ����
    */
    bool isNegative() const;
    /*
    * ��ƽ����
    * @return ƽ����
    */
    Doubles& sqrt();
    /*
    * �����ֵ
    * @return ����ֵ
    */
    Doubles abs() const;
    // ���������
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

    // �Ƚ������
    bool operator !=(const Doubles& other) const;
    bool operator==(const Doubles& other) const;
    bool operator<=(const Doubles& other) const;
    bool operator<(const Doubles& other) const;
    bool operator>=(const Doubles& other) const;
    bool operator>(const Doubles& other) const;
    // ��������
    friend std::ostream& operator<<(std::ostream& os, const Doubles& num);
private:
    bool is_negative = false;// �Ƿ�Ϊ����
    size_t precision = 50;      // ���ȣ���С���������λ
    std::vector<uint8_t> integer;    // ��˴洢������123��Ϊ[1,2,3]
    std::vector<uint8_t> fractional; // С�˴洢������0.45��Ϊ[4,5]
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
#include "../h/Doubles.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <iomanip>

Doubles::Doubles(const std::string& number, size_t precision) : precision(precision) {
    parseNumber(number, integer, fractional, is_negative);
    normalize(integer, fractional, precision);
}

void Doubles::setPrecision(size_t precision) {
    this->precision = precision;
    normalize(integer, fractional, precision);
}

size_t Doubles::getPrecision() const {
    return precision;
}

void Doubles::setNumber(const std::string& number) {
    parseNumber(number, integer, fractional, is_negative);
    normalize(integer, fractional, precision);
}

std::string Doubles::getNumber() const {
    std::stringstream ss;
    if (is_negative) ss << "-";
    if (integer.empty()) {
        ss << "0";
    }
    else {
        for (auto d : integer) {
            ss << static_cast<int>(d);
        }
    }
    size_t lastNonZero = 0;
    for (lastNonZero = fractional.size(); lastNonZero > 0; --lastNonZero) {
        if (fractional[lastNonZero - 1] != 0) break;
    }
    if (lastNonZero > 0) {
        ss << ".";
        for (size_t i = 0; i < lastNonZero; ++i) {
            ss << static_cast<int>(fractional[i]);
        }
    }
    else {
        ss << ".0";
    }
    return ss.str();
}

bool Doubles::isNegative() const {
    return is_negative;
}

Doubles& Doubles::sqrt() {
    if (is_negative)
        throw std::invalid_argument("sqrt of negative number");

    // 保存原始输入值
    const Doubles original_input = *this;

    if (original_input.isZero()) {
        integer = { 0 };
        fractional.assign(precision, 0);
        is_negative = false;
        return *this;
    }

    // 临时提升计算精度
    const size_t original_precision = this->precision;
    this->setPrecision(original_precision + 4);

    // 计算有效位数和科学计数法指数
    auto first_non_zero = std::find_if(integer.begin(), integer.end(),
        [](uint8_t d) { return d != 0; });
    const size_t significant_digits = (first_non_zero != integer.end()) ?
        integer.size() - std::distance(integer.begin(), first_non_zero) : 0;
    const size_t exponent = (significant_digits > 0) ? significant_digits - 1 : 0;
    const bool is_odd_exponent = (exponent % 2 != 0);
    const size_t half_power = exponent / 2;

    // 生成合法初始猜测值
    std::string initial_str;
    if (is_odd_exponent) {
        initial_str = "316227766017"; // sqrt(10)的近似值
        const size_t decimal_pos = 1 + half_power;
        if (decimal_pos <= initial_str.length()) {
            initial_str.insert(decimal_pos, ".");
        }
        else {
            initial_str.append(decimal_pos - initial_str.length(), '0');
            initial_str += ".0";
        }
        initial_str = initial_str.substr(0, 15); // 限制长度
    }
    else {
        initial_str = "1" + std::string(half_power, '0');
    }

    // 设置初始猜测值
    *this = Doubles(initial_str.c_str(), this->precision);

    // 牛顿迭代法（使用原始输入值）
    const Doubles two("2", this->precision);
    const int max_iter = 20;
    int iterations = 0;

    // 生成动态阈值（1e-precision）
    std::string threshold_str = "0.";
    for (size_t i = 0; i < original_precision - 1; ++i) {
        threshold_str += "0";
    }
    threshold_str += "1";
    const Doubles threshold(threshold_str, original_precision);

    do {
        Doubles prev = *this;
        *this = (prev + (original_input / prev)) / two;
        Doubles diff = (*this - prev).abs();

        if (++iterations >= max_iter || diff < threshold) {
            break;
        }
    } while (true);

    // 恢复精度并规范化
    this->setPrecision(original_precision);
    this->is_negative = false;
    this->normalize();

    return *this;
}

Doubles Doubles::abs() const {
    Doubles result;
    result.integer = integer;
    result.fractional = fractional;
    result.precision = precision;
    result.is_negative = false; // 强制为正
    result.normalize(); // 确保规范化
    return result;
}

Doubles& Doubles::operator=(const Doubles& other) {
    if (this != &other) {
        integer = other.integer;
        fractional = other.fractional;
        precision = other.precision;
        is_negative = other.is_negative;
    }
    return *this;
}

Doubles& Doubles::operator=(const std::string& other) {
    Doubles temp(other, precision);
    *this = temp;
    return *this;
}

Doubles& Doubles::operator=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Null pointer assignment");
    }
    *this = std::string(other);
    return *this;
}

Doubles& Doubles::operator=(float other) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << other;
    *this = ss.str();
    return *this;
}

Doubles& Doubles::operator=(double other) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(15) << other;
    *this = ss.str();
    return *this;
}

Doubles& Doubles::operator=(int other) {
    *this = static_cast<long long>(other);
    return *this;
}

Doubles& Doubles::operator=(unsigned int other) {
    *this = static_cast<unsigned long long>(other);
    return *this;
}

Doubles& Doubles::operator=(long long other) {
    std::stringstream ss;
    ss << other;
    *this = ss.str();
    return *this;
}

Doubles& Doubles::operator=(unsigned long long other) {
    std::stringstream ss;
    ss << other;
    *this = ss.str();
    return *this;
}

Doubles Doubles::operator+(const Doubles& other) const {
    std::vector<uint8_t> aInt = integer, aFrac = fractional;
    std::vector<uint8_t> bInt = other.integer, bFrac = other.fractional;
    alignDecimals(aInt, aFrac, bInt, bFrac);
    if (is_negative == other.is_negative) {
        std::vector<uint8_t> resInt, resFrac;
        addCore(aInt, aFrac, bInt, bFrac, resInt, resFrac);
        Doubles result;
        result.integer.swap(resInt);
        result.fractional.swap(resFrac);
        result.is_negative = is_negative;
        result.normalize(result.integer, result.fractional, std::max(precision, other.precision));
        return result;
    }

    const int cmp = compareAbsolute(aInt, aFrac, bInt, bFrac);
    if (cmp == 0) return Doubles("0", std::max(precision, other.precision));
    const bool aLarger = cmp > 0;
    std::vector<uint8_t> resInt, resFrac;
    if (aLarger) {
        subtractCore(aInt, aFrac, bInt, bFrac, resInt, resFrac);
    }
    else {
        subtractCore(bInt, bFrac, aInt, aFrac, resInt, resFrac);
    }
    Doubles result;
    result.integer.swap(resInt);
    result.fractional.swap(resFrac);
    result.is_negative = aLarger ? is_negative : other.is_negative;
    result.normalize(result.integer, result.fractional, std::max(precision, other.precision));
    return result;
}

Doubles Doubles::operator+(const std::string& other) const {
    if (other.empty()) {
        throw std::invalid_argument("Empty string operand");
    }
    return *this + Doubles(other, this->precision);
}

Doubles Doubles::operator+(const char* other) const {
    if (!other) {
        throw std::invalid_argument("Null pointer operand");
    }
    return *this + std::string(other);
}

Doubles& Doubles::operator+=(const Doubles& other) {
    *this = *this + other;
    return *this;
}

Doubles& Doubles::operator+=(const std::string& other) {
    *this = *this + Doubles(other, this->precision);
    return *this;
}

Doubles& Doubles::operator+=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Null pointer operand");
    }
    *this = *this + std::string(other);
    return *this;
}

Doubles Doubles::operator-(const Doubles& other) const {
    std::vector<uint8_t> aInt = integer;
    std::vector<uint8_t> aFrac = fractional;
    std::vector<uint8_t> bInt = other.integer;
    std::vector<uint8_t> bFrac = other.fractional;
    alignDecimals(aInt, aFrac, bInt, bFrac);
    if (is_negative != other.is_negative) {
        std::vector<uint8_t> resInt, resFrac;
        addCore(aInt, aFrac, bInt, bFrac, resInt, resFrac);
        Doubles result;
        result.integer = resInt;
        result.fractional = resFrac;
        result.is_negative = is_negative;
        result.normalize(result.integer, result.fractional, std::max(precision, other.precision));
        return result;
    }
    else {
        const int cmp = compareAbsolute(aInt, aFrac, bInt, bFrac);
        if (cmp == 0) return Doubles("0", std::max(precision, other.precision));
        const bool aLarger = (cmp > 0);
        const auto& largerInt = aLarger ? aInt : bInt;
        const auto& largerFrac = aLarger ? aFrac : bFrac;
        const auto& smallerInt = aLarger ? bInt : aInt;
        const auto& smallerFrac = aLarger ? bFrac : aFrac;
        std::vector<uint8_t> resInt, resFrac;
        subtractCore(largerInt, largerFrac, smallerInt, smallerFrac, resInt, resFrac);
        Doubles result;
        result.integer = resInt;
        result.fractional = resFrac;
        result.is_negative = aLarger ? is_negative : !is_negative;
        result.normalize(result.integer, result.fractional, std::max(precision, other.precision));
        return result;
    }
}

Doubles Doubles::operator-(const std::string& other) const {
    return *this - Doubles(other, this->precision);
}

Doubles Doubles::operator-(const char* other) const {
    if (!other) throw std::invalid_argument("Null pointer operand");
    return *this - std::string(other);
}

Doubles& Doubles::operator-=(const Doubles& other) {
    *this = *this - other;
    return *this;
}

Doubles& Doubles::operator-=(const std::string& other) {
    *this = *this - Doubles(other, this->precision);
    return *this;
}

Doubles& Doubles::operator-=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Subtraction with null pointer operand");
    }
    *this = *this - std::string(other);
    return *this;
}

Doubles Doubles::operator*(const Doubles& other) const {
    const size_t frac1_len = fractional.size();
    const size_t frac2_len = other.fractional.size();
    std::vector<uint8_t> num1 = integer;
    num1.insert(num1.end(), fractional.begin(), fractional.end());
    std::vector<uint8_t> num2 = other.integer;
    num2.insert(num2.end(), other.fractional.begin(), other.fractional.end());
    std::vector<uint8_t> product(num1.size() + num2.size(), 0);
    for (int i = num1.size() - 1; i >= 0; --i) {
        int carry = 0;
        for (int j = num2.size() - 1; j >= 0; --j) {
            int pos = i + j + 1;
            int sum = num1[i] * num2[j] + product[pos] + carry;
            product[pos] = sum % 10;
            carry = sum / 10;
        }
        product[i] += carry;
    }
    const size_t total_frac = frac1_len + frac2_len;
    int decimal_pos = static_cast<int>(product.size()) - static_cast<int>(total_frac);
    std::vector<uint8_t> res_int, res_frac;
    if (decimal_pos > 0) {
        res_int = std::vector<uint8_t>(product.begin(), product.begin() + decimal_pos);
        res_frac = std::vector<uint8_t>(product.begin() + decimal_pos, product.end());
    }
    else {
        res_int = { 0 };
        res_frac.resize(total_frac, 0);
        std::copy(product.begin(), product.end(), res_frac.begin() - decimal_pos);
    }
    auto first_non_zero = std::find_if(res_int.begin(), res_int.end(), [](uint8_t d) { return d != 0; });
    if (first_non_zero != res_int.end()) {
        res_int.erase(res_int.begin(), first_non_zero);
    }
    else {
        res_int = { 0 };
    }
    Doubles result;
    result.integer = res_int;
    result.fractional = res_frac;
    result.is_negative = is_negative ^ other.is_negative;
    result.precision = std::max(precision, other.precision);
    if (result.fractional.size() > result.precision) {
        result.fractional.resize(result.precision);
    }
    else {
        result.fractional.insert(result.fractional.end(),
            result.precision - result.fractional.size(), 0);
    }
    return result;
}

Doubles Doubles::operator*(const std::string& other) const {
    return *this * Doubles(other, this->precision);
}

Doubles Doubles::operator*(const char* other) const {
    if (!other) {
        throw std::invalid_argument("Multiplication by null pointer");
    }
    return *this * std::string(other);
}

Doubles& Doubles::operator*=(const Doubles& other) {
    *this = *this * other;
    return *this;
}

Doubles& Doubles::operator*=(const std::string& other) {
    *this = *this * Doubles(other, this->precision);
    return *this;
}

Doubles& Doubles::operator*=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Multiplication with null pointer operand");
    }
    *this = *this * std::string(other);
    return *this;
}

Doubles Doubles::operator/(const Doubles& other) const {
    if (other.isZero()) throw std::invalid_argument("Division by zero");
    const size_t extend_digits = 2 * std::max(precision, other.precision);
    std::vector<uint8_t> dividend = convertToExtended(extend_digits);
    std::vector<uint8_t> divisor = other.convertToExtended(extend_digits);
    auto divisor_start = std::find_if(divisor.begin(), divisor.end(), [](uint8_t d) { return d != 0; });
    if (divisor_start == divisor.end()) {
        throw std::logic_error("Divisor became zero after normalization");
    }
    divisor = std::vector<uint8_t>(divisor_start, divisor.end());
    std::vector<uint8_t> quotient;
    std::vector<uint8_t> remainder(dividend.begin(), dividend.end());
    size_t loop_counter = 0;
    const size_t max_loops = precision + 10;
    while (loop_counter++ < max_loops) {
        uint8_t q = 0;
        while (!isLessThan(remainder, divisor)) {
            subtractAbsolute(remainder, divisor);
            if (++q == 10) break;
        }
        quotient.push_back(q);
        if (std::all_of(remainder.begin(), remainder.end(), [](uint8_t d) { return d == 0; }) &&
            quotient.size() >= precision) {
            break;
        }
        remainder.push_back(0);
        auto first_non_zero = std::find_if(remainder.begin(), remainder.end(), [](uint8_t d) { return d != 0; });
        if (first_non_zero != remainder.begin()) {
            remainder.erase(remainder.begin(), first_non_zero);
        }
    };
    // 修改关键部分：decimal_pos计算与商的前导零填充
    const int decimal_shift = integer.size() - other.integer.size();
    size_t decimal_pos = (decimal_shift >= 0) ? (decimal_shift + 1) : 0;

    // 确保商足够长以正确分割
    if (quotient.size() < decimal_pos) {
        quotient.insert(quotient.begin(), decimal_pos - quotient.size(), 0);
    }

    std::vector<uint8_t> res_int, res_frac;
    if (decimal_pos > 0 && decimal_pos <= quotient.size()) {
        res_int = std::vector<uint8_t>(quotient.begin(), quotient.begin() + decimal_pos);
        res_frac = std::vector<uint8_t>(quotient.begin() + decimal_pos, quotient.end());
    }
    else {
        res_int = { 0 };
        res_frac = quotient;
    }

    Doubles result;
    result.integer = res_int;
    result.fractional = res_frac;
    result.is_negative = is_negative ^ other.is_negative;
    result.precision = precision;
    result.normalizeDivisionResult();
    return result;
}

std::vector<uint8_t> Doubles::convertToExtended(size_t target_digits) const {
    std::vector<uint8_t> extended = integer;
    extended.insert(extended.end(), fractional.begin(), fractional.end());
    if (extended.size() < target_digits) {
        extended.insert(extended.end(), target_digits - extended.size(), 0);
    }
    return extended;
}

void Doubles::subtractAbsolute(std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const {
    int borrow = 0;
    const size_t a_size = a.size();
    const size_t b_size = b.size();

    for (int i = a_size - 1; i >= 0; --i) {
        const int b_idx = i - (a_size - b_size);
        const uint8_t b_digit = (b_idx >= 0 && b_idx < static_cast<int>(b_size)) ? b[b_idx] : 0;

        int sub = a[i] - b_digit - borrow;
        if (sub < 0) {
            sub += 10;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        a[i] = static_cast<uint8_t>(sub);
    }

    if (borrow != 0) {
        throw std::runtime_error("Subtraction underflow");
    }
}

void Doubles::normalizeDivisionResult() {
    auto first_non_zero = std::find_if(integer.begin(), integer.end(), [](uint8_t d) { return d != 0; });
    if (first_non_zero != integer.end()) {
        integer.erase(integer.begin(), first_non_zero);
    }
    else {
        integer = { 0 };
    }

    if (fractional.size() > precision) {
        fractional.resize(precision);
    }
}

bool Doubles::isZero() const
{
    return std::all_of(integer.begin(), integer.end(), [](uint8_t d) { return d == 0; }) &&
        std::all_of(fractional.begin(), fractional.end(), [](uint8_t d) { return d == 0; });
}

bool Doubles::isLessThan(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b)
{
    auto a_begin = std::find_if(a.begin(), a.end(), [](uint8_t d) { return d != 0; });
    auto b_begin = std::find_if(b.begin(), b.end(), [](uint8_t d) { return d != 0; });

    const size_t a_len = a.end() - a_begin;
    const size_t b_len = b.end() - b_begin;

    if (a_len != b_len) return a_len < b_len;

    while (a_begin != a.end() && b_begin != b.end()) {
        if (*a_begin != *b_begin) {
            return *a_begin < *b_begin;
        }
        ++a_begin;
        ++b_begin;
    }
    return false;
}

Doubles Doubles::operator/(const std::string& other) const {
    return *this / Doubles(other, this->precision);
}

Doubles Doubles::operator/(const char* other) const {
    if (!other) {
        throw std::invalid_argument("Division by null string");
    }
    return *this / std::string(other);
}

Doubles& Doubles::operator/=(const Doubles& other) {
    *this = *this / other;
    return *this;
}

Doubles& Doubles::operator/=(const std::string& other) {
    *this = *this / Doubles(other, this->precision);
    return *this;
}

Doubles& Doubles::operator/=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Division by null pointer operand");
    }
    *this = *this / std::string(other);
    return *this;
}

Doubles Doubles::operator%(const Doubles& other) const {
    if (other.isZero()) {
        throw std::invalid_argument("Modulo by zero");
    }

    Doubles quotient = (*this / other).floor();

    Doubles remainder = *this - (other * quotient);

    remainder.setPrecision(precision);
    remainder.normalize();
    return remainder;
}

Doubles Doubles::operator%(const std::string& other) const {
    return *this % Doubles(other, this->precision);
}

Doubles Doubles::operator%(const char* other) const {
    if (!other) {
        throw std::invalid_argument("Modulo by null pointer");
    }
    return *this % std::string(other);
}

Doubles& Doubles::operator%=(const Doubles& other) {
    *this = *this % other;
    return *this;
}

Doubles& Doubles::operator%=(const std::string& other) {
    *this = *this % Doubles(other, this->precision);
    return *this;
}

Doubles& Doubles::operator%=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Modulo by null pointer");
    }
    *this = *this % std::string(other);
    return *this;
}

Doubles Doubles::operator&(const Doubles& other) const {
    if (!isInteger() || !other.isInteger()) {
        throw std::invalid_argument("Bitwise AND requires integer values");
    }
    int64_t this_val = toInt64();
    int64_t other_val = other.toInt64();
    int64_t result = this_val & other_val;
    return Doubles(std::to_string(result), precision);
}

Doubles& Doubles::operator&=(const Doubles& other) {
    *this = *this & other;
    return *this;
}

bool Doubles::isInteger() const {
    return std::all_of(fractional.begin(), fractional.end(), [](uint8_t d) { return d == 0; });
}

int64_t Doubles::toInt64() const {
    int64_t value = 0;
    for (auto d : integer) {
        if (value > (INT64_MAX - d) / 10) {
            throw std::overflow_error("Integer value too large");
        }
        value = value * 10 + d;
    }
    return is_negative ? -value : value;
}

Doubles Doubles::operator&(const std::string& other) const {
    return *this & Doubles(other, precision);
}

Doubles Doubles::operator&(const char* other) const {
    if (!other) {
        throw std::invalid_argument("Null pointer operand");
    }
    return *this & std::string(other);
}

Doubles& Doubles::operator&=(const std::string& other) {
    *this = *this & Doubles(other, precision);
    return *this;
}

Doubles& Doubles::operator&=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Null pointer operand");
    }
    *this = *this & std::string(other);
    return *this;
}

Doubles Doubles::operator|(const Doubles& other) const {
    if (!isInteger() || !other.isInteger()) {
        throw std::invalid_argument("Bitwise OR requires integer values");
    }
    int64_t this_val = toInt64();
    int64_t other_val = other.toInt64();
    int64_t result = this_val | other_val;
    return Doubles(std::to_string(result), precision);
}

Doubles& Doubles::operator|=(const Doubles& other) {
    *this = *this | other;
    return *this;
}

Doubles Doubles::operator|(const std::string& other) const {
    return *this | Doubles(other, precision);
}

Doubles Doubles::operator|(const char* other) const {
    if (!other) {
        throw std::invalid_argument("Null pointer operand");
    }
    return *this | std::string(other);
}

Doubles& Doubles::operator|=(const std::string& other) {
    *this = *this | Doubles(other, precision);
    return *this;
}

Doubles& Doubles::operator|=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Null pointer operand");
    }
    *this = *this | std::string(other);
    return *this;
}

Doubles Doubles::operator^(const Doubles& other) const {
    if (!isInteger() || !other.isInteger()) {
        throw std::invalid_argument("Bitwise XOR requires integer values");
    }
    int64_t this_val = toInt64();
    int64_t other_val = other.toInt64();
    int64_t result = this_val ^ other_val;
    return Doubles(std::to_string(result), precision);
}

Doubles& Doubles::operator^=(const Doubles& other) {
    *this = *this ^ other;
    return *this;
}

Doubles Doubles::operator^(const std::string& other) const {
    return *this ^ Doubles(other, precision);
}

Doubles Doubles::operator^(const char* other) const {
    if (!other) {
        throw std::invalid_argument("Null pointer operand");
    }
    return *this ^ std::string(other);
}

Doubles& Doubles::operator^=(const std::string& other) {
    *this = *this ^ Doubles(other, precision);
    return *this;
}

Doubles& Doubles::operator^=(const char* other) {
    if (!other) {
        throw std::invalid_argument("Null pointer operand");
    }
    *this = *this ^ std::string(other);
    return *this;
}

Doubles Doubles::floor() const {
    Doubles result = *this;
    if (!is_negative) {
        result.fractional.assign(precision, 0);
        return result;
    }
    if (std::any_of(fractional.begin(), fractional.end(), [](uint8_t d) { return d != 0; })) {
        std::vector<uint8_t> one{ 1 };
        result.integer = addInteger(result.integer, one);
    }
    result.fractional.assign(precision, 0);
    return result;
}

std::vector<uint8_t> Doubles::addInteger(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const {
    std::vector<uint8_t> result;
    int carry = 0;
    size_t i = 0;

    while (i < a.size() || i < b.size() || carry) {
        int sum = carry;
        if (i < a.size()) sum += a[a.size() - 1 - i];
        if (i < b.size()) sum += b[b.size() - 1 - i];
        result.insert(result.begin(), sum % 10);
        carry = sum / 10;
        ++i;
    }

    return result;
}

static void alignNumbers(std::vector<uint8_t>& aInt, std::vector<uint8_t>& aFrac,
    std::vector<uint8_t>& bInt, std::vector<uint8_t>& bFrac) {
    size_t maxInt = std::max(aInt.size(), bInt.size());
    if (aInt.size() < maxInt) {
        aInt.insert(aInt.begin(), maxInt - aInt.size(), 0);
    }
    else {
        bInt.insert(bInt.begin(), maxInt - bInt.size(), 0);
    }
    size_t maxFrac = std::max(aFrac.size(), bFrac.size());
    if (aFrac.size() < maxFrac) {
        aFrac.resize(maxFrac, 0);
    }
    else {
        bFrac.resize(maxFrac, 0);
    }
}


bool Doubles::isLessThan(const std::vector<uint8_t>& aInt, const std::vector<uint8_t>& aFrac,
    const std::vector<uint8_t>& bInt, const std::vector<uint8_t>& bFrac) {
    if (aInt.size() != bInt.size()) {
        return aInt.size() < bInt.size();
    }
    for (size_t i = 0; i < aInt.size(); ++i) {
        if (aInt[i] != bInt[i]) {
            return aInt[i] < bInt[i];
        }
    }
    for (size_t i = 0; i < std::min(aFrac.size(), bFrac.size()); ++i) {
        if (aFrac[i] != bFrac[i]) {
            return aFrac[i] < bFrac[i];
        }
    }
    return aFrac.size() < bFrac.size();
}

bool Doubles::operator!=(const Doubles& other) const {
    return !(*this == other);
}

bool Doubles::operator==(const Doubles& other) const {
    if (is_negative != other.is_negative) {
        return false;
    }
    if (integer != other.integer) {
        return false;
    }
    size_t max_frac = std::max(fractional.size(), other.fractional.size());
    std::vector<uint8_t> this_frac = fractional;
    std::vector<uint8_t> other_frac = other.fractional;
    this_frac.resize(max_frac, 0);
    other_frac.resize(max_frac, 0);
    return this_frac == other_frac;
}

bool Doubles::operator<=(const Doubles& other) const
{
    if (*this == other) {
        return true;
    }
    else {
        return *this < other;
    }
}

bool Doubles::operator<(const Doubles& other) const {
    if (is_negative != other.is_negative) {
        return is_negative;
    }

    const bool absLess = isLessThan(integer, fractional, other.integer, other.fractional);
    return is_negative ? !absLess : absLess;
}

bool Doubles::operator>=(const Doubles& other) const {
    if (*this == other) {
        return true;
    }
    else {
        return *this > other;
    }
}

bool Doubles::operator>(const Doubles& other) const {
    if (is_negative != other.is_negative) {
        return !is_negative;
    }

    const bool abs_less = isLessThan(integer, fractional, other.integer, other.fractional);
    const bool abs_greater = !abs_less &&
        !isLessThan(other.integer, other.fractional, integer, fractional);
    return is_negative ? !abs_greater : abs_greater;
}

void Doubles::parseNumber(const std::string& num, std::vector<uint8_t>& integer, std::vector<uint8_t>& fractional, bool& is_negative)
{
    is_negative = false;
    size_t start = 0;

    if (!num.empty()) {
        if (num[0] == '-') {
            is_negative = true;
            start = 1;
        }
        else if (num[0] == '+') {
            start = 1;
        }
    }
    size_t dotPos = num.find('.', start);
    std::string intPart = num.substr(start, (dotPos != std::string::npos) ? (dotPos - start) : std::string::npos);
    std::string fracPart = (dotPos != std::string::npos) ? num.substr(dotPos + 1) : "";
    for (char c : intPart) {
        if (!std::isdigit(c)) throw std::invalid_argument("Invalid number format");
        integer.push_back(c - '0');
    }
    for (char c : fracPart) {
        if (!std::isdigit(c)) throw std::invalid_argument("Invalid number format");
        fractional.push_back(c - '0');
    }
    if (integer.empty()) integer.push_back(0);
}

void Doubles::normalize(std::vector<uint8_t>& integer, std::vector<uint8_t>& fractional, size_t precision)
{
    auto firstNonZero = std::find_if(integer.begin(), integer.end(), [](uint8_t d) { return d != 0; });
    integer.erase(integer.begin(), firstNonZero);
    if (integer.empty()) integer.push_back(0);
    if (fractional.size() > precision) {
        fractional.resize(precision);
    }
    else {
        fractional.insert(fractional.end(), precision - fractional.size(), 0);
    }
}

void Doubles::normalize() {
    auto firstNonZero = std::find_if(integer.begin(), integer.end(), [](uint8_t d) { return d != 0; });
    if (firstNonZero == integer.end()) {
        integer = { 0 };
    }
    else {
        integer.erase(integer.begin(), firstNonZero);
    }
    if (fractional.size() > precision) {
        fractional.resize(precision);
    }
    else {
        fractional.insert(fractional.end(), precision - fractional.size(), 0);
    }
}

void Doubles::alignDecimals(std::vector<uint8_t>& aInt, std::vector<uint8_t>& aFrac, std::vector<uint8_t>& bInt, std::vector<uint8_t>& bFrac)
{
    const size_t maxInt = std::max(aInt.size(), bInt.size());
    aInt.insert(aInt.begin(), maxInt - aInt.size(), 0);
    bInt.insert(bInt.begin(), maxInt - bInt.size(), 0);
    const size_t maxFrac = std::max(aFrac.size(), bFrac.size());
    aFrac.resize(maxFrac, 0);
    bFrac.resize(maxFrac, 0);
}

void Doubles::addCore(const std::vector<uint8_t>& aInt, const std::vector<uint8_t>& aFrac, const std::vector<uint8_t>& bInt, const std::vector<uint8_t>& bFrac, std::vector<uint8_t>& resInt, std::vector<uint8_t>& resFrac)
{
    resInt.clear();
    resFrac.clear();
    int carry = 0;
    for (int i = aFrac.size() - 1; i >= 0; --i) {
        int sum = aFrac[i] + bFrac[i] + carry;
        resFrac.insert(resFrac.begin(), sum % 10);
        carry = sum / 10;
    }
    size_t maxLen = std::max(aInt.size(), bInt.size());
    for (size_t i = 0; i < maxLen || carry > 0; ++i) {
        int a = (i < aInt.size()) ? aInt[aInt.size() - 1 - i] : 0;
        int b = (i < bInt.size()) ? bInt[bInt.size() - 1 - i] : 0;
        int sum = a + b + carry;
        resInt.insert(resInt.begin(), sum % 10);
        carry = sum / 10;
    }
    if (resInt.size() > 1) {
        auto firstNonZero = std::find_if(resInt.begin(), resInt.end(), [](uint8_t d) { return d != 0; });
        if (firstNonZero != resInt.end() && firstNonZero != resInt.begin()) {
            resInt.erase(resInt.begin(), firstNonZero);
        }
    }
}

void Doubles::subtractCore(const std::vector<uint8_t>& largerInt, const std::vector<uint8_t>& largerFrac, const std::vector<uint8_t>& smallerInt, const std::vector<uint8_t>& smallerFrac, std::vector<uint8_t>& resInt, std::vector<uint8_t>& resFrac)
{
    resInt.clear();
    resFrac.clear();
    int borrow = 0;
    for (int i = largerFrac.size() - 1; i >= 0; --i) {
        int sub = largerFrac[i] - borrow;
        if (i < static_cast<int>(smallerFrac.size())) {
            sub -= smallerFrac[i];
        }
        borrow = sub < 0 ? 1 : 0;
        resFrac.insert(resFrac.begin(), (sub + 10) % 10);
    }
    for (int i = largerInt.size() - 1; i >= 0; --i) {
        int sub = largerInt[i] - borrow;
        if (i < static_cast<int>(smallerInt.size())) {
            sub -= smallerInt[i];
        }
        borrow = sub < 0 ? 1 : 0;
        resInt.insert(resInt.begin(), (sub + 10) % 10);
    }
    auto firstNonZero = std::find_if(resInt.begin(), resInt.end(), [](uint8_t d) { return d != 0; });
    if (firstNonZero == resInt.end()) {
        resInt = { 0 };
    }
    else {
        resInt.erase(resInt.begin(), firstNonZero);
    }
}

int Doubles::compareAbsolute(const std::vector<uint8_t>& aInt, const std::vector<uint8_t>& aFrac, const std::vector<uint8_t>& bInt, const std::vector<uint8_t>& bFrac)
{
    if (aInt.size() != bInt.size()) {
        return aInt.size() > bInt.size() ? 1 : -1;
    }
    for (size_t i = 0; i < aInt.size(); ++i) {
        if (aInt[i] != bInt[i]) {
            return aInt[i] > bInt[i] ? 1 : -1;
        }
    }
    size_t minFrac = std::min(aFrac.size(), bFrac.size());
    for (size_t i = 0; i < minFrac; ++i) {
        if (aFrac[i] != bFrac[i]) {
            return aFrac[i] > bFrac[i] ? 1 : -1;
        }
    }
    if (aFrac.size() != bFrac.size()) {
        return aFrac.size() > bFrac.size() ? 1 : -1;
    }
    return 0;
}

std::ostream& operator<<(std::ostream& os, const Doubles& num) {
    os << num.getNumber();
    return os;
}


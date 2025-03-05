#include "../h/Byteset.h"
#include <cstddef>
#include <algorithm>
#include <unordered_set>
Byteset::~Byteset()
{
    this->data_.clear();
}
Byteset::Byteset(const std::string& str)
{
   this->data_.resize(str.size());
    std::memcpy(this->data_.data(), str.data(), str.size());
}

Byteset::Byteset(const void* bytes, long long size)
{
    const char* byteData = static_cast<const char*>(bytes);
    data_.assign(byteData, byteData + size);
}

Byteset::Byteset(const char* bytes)
{
    if (bytes == nullptr) return;
    this->data_.assign(bytes, bytes + std::strlen(bytes));
}

Byteset::Byteset(int num)
{
    this->data_.resize(sizeof(int));
    std::memcpy(this->data_.data(), &num, sizeof(int));
}

Byteset::Byteset(long num)
{
    this->data_.resize(sizeof(long));
    std::memcpy(this->data_.data(), &num, sizeof(long));
}

Byteset::Byteset(long long num)
{
    this->data_.resize(sizeof(long long));
    std::memcpy(this->data_.data(), &num, sizeof(long long));
}

Byteset::Byteset(size_t num)
{
    this->data_.resize(sizeof(size_t));
    std::memcpy(this->data_.data(), &num, sizeof(size_t));
}

Byteset::Byteset(float num)
{
    this->data_.resize(sizeof(float));
    std::memcpy(this->data_.data(), &num, sizeof(float));
}

Byteset::Byteset(double num)
{
    this->data_.resize(sizeof(double));
    std::memcpy(this->data_.data(), &num, sizeof(double));
}

Byteset::Byteset(long double num)
{
    this->data_.resize(sizeof(long double));
    std::memcpy(this->data_.data(), &num, sizeof(long double));
}

Byteset::Byteset(bool flag)
{
    this->data_.resize(sizeof(bool));
    std::memcpy(this->data_.data(), &flag, sizeof(bool));
}

Byteset::Byteset(char ch)
{
    this->data_.resize(sizeof(char));
    std::memcpy(this->data_.data(), &ch, sizeof(char));
}

Byteset::Byteset(short num)
{
    this->data_.resize(sizeof(short));
    std::memcpy(this->data_.data(), &num, sizeof(short));
}

Byteset::Byteset(const Byteset& other)
{
    this->data_ = other.data_;
}

Byteset::Byteset(Byteset&& other) noexcept
{
    this->data_ = std::move(other.data_);
}

bool Byteset::operator==(const Byteset& other) const
{
    return this->data_ == other.data_;
}

bool Byteset::operator!=(const Byteset& other) const
{
    return this->data_!= other.data_;
}

Byteset& Byteset::operator=(const Byteset& other)
{
    if (this != &other) {
        this->data_ = other.data_;
    }
    return *this;
}

Byteset& Byteset::operator+=(const Byteset& other)
{
    this->data_.insert(this->data_.end(), other.data_.begin(), other.data_.end());
    return *this;
}

Byteset& Byteset::operator=(const std::string& str)
{
    this->data_.resize(str.size());
    std::memcpy(this->data_.data(), str.data(), str.size());
    return *this;
}

Byteset& Byteset::operator=(const char* bytes)
{
    this->data_.assign(bytes, bytes + std::strlen(bytes));
    return *this;
}

Byteset& Byteset::operator=(std::initializer_list<unsigned char> init_list)
{
    this->data_ = init_list;
    return *this;
}

Byteset::Byteset(std::initializer_list<unsigned char> init_list) : data_(init_list) { }

Byteset& Byteset::append(const void* bytes, long long size) {
    const char* byteData = static_cast<const char*>(bytes);
    this->data_.insert(this->data_.end(), byteData, byteData + size);
    return *this;
}

Byteset& Byteset::append(const Byteset& other)
{
    if (other == nullptr) return *this;
    this->data_.insert(this->data_.end(), other.data_.begin(), other.data_.end());
    return *this;
}

void Byteset::push_back(unsigned char byte)
{
    this->data_.push_back(byte);
}

unsigned char Byteset::pop_back()
{
    return this->data_.back();
}

unsigned char& Byteset::operator[](long long index)
{
    return this->data_[index];
}

const unsigned char& Byteset::operator[](long long index) const
{
    return this->data_[index];
}


long long Byteset::size() const
{
    return this->data_.size();
}

void Byteset::resize(long long size)
{
    this->data_.resize(size);
}

bool Byteset::empty() const
{
    return this->data_.empty();
}

void Byteset::clear()
{
    this->data_.clear();
}

const unsigned char* Byteset::data(long long index) const
{
    return this->data_.data() + index;
}

long long Byteset::find(const Byteset& other, long long pos, bool vague, const Byteset& vagueContent) const
{
    if (pos >= data_.size()) {
        return -1;
    }
    if (other.size() == 0) {
        return pos;
    }
    if (!vague) {
        auto it = std::search(data_.begin() + pos, data_.end(),
            other.data_.begin(), other.data_.end());
        if (it != data_.end()) {
            return std::distance(data_.begin(), it);
        }
        return -1;
    }
    else {
        long long otherSize = other.size();
        if (pos > data_.size() - otherSize) {
            return -1;
        }
        std::unordered_set<unsigned char> vagueSet(vagueContent.data_.begin(), vagueContent.data_.end());

        for (long long i = pos; i <= data_.size() - otherSize; ++i) {
            const unsigned char* textPtr = &data_[i];
            const unsigned char* otherPtr = other.data_.data();
            long long j = 0;

            while (j < otherSize) {
                if (vagueSet.find(otherPtr[j]) != vagueSet.end()) {
                    ++j;
                }
                else if (textPtr[j] != otherPtr[j]) {
                    break;
                }
                else {
                    ++j;
                }

                if (j == otherSize) {
                    return i;
                }
            }
        }
        return -1;
    }
}


long long Byteset::replace(const Byteset& old, const Byteset& new_str, int count, long long pos)
{
    if (pos >= this->data_.size()) {
        return 0;
    }

    long long replaced = 0;
    long long old_size = old.size();
    long long new_size = new_str.size();

    while (count != 0 && (pos = this->find(old, pos, false)) != -1) {
        this->data_.erase(this->data_.begin() + pos, this->data_.begin() + pos + old_size);
        this->data_.insert(this->data_.begin() + pos, new_str.data_.begin(), new_str.data_.end());
        pos += new_size;
        ++replaced;
        --count;
    }

    return replaced;
}

long long Byteset::remove(const Byteset& other, long long pos, int count)
{
    if (pos >= this->data_.size()) {
        return 0;
    }

    long long removed = 0;
    long long other_size = other.size();

    while (count != 0 && (pos = this->find(other, pos, false)) != -1) {
        this->data_.erase(this->data_.begin() + pos, this->data_.begin() + pos + other_size);
        ++removed;
        --count;
    }

    return removed;
}

Byteset Byteset::subBytes(long long pos, long long len) const
{
    if (pos >= this->data_.size()) {
        return Byteset();
    }

    if (pos + len > this->data_.size()) {
        len = this->data_.size() - pos;
    }
    return Byteset(this->data_.data() + pos, len);
}

std::vector<unsigned char>::iterator Byteset::begin()
{
    return data_.begin();
}

std::vector<unsigned char>::const_iterator Byteset::begin() const
{
    return data_.begin();
}

std::vector<unsigned char>::iterator Byteset::end()
{
    return data_.end();
}

std::vector<unsigned char>::const_iterator Byteset::end() const
{
    return data_.end();
}

std::vector<unsigned char>::reverse_iterator Byteset::rbegin()
{
    return data_.rbegin();
}

std::vector<unsigned char>::const_reverse_iterator Byteset::rbegin() const
{
    return data_.rbegin();
}

std::vector<unsigned char>::reverse_iterator Byteset::rend()
{
    return data_.rend();
}

std::vector<unsigned char>::const_reverse_iterator Byteset::rend() const
{
    return data_.rend();
}

std::ostream& operator<<(std::ostream& os, const Byteset& byteset) {
    os << "{ ";
    for (long long i = 0; i < byteset.size(); ++i) {
        os << static_cast<unsigned int>(byteset[i]);
        if (i < byteset.size() - 1) {
            os << ", ";
        }
    }
    os << " }";
    return os;
}
#pragma once
#include <iostream>
#include <cstring>
struct packet_t
{
    packet_t() : id(0), length(0)
    {
        memset(data, 0, 8);
    }
    uint16_t id;
    uint8_t data[8];
    uint8_t length;
};
template <typename T>

class MyClass
{
public:
    MyClass(T value) : value_(value) {}

    T getValue() const
    {
        return value_;
    }

    void setValue(T value)
    {
        value_ = value;
    }
    void printValue() const
    {
        std::cout << "Value: " << value_ << std::endl;
    }

private:
    T value_; // ← これを追加
};
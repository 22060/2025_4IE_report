#include "class.hpp"
#include <iostream>
#include "mortor.hpp"

Mortors motor[8] = {
    Mortors(0, false),
    Mortors(1, true),
    Mortors(2, false),
    Mortors(3, true),
    Mortors(4, false),
    Mortors(5, true),
    Mortors(6, false),
    Mortors(7, true)};
MyClass<Mortors *> myClass(motor); // ← 修正

int main()
{
    std::cout << "Hello, World!" << std::endl;
    myClass.printValue();
    myClass.setValue(&motor[1]);
    myClass.printValue();
}
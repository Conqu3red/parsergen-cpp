#include <iostream>
#include <string>
#include <memory>
using namespace std;

class Base {
public:
    virtual string class_name() { return "Base"; }
    Base() {}
};

class Derived : public Base {
public:
    virtual string class_name() { return "Derived"; }
    Derived() : Base() {}
};

void f(shared_ptr<Base> item) {
    if (item->class_name() == "Derived")
        f(dynamic_pointer_cast<Derived>(item));
    cout << "f<Base>\n";
};

void f(shared_ptr<Derived> item) {
    cout << "f<Derived>\n";
};


int main()
{
    auto p = make_shared<Derived>();
    f(shared_ptr<Base>(p));
    return 0;
}
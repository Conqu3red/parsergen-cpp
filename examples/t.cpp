#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "decorators.hpp"

using namespace std;

template <typename T>
auto memoize(T func)
{
    auto r_func = 
    [=](auto ...args)
    {
        auto result = func(args...);
        std::cout << "Result: " << result << std::endl;
        return result;
    };

    return r_func;
}

class base {
public:
    string val;
    base(string val) : val(val) {}
    virtual ~base(){}
};

class item : public base {
public:
    string val;
    int num;
    item(int num) : base("item") {}
};

class base_list : public base {
public:
    vector<unique_ptr<base>> parts;

    base_list() : base("base_list") {}
    void print(){
        for (auto &element : parts){
            cout << element->val << endl;
        }
    }
};

class apples : public enable_memberfunc_traits<apples> {
private:
    // member function that throws
    double calculate_cost_impl(int count, double weight) {
        if(count <= 0)
            throw std::runtime_error("must have 1 or more apples");
        
        if(weight <= 0)
            throw std::runtime_error("apples must weigh more than 0 ounces");

        return count*weight*cost_per_apple;
    }

    double cost_per_apple;

public:
    // ctor
    apples(double cost_per_apple)
        : cost_per_apple(cost_per_apple)
        , calculate_cost(this)
    { 
        // decorate our member function in ctor
        this->calculate_cost = log_time(output(exception_fail_safe(classmethod(&apples::calculate_cost_impl))));
    }

    ~apples() { }

    // define a functor with the same signature as our member function
    memberfunc<decltype(&apples::calculate_cost_impl)> calculate_cost;
};

int main(){
    base_list list;
    item e1(1);
    item e2(2);
    list.parts.push_back(make_unique<item>(e1));
    list.parts.push_back(make_unique<item>(e2));
    
    list.print();
    item *_e1_ = nullptr;
    if (_e1_ = dynamic_cast<item *>(list.parts[0].get())){
        std::cout << _e1_->num;
    }

    apples a(5);
    std::cout << a.calculate_cost(5, 5);

    return 0;
}
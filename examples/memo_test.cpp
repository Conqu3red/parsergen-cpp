#include <functional>
#include <iostream>
#include <map>
#include <tuple>
#include <memory>
#include <chrono>
#include <any>

/*
template <typename R, typename... Args>
std::function<R (Args...)> memoize(R (*fn)(Args...)) {
    // memoize non member function
    std::map<std::tuple<Args...>, R> table;
    return [fn, table](Args... args) mutable -> R {
        auto argt = std::make_tuple(args...);
        auto memoized = table.find(argt);
        if(memoized == table.end()) {
            auto result = fn(args...);
            table[argt] = result;
            return result;
        } else {
            return memoized->second;
        }
    };
}

#define MEMOIZE(f)                                      \
template<typename... Args>                              \
inline auto f(Args... args){                            \
    static auto memoized = memoize(_ ##f);              \
    return memoized(args...);                           \
}

#define MEMOIZE_INLINE(f) auto f = memoize(_ ##f);

*/
template <typename R, typename... Args>
struct s_memoize {
    R (*fn)(Args...);
    std::map<std::tuple<Args...>, R> table;
    
    s_memoize(R (*fn)(Args...)) : fn(fn){}

    R operator()(Args... args){
        auto argt = std::make_tuple(args...);
        auto memoized = table.find(argt);
        if(memoized == table.end()) {
            auto result = fn(args...);
            table[argt] = result;
            return result;
        }
        return memoized->second;
    }
};
/*


template <typename R, typename C, typename... Args>
auto memoize_class_func(R (C::*fn)(Args...)) {
    // memoize member function
    std::map<std::tuple<Args...>, R> table;
    return [fn, table](C *instance, Args... args) mutable -> R {
        auto argt = std::make_tuple(args...);
        auto memoized = table.find(argt);
        if(memoized == table.end()) {
            auto result = (instance->*fn)(args...);
            table[argt] = result;
            return result;
        } else {
            return memoized->second;
        }
    };
}

#define MEMOIZE_CLASS(f)                                \
template<typename... Args>                              \
auto f(Args... args){                                   \
    static auto memoized = memoize_class_func(_ ##f);   \
    return memoized(this, args...);                     \
}
*/





// memoize function named _ <value of f>
// if the function is in a global scope, then you have
// to declare the signature of the function before calling
// the macro.
#define MEMOIZE(f)                                      \
template<typename... Args>                              \
auto f(Args... args){                                   \
    using R = decltype(_ ## f (args...));               \
    static std::map<std::tuple<Args...>, R> table;      \
    auto argt = std::make_tuple(args...);               \
    auto memoized = table.find(argt);                   \
    if (memoized == table.end()) {                      \
        auto result = _ ## f(args...);                  \
        table[argt] = result;                           \
        return result;                                  \
    }                                                   \
    return memoized->second;                            \
}

#define PASTE(a, b) a ## b
#define PASTE3(a, b, c) a ## b ## c
#define MEMOIZE_ON_POS(f)                               \
template<typename R, typename... Args>\
static std::map<std::tuple<Args...>, R> PASTE3(__, f, _table);      \
template<typename... Args>                              \
auto f(Args... args){                                   \
    using R = decltype(_ ## f (args...));               \
    auto pos = mark();                                  \
    auto argt = std::make_tuple(pos, args...);          \
    auto memoized = PASTE3(__, f, _table)<R, Args...>.find(argt);    \
    if (memoized == PASTE3(__, f, _table)<R, Args...>.end()) {       \
        auto result = _ ## f(args...);                  \
        PASTE3(__, f, _table)<R, Args...>[argt] = result;            \
        return result;                                  \
    }                                                   \
    return std::any_cast<R>(memoized->second);          \
}

class someClass {
public:
    std::string x = "abc ";
    
    MEMOIZE(some_function)
    int _some_function(int param){
        std::cout << x << "someClass::some_function called\n";
        return param + 1;
    }

    MEMOIZE(fibonacci);
    long long _fibonacci(int n){
        if (n <= 1) return n;
        return fibonacci(n - 2) + fibonacci(n - 1);
    }

    //decltype(memoize(&_ ##f)) f = memoize(&_ ##f)
    //template<typename... Args>
    //auto memoized_some_function(Args... args){
    //    static auto memoized = memoize_class_func(_some_function);
    //    return memoized(this, args...);
    //}
};

template <typename R, typename C, typename... Args>
std::map<std::tuple<int, Args...>, R> construct_memoization_table(R (C::*fn)(Args...)){
    return std::map<std::tuple<int, Args...>, R>{};
}

#define MEMOIZE_POS(f) \
std::any PASTE3(__, f, _table) = construct_memoization_table(PASTE(_, f));          \
template<typename... Args>                                                          \
auto some_statement(Args... args){                                                  \
    using R = decltype(PASTE(_, f)(args...));                                       \
    using table_t = decltype(construct_memoization_table(PASTE(_, f)));             \
    auto table = std::any_cast<table_t>(PASTE3(__, f, _table));                     \
    auto pos = mark();                                                              \
    auto argt = std::make_tuple(pos, args...);                                      \
    auto memoized = table.find(argt);                                               \
    if (memoized == table.end()) {                                                  \
        auto result = PASTE(_, f)(args...);                                         \
        table[argt] = result;                                                       \
        return result;                                                              \
    }                                                                               \
    return memoized->second;                                                        \
}

class _Parser {
public:
    int mark(){
        return 5;
    }

    std::optional<std::shared_ptr<int>> base_statement(){
        // do something
        return std::make_shared<int>(12345);
    }

    //MEMOIZE_ON_POS(some_statement)
    /*
    std::any __some_statement_table = construct_memoization_table(_some_statement);
    
    template<typename... Args>                              
    auto some_statement(Args... args){                                   
        using R = decltype(_some_statement(args...));    
        using table_t = decltype(construct_memoization_table(_some_statement));      
        auto table = std::any_cast<table_t>(__some_statement_table);     
        auto pos = mark();                                  
        auto argt = std::make_tuple(pos, args...);          
        auto memoized = table.find(argt);    
        if (memoized == table.end()) {       
            auto result = _some_statement(args...);                  
            table[argt] = result;            
            return result;                                  
        }                                                   
        return memoized->second;          
    }
    */
    MEMOIZE_POS(some_statement)
    std::optional<std::shared_ptr<int>> _some_statement(){
        std::cout << "some_statement called\n";
        // do something
        return std::make_shared<int>(12345);
    }
};

int some_function(int param){
    std::cout << "some_function called\n";
    return param + 1;
}

int other_function(int param){
    std::cout << "other_function called\n";
    return 123;
}

long long fib(int n){
    //std::cout << "fibonacci " << n << "\n";
    if (n <= 1) return n;
    return fib(n - 2) + fib(n - 1);
}

long long _fibonacci(int);
MEMOIZE(fibonacci)
inline long long _fibonacci(int n){
    //std::cout << "fibonacci " << n << "\n";
    if (n <= 1) return n;
    return fibonacci(n - 2) + fibonacci(n - 1);
}

long long int new_fib(int n);
s_memoize struct_fib(new_fib);
long long int new_fib(int n){
    if (n <= 1) return n;
    return struct_fib(n - 2) + struct_fib(n - 1);
}


int main(){
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    
    someClass c;
    
    std::cout << c.some_function(1) << "\n";
    std::cout << c.some_function(1) << "\n";

    auto t1 = high_resolution_clock::now();
    fib(32);
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    std::cout << "normal:\n" << ms_double.count() << "ms" << "\n";
    
    t1 = high_resolution_clock::now();
    fibonacci(1024);
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    std::cout << "memoized:\n" << ms_double.count() << "ms" << "\n";

    t1 = high_resolution_clock::now();
    struct_fib(1024);
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    std::cout << "struct memoized:\n" << ms_double.count() << "ms" << "\n";

    t1 = high_resolution_clock::now();
    c.fibonacci(1024);
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    std::cout << "class memoized:\n" << ms_double.count() << "ms" << "\n";

    _Parser p;
    //std::cout << p.some_statement().value() << "\n";
    //std::cout << p.some_statement().value() << "\n";
    return 0;
}
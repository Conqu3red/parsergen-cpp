#pragma once
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <map>
#include <variant>
#include <memory>

namespace JSON {

class JSONReadException : public std::runtime_error {
public:
    JSONReadException()
        : runtime_error("Incorrect type")
    {}
};

class JsonObject;

// Union implementation
enum NodeType {
    Object,
    Array,
    String,
    Number,
    Bool,
    Null
};

class Value;
typedef std::vector<Value> JsonArray;

class Value {
public:
    NodeType GetType() { return type; }
    
    Value(std::shared_ptr<JsonObject> object) : val(object), type(Object) {}
    Value(std::shared_ptr<JsonArray> array) : val(array), type(Array) {}
    Value(std::string string) : val(string), type(String) {}
    Value(double number) : val(number), type(Number) {}
    Value(bool boolean) : val(boolean), type(Bool) {}
    Value() : type(Null) {}

    auto as_object() {
        if (type != Object)
            throw new JSONReadException;
        return std::get<std::shared_ptr<JsonObject>>(val);
    }

    auto as_array() {
        if (type != Array)
            throw new JSONReadException;
        return std::get<std::shared_ptr<JsonArray>>(val);
    }

    auto as_string() {
        if (type != String)
            throw new JSONReadException;
        return std::get<std::string>(val);
    }

    auto as_number() {
        if (type != Number)
            throw new JSONReadException;
        return std::get<double>(val);
    }

    auto as_boolean() {
        if (type != Bool)
            throw new JSONReadException;
        return std::get<bool>(val);
    }

    bool is_null() {
        return type == Null;
    }
private:
    std::variant<std::shared_ptr<JsonObject>, std::shared_ptr<JsonArray>, std::string, double, bool> val;
    NodeType type;
};

static Value null_value {};

typedef std::pair<std::string, Value> Member;

class JsonObject {
public:
    std::vector<Member> members;
    JsonObject(std::vector<Member> members) : members(members){}
    Value &get(std::string key){
        for (auto &[k, v] : members){
            if (key == k)
                return v;
        }
        return null_value;
    }
    Value &operator[](std::string key){
        return get(key);
    }
};

};
#pragma once
#include <algorithm> 
#include <cctype>
#include <locale>
#include <string_view>
#include <regex>
#include <string>
#include <functional>
#include <memory>
#include <set>
#include <iostream>


namespace Parsergen {
namespace utils {

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

// regex string_view
using svmatch = std::match_results<std::string_view::const_iterator>;
using svsub_match = std::sub_match<std::string_view::const_iterator>;

inline std::string_view get_sv(const svsub_match &m) {
    return std::string_view(m.first, m.length());
}

inline bool regex_match(
    std::string_view sv,
    svmatch &m,
    const std::regex &e,
    std::regex_constants::match_flag_type flags = std::regex_constants::match_default
){
    return std::regex_match(sv.begin(), sv.end(), m, e, flags);
}

inline bool regex_match(
    std::string_view sv,
    const std::regex &e,
    std::regex_constants::match_flag_type flags = std::regex_constants::match_default
){
    return std::regex_match(sv.begin(), sv.end(), e, flags);
}

inline bool regex_search(
    std::string_view sv,
    svmatch &m,
    const std::regex &e,
    std::regex_constants::match_flag_type flags = std::regex_constants::match_default
){
    return std::regex_search(sv.begin(), sv.end(), m, e, flags);
}

inline bool regex_search(
    std::string_view sv,
    const std::regex &e,
    std::regex_constants::match_flag_type flags = std::regex_constants::match_default
){
    return std::regex_search(sv.begin(), sv.end(), e, flags);
}

/// Returns a version of 'str' where every occurrence of
/// 'find' is substituted by 'replace'.
/// - Inspired by James Kanze.
/// - http://stackoverflow.com/questions/20406744/
static std::string ReplaceAll(
    const std::string & str ,   // where to work
    const std::string & find ,  // substitute 'find'
    const std::string & replace //      by 'replace'
) {
    using namespace std;
    string result;
    size_t find_len = find.size();
    size_t pos,from=0;
    while ( string::npos != ( pos=str.find(find,from) ) ) {
        result.append( str, from, pos-from );
        result.append( replace );
        from = pos + find_len;
    }
    result.append( str, from , string::npos );
    return result;
/*
    This code might be an improvement to James Kanze's
    because it uses std::string methods instead of
    general algorithms [as 'std::search()'].
*/
}

static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static bool startsWith(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

static std::string normalizeNewlines(std::string string){
    string = ReplaceAll(string, "\r\n", "\n"); // windows -> unix
    string = ReplaceAll(string, "\r", "\n"); // mac -> unix
    return string;
}

template <typename T>
void set_union(std::set<T>& a, std::set<T> b)
{
    a.insert(b.begin(), b.end());
}

template <typename T>
std::set<T> set_intersection(std::set<T> &a, std::set<T> &b)
{
    std::set<T> result;
    for (auto l : a){
        if (b.find(l) != b.end()) result.insert(l);
    }
    return result;
}

template <typename T>
inline bool contains(std::vector<T> &vec, T &val){
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

template <typename T>
inline bool contains(std::vector<T> &vec, const T &val){
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

}
}
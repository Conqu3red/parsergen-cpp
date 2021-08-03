#include <algorithm> 
#include <cctype>
#include <locale>
#include <string_view>
#include <regex>
#include <string>
#include <functional>

#pragma once

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

static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static bool startsWith(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

}
}
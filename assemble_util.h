#pragma once


/* Utility functions for assembler */

#ifndef ASSEMBLE_UTIL
#define ASSEMBLE_UTIL


#include <vector>
#include <string>

using namespace std;

// split a string with greedy strategy, i.e. multiple same
// char will be ignored together
vector<string> split(const string& str, char s) {
	vector<string> result;

	string buffer;
	for (const char& c : str) {
		if (c == s) {
			if (!buffer.empty()) {
				result.push_back(buffer);
				buffer.clear();
			}
		}
		else {
			buffer.push_back(c);
		}
	}
	if (!buffer.empty()) {
		result.push_back(buffer);
	}
	return result;
}

// remove char in head and tail
string trim(const string& str, char s=' ') {
    auto begin = str.find_first_not_of(s);
    if (begin == string::npos)return "";
    return str.substr(begin, str.find_last_not_of(s) - begin + 1);
}

string remove_comment(const string& str, char comment_begin = ';') {
    return string(str.begin(), find(str.begin(), str.end(), comment_begin));
}

// test if str starts with str_start
bool starts_with(const string& str, const string& str_start) {
    for (auto iter_1 = str.begin(), iter_2 = str_start.begin(); iter_2 != str_start.end(); iter_1++, iter_2++) {
        if (iter_1 == str.end() || *iter_1 != *iter_2) {
            return false;
        }
    }
    return true;
}

// is a str starts with digit
bool is_digit(const string& str) {
    return (str[0] >= '0' && str[0] <= '9') || str[0] == '-';
}

#endif // !ASSEMBLE_UTIL

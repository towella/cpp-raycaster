#pragma once

#include <vector>
#include <string>
#include <cstdlib>

// MacOS/Linux specific terminal clear. Will not work on Windows
void clear() {
    system("clear");
}

// MARK: ---- VECTOR -------------------------------------------------------------------

template <typename data>
bool contains(const std::vector<data>& v, const data& d) {
    return std::count(v.begin(), v.end(), d) != 0;
}

template <typename data>
int getValueIndex(const std::vector<data>& v, const data& d) {
    ptrdiff_t index = std::find(v.begin(), v.end(), d) - v.begin();
    return (index < v.size()) ? index : -1;
}

template <typename data>
void popAt(std::vector<data>& v, const int& i) {
    if (i < v.size() && i >= 0) {
        v.erase(v.begin() + i);
    }
}

template <typename data>
void popAllOfValue(std::vector<data>& v, const data& d) {
    v.erase(remove(v.begin(), v.end(), d), v.end());
}
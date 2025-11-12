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

// MARK: ---- ANGLES ---------------------------------------------------------------------

// brings angle to be between 0 and 2PI
double wrapRadAngle(double angle) {
    while (angle <= 0 || angle > 2*M_PI) {
        if (angle > 2*M_PI) {
            angle = fmod(angle, 2*M_PI);
        } else if (angle <= 0) {
            angle = 2*M_PI + angle;
        }
    }
    return angle;
}
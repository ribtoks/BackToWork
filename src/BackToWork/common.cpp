#include "common.h"
#include <algorithm>
#include <string>

char easytolower(char in) {
    if (in <= 'Z' && in >= 'A') {
        return in - ('Z' - 'z');
    }

    return in;
}

void tolower(std::string &data) {
    std::transform(data.begin(), data.end(), data.begin(), easytolower);
}

#include "appconfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <iterator>

#include "logger.h"
#include "common.h"
#include "winkeys.h"

#define APP_KEY "app"
#define MIN_WINDOWS_KEY "min_windows"
#define HOTKEY_KEY "hotkey"

const char* ws = " \t\n\r\f\v";

std::string& rtrim(std::string& s, const char *t = ws) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string& ltrim(std::string& s, const char *t = ws) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (left & right)
std::string& trim(std::string& s, const char *t = ws) {
    return ltrim(rtrim(s, t), t);
}

AppConfig::AppConfig(const std::string& pathToConfig):
    m_PathToConfig(pathToConfig),
    m_WindowsToActivate(1)
{
    LOG << "Using config" << pathToConfig;
    m_HotKeyModifier = KEY_SHIFT | KEY_CONTROL;
    m_HotKey = KEY_0;
}

std::map<std::string, int> stringToKey = {
    {"0", KEY_0},
    {"1", KEY_1},
    {"2", KEY_2},
    {"3", KEY_3},
    {"4", KEY_4},
    {"5", KEY_5},
    {"6", KEY_6},
    {"7", KEY_7},
    {"8", KEY_8},
    {"9", KEY_9},

    {"a",  KEY_A },
    {"b",  KEY_B },
    {"c",  KEY_C },
    {"d",  KEY_D },
    {"e",  KEY_E },
    {"f",  KEY_F },
    {"g",  KEY_G },
    {"h",  KEY_H },
    {"i",  KEY_I },
    {"j",  KEY_J },
    {"k",  KEY_K },
    {"l",  KEY_L },
    {"m",  KEY_M },
    {"n",  KEY_N },
    {"o",  KEY_O },
    {"p",  KEY_P },
    {"q",  KEY_Q },
    {"r",  KEY_R },
    {"s",  KEY_S },
    {"t",  KEY_T },
    {"u",  KEY_U },
    {"v",  KEY_V },
    {"w",  KEY_W },
    {"x",  KEY_X },
    {"y",  KEY_Y },
    {"z",  KEY_Z },

    {"left", KEY_LEFT},
    {"right", KEY_RIGHT}
};

std::map<std::string, int> stringToModifier = {
    {"shift", KEY_SHIFT},
    {"ctrl", KEY_CONTROL},
    {"alt", KEY_ALT}
};

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

bool parseHotKey(const std::string &value, int &modifierOut, int &keyOut) {
    std::vector<std::string> parts = split(value, '+');

    int modifier = 0, key = 0;
    bool anyMissing = false;
    auto keysEnd = stringToKey.end();
    auto modifiersEnd = stringToModifier.end();

    for (auto &part: parts) {
        auto it = stringToKey.find(part);
        if (it != keysEnd) {
            key |= it->second;
        } else {
            auto modIt = stringToModifier.find(part);
            if (modIt != modifiersEnd) {
                modifier |= modIt->second;
            } else {
                anyMissing = true;
                break;
            }
        }
    }

    if (!anyMissing) {
        modifierOut = modifier;
        keyOut = key;
    }

    return !anyMissing;
}

bool AppConfig::parse() {
    std::ifstream configStream(DEFAULT_CONFIG_NAME);
    if (!configStream.good()) {
        return false;
    }

    for (;;) {
        std::string line;
        std::getline(configStream, line);

        if (!configStream) { break; }
        if (line.empty() || line[0] == '#') { continue; }

        std::string key;
        std::istringstream isLine(line);

        if (std::getline(isLine, key, '=')) {
            std::string value;

            if (std::getline(isLine, value)) {
                processConfigKeyValue(key, value);
            }
        }
    }

    bool result = configStream.eof();
    return result;
}

void AppConfig::initDefaults() {
    LOG << "Initializing the default list of apps...";

    m_PriorityApps.insert(m_PriorityApps.end(),
    { "qt creator"
      , "visual studio"
      , "emacs"
      , "sublime"
      , "atom"
      , "notepad++"
      , "code"
                          });
}

void AppConfig::processConfigKeyValue(std::string &key, std::string &value) {
    trim(key); trim(value);

    if (key.empty() || value.empty()) {
        LOG << "Skipping empty key or value" << key << value;
        return;
    }

    if (key[0] == '#') { return; }

    if (key == APP_KEY) {
        tolower(value);
        m_PriorityApps.push_back(value);
        LOG << "Registered" << value << "as an App";
    } else if (key == MIN_WINDOWS_KEY) {
        m_WindowsToActivate = std::stoi(value);
        LOG << value << "minimum windows to activate";
    } else if (key == HOTKEY_KEY) {
        tolower(value);
        if (parseHotKey(value, m_HotKeyModifier, m_HotKey)) {
            LOG << "Found" << value << "as a hotkey";
        } else {
            LOG << "[Error] Cannot parse a hotkey.";
        }
    }
}

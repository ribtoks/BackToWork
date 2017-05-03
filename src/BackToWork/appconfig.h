#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <string>
#include <vector>

#define DEFAULT_CONFIG_NAME "btw.config"

class AppConfig
{
public:
    AppConfig(const std::string &pathToConfig);

public:
    bool parse();
    void initDefaults();

public:
    const std::vector<std::string> &getApps() const { return m_PriorityApps; }
    int getWindowsToActivate() const { return m_WindowsToActivate; }
    int getHotKey() const { return m_HotKey; }
    int getHotKeyModifier() const { return m_HotKeyModifier; }

private:
    void processConfigKeyValue(std::string &key, std::string &value);

private:
    std::string m_PathToConfig;
    std::vector<std::string> m_PriorityApps;
    int m_WindowsToActivate;
    int m_HotKeyModifier;
    int m_HotKey;
};

#endif // APPCONFIG_H
